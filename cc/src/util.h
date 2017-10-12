/**
 * @file util.h
 * @brief 文字列，プロセスなどの雑多なライブラリ
 * @date 2017-10-12
 */
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>
#include <boost/exception/all.hpp>

class io_error : public boost::exception, public std::exception { };
class parse_error: public boost::exception, public std::exception { };
class invalid_operation: public boost::exception, public std::exception { };
class process_error: public boost::exception, public std::exception { };
class not_implemented: public boost::exception, public std::exception { };

/** @class util
 * @brief 文字列，プロセスなどの雑多なライブラリ
 */
class util {
public:
    /**
     * @brief string から u32string へ変換
     * @param u8s 入力
     * @return 出力
     */
    static std::u32string u8tou32(const std::string& u8s);

    /**
     * @brief u32string から string へ変換
     * @param u32s 入力
     * @return 出力
     */
    static std::string u32tou8(const std::u32string& u32s);

    /**
     * @brief char *からu32stringへ変換
     */
    static std::u32string charstou32(const char *chars);

    /**
     * @brief char32_t から string へ変換
     * @param c32 入力
     * @return 出力
     */
    static std::string c32tou8(const char32_t& c32);

    /**
     * @brief file descriptor から u32string (char32_t) へ変換
     * @param fd 入力
     */
    static std::u32string fdtou32(const int fd);
    
    /**
     * @brief file descriptor から char * へ変換
     * @param fd 入力
     */
    static char* fdtochar(const int fd);

    /** @brief 文字のターミナル幅を返す
     * @param c32 入力
     * @return ターミナル幅
     * @retval 1 半角
     * @retval 2 全角
     */
    static size_t twidth(const char32_t c32);

    /** @brief システムのエンディアンを取得する
     * @retval true リトルエンディアン
     * @retval false ビッグエンディアン
     */
    static bool endian();

    /**
     *  @brief 文字列をdelimで分割する
     *  @param s 文字列
     *  @param delim 区切り
     *  @return delimで分割された文字列
     *  @pre Tがstd::charT以外のときのコンパイル結果およびその動作は未定義である．
     */
    template <typename T>
    static std::vector<std::basic_string<T>> split(
        const std::basic_string<T> &s, 
        const std::basic_string<T> &delim) {
        std::vector<std::basic_string<T>> ret;
        std::basic_string<T> item;
        size_t i = 0;
        size_t delimsize = delim.size();
        for (T c: s) {
            if (c == delim[i]) {
                ++i;
                if (delimsize == i) {
                    if (!item.empty()) {
                        ret.push_back(item);
                    }
                    item.clear();
                    i = 0;
                }
            } else {
                i = 0;
                item += c;
            }
        }
        if (!item.empty()) {
            ret.push_back(item);
        }
        return ret;
    }

     /**
     *  @breif 文字列sが文字列startで始まるかどうかを判定する

     *  @param s 文字列
     *  @param start 開始すべき文字列
     *  @return 判定
     *  @retval true startで始まる
     *  @retval false startで始まらない
     *  @pre Tがstd::charT以外のときのコンパイル結果およびその動作は未定義である．
     */
    template <typename T>
    static bool startswith(
        const std::basic_string<T> &s,
        const std::basic_string<T> &start) {
        if (s.size() < start.size()) {
            return false;
        }
        size_t startsize = start.size();
        for (size_t i = 0; i < startsize; i++) {
            if (s[i] != start[i]) {
                return false;
            }
        }
        return true;
    }

    /** @brief プロセスを実行
     * @param command コマンドライン
     * @param in stdin
     * @param poutr stdout file descriptor
     * @param perrr stderr file descriptor
     */
    static int execute_base(
        char *const *command, 
        const char *in, 
        int& poutr,
        int& perrr);

    /** @brief プロセスを実行
     * @param command コマンドライン
     * @param in stdin
     * @param out stdout
     * @param out stderr
     */
    static int execute_low(
        char *const *command, 
        const char *in, 
        char*& out, 
        char*& err);

    /** @brief プロセスを実行
     * @param command コマンドライン
     * @param in stdin
     * @param out stdout
     * @param err stderr
     */
    static int execute(
        char * const *command,
        const std::u32string& in,
        std::u32string& out,
        std::u32string& err);
};

