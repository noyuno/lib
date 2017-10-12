#include "util.h"
#include <codecvt>
#include <uchar.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

std::u32string util::u8tou32(const std::string& u8s) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    return converter.from_bytes(u8s);
}

std::string util::u32tou8(const std::u32string& u32s) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    return converter.to_bytes(u32s);
}

std::u32string util::charstou32(const char *chars) {
    return u8tou32(std::string(chars));
}

std::string util::c32tou8(const char32_t& c32) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    return converter.to_bytes(c32);
}

std::u32string util::fdtou32(const int fd) {
    std::vector<char> b;
    b.reserve(1024);
    char c = 0;
    while (read(fd, &c, 1)) {
        b.push_back(c);
    }
    return util::u8tou32(std::string(b.begin(), b.end()));
}

char *util::fdtochar(const int fd) {
    const size_t size = 1024;
    char *b = (char *)malloc(size);
    char *c = NULL;
    int ret = 0;
    int readed = 0;
    while ((ret = read(fd, b + readed, size)) == size) {
        readed += size;
        if ((c = (char *)realloc(b, readed + size)) == NULL) {
            std::cout << "errno: " << errno << std::endl;
            free(b);
            BOOST_THROW_EXCEPTION(io_error());
        }
        b = c;
    }
    b[readed + ret] = 0;
    return b;
}

size_t util::twidth(const char32_t c32) {
    char32_t c;
    // 半角カタカナ 0000 ff61 から 0000 ff9f まで
    if (c32 >= 0x0000'ff61 && c32 <= 0x0000'ff9f) {
        return 1;
    }
    if (util::endian()) {
        c = (c32 >> 8);
    } else {
        c = (c32 << 8);
    }
    return c ? 2 : 1;
}

bool util::endian() {
    char32_t e = 1;
    return *(char *)&e ? true : false;
}

int util::execute_base(
    char * const *command,
    const char *in,
    int& poutr,
    int& perrr) {
    const int r = 0;
    const int w = 1;
    int pipein[2], pipeout[2], pipeerr[2];
    int pid;
    int ret;

    if (pipe(pipein) < 0) {
        BOOST_THROW_EXCEPTION(process_error());
    }
    if (pipe(pipeout) < 0) {
        close(pipein[r]);
        close(pipein[w]);
        BOOST_THROW_EXCEPTION(process_error());
    }
    if (pipe(pipeerr) < 0) {
        close(pipein[r]);
        close(pipein[w]);
        close(pipeout[r]);
        close(pipeout[w]);
        BOOST_THROW_EXCEPTION(process_error());
    }
    
    /* invoke process */
    if ((pid = fork()) < 0) {
        close(pipein[r]);
        close(pipein[w]);
        close(pipeout[r]);
        close(pipeout[w]);
        close(pipeerr[r]);
        close(pipeerr[w]);
        BOOST_THROW_EXCEPTION(process_error());
    }

    if (pid == 0) {
        /* child process */
        close(pipein[w]);
        close(pipeout[r]);
        close(pipeerr[r]);
        dup2(pipein[r], STDIN_FILENO);
        dup2(pipeout[w], STDOUT_FILENO);
        dup2(pipeerr[w], STDERR_FILENO);
        close(pipein[r]);
        close(pipeout[w]);
        close(pipeerr[w]);
        if (execvp(command[0], command) < 0) {
            close(pipein[r]);
            close(pipeout[w]);
            close(pipeerr[w]);
            exit(1);
        }
    }

    /* parent process */

    close(pipein[r]);
    close(pipeout[w]);
    close(pipeerr[w]);

    // write to stdin
    if (write(pipein[w], in, strlen(in)) < 0) {
        std::cout << "errno: " << errno << std::endl;
        close(pipein[w]);
        close(pipeout[r]);
        close(pipeerr[r]);
        BOOST_THROW_EXCEPTION(io_error());
    }

    close(pipein[w]);

    waitpid(pid, &ret, WUNTRACED);
    ret >>= 8;

    // get stdout, stderr
    poutr = pipeout[r];
    perrr = pipeerr[r];
    return ret;
}

int util::execute_low(
    char * const *command,
    const char *in,
    char*& out,
    char*& err) {
    int poutr = 0;
    int perrr = 0;
    int ret = execute_base(command, in, poutr, perrr);
    out = util::fdtochar(poutr);
    err = util::fdtochar(perrr);
    return ret;
}

int util::execute(
    char * const *command,
    const std::u32string& in,
    std::u32string& out,
    std::u32string& err) {
    std::string ins = u32tou8(in);
    const char *cin = ins.c_str();
    int poutr = 0;
    int perrr = 0;
    int ret = execute_base(command, cin, poutr, perrr);
    out = util::fdtou32(poutr);
    err = util::fdtou32(perrr);
    return ret;
}

