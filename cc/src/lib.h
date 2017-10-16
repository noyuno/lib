/**
 * @file lib.h
 * @brief 文字列，プロセスなどの雑多なライブラリ
 * @date 2017-10-12
 */
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>
#include <boost/exception/all.hpp>
#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
class io_error : public boost::exception, public std::exception { };
class parse_error: public boost::exception, public std::exception { };
class invalid_operation: public boost::exception, public std::exception { };
class process_error: public boost::exception, public std::exception { };
class not_implemented: public boost::exception, public std::exception { };

/** @namespace lib
 * @brief 文字列，プロセスなどの雑多なライブラリ
 */
namespace lib {
    /**
     * @brief string から u32string へ変換
     * @param u8s 入力
     * @return 出力
     */
    std::u32string u8tou32(const std::string& u8s);

    /**
     * @brief u32string から string へ変換
     * @param u32s 入力
     * @return 出力
     */
    std::string u32tou8(const std::u32string& u32s);

    /**
     * @brief char *からu32stringへ変換
     */
    std::u32string charstou32(const char *chars);

    /**
     * @brief char32_t から string へ変換
     * @param c32 入力
     * @return 出力
     */
    std::string c32tou8(const char32_t& c32);

    /**
     * @brief file descriptor から u32string (char32_t) へ変換
     * @param fd 入力
     */
    std::u32string fdtou32(const int fd);
    
    /**
     * @brief file descriptor から char * へ変換
     * @param fd 入力
     */
    char* fdtochar(const int fd);

    /** @brief システムのエンディアンを取得する
     * @retval true リトルエンディアン
     * @retval false ビッグエンディアン
     */
    inline bool endian() {
        char32_t e = 1;
        return *(char *)&e ? true : false;
    }

    /** @brief 文字のターミナル幅を返す
     * @param c32 入力
     * @return ターミナル幅
     * @retval 1 半角
     * @retval 2 全角
     */
    inline size_t twidth(const char32_t c32) {
        char32_t c;
        // 半角カタカナ 0000 ff61 から 0000 ff9f まで
        if (c32 >= 0x0000'ff61 && c32 <= 0x0000'ff9f) {
            return 1;
        }
        if (endian()) {
            c = (c32 >> 8);
        } else {
            c = (c32 << 8);
        }
        return c ? 2 : 1;
    }

    /**
     *  @brief 文字列をdelimで分割する
     *  @param s 文字列
     *  @param delim 区切り
     *  @return delimで分割された文字列
     *  @pre Tがstd::charT継承クラス(char32_tなど)以外のときのコンパイル結果およびその動作は未定義である．
     */
    template <typename T = char32_t>
    inline std::vector<std::basic_string<T>> split(
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
     *  @pre Tがstd::charT継承クラス(char32_tなど)以外のときのコンパイル結果およびその動作は未定義である．
     */
    template <typename T = char32_t>
    inline bool startswith(
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

    /**
     * @brief 文字列中sに含まれる文字列beforeを文字列afterで置換したものを返す
     * @param s 文字列
     * @param before 前の文字列
     * @param after 後の文字列
     * @return 置換された文字列
     */
     template <typename T = char32_t>
     inline T replace(
        const std::basic_string<T> s, 
        const std::basic_string<T> &before, 
        const std::basic_string<T> &after) {
        auto pos = s.find(before);
        while (pos !=  std::string::npos) {
            s.replace(pos, before.size(), after);
            pos = s.find(before, pos + after.size());
        }
        return s;
     }

    /**
     * @brief 文字列s中のaとbで囲まれたものをstd::vectorで返す
     * @param s 文字列
     * @param a 初めの文字列
     * @param b 終わりの文字列
     * @return 構造化された文字列
     * @pre 文字列sの中にaが連続しているときは，連続した部分中最後のaのみを取り扱い，それ以外は無視して戻り値にも含めない．
     * 文字列sの中でbで終わっていない部分は，無視して戻り値にも含めない．
     * Tは文字の型，F(cond)(std::basic_string<T>)は条件を返す関数
     */
    template <typename T = char32_t, class F>
    inline std::vector<std::basic_string<T>> split2(
        const std::basic_string<T>& s, 
        const std::basic_string<T>& a, 
        const std::basic_string<T>& b, 
        const F cond = nullptr) {
        std::string::size_type p = 0;
        std::vector<std::basic_string<T>> ret;
        ret.reserve(32);
        while (true) {
            auto pb = s.find(b, p + 1);
            auto pa = s.rfind(a, pb);
            if (pa == std::string::npos || pb == std::string::npos || p > pa) {
                break;
            }
            std::basic_string<T> c = s.substr(pa + 1, pb - pa - 1);
            if (cond == nullptr || cond(c)) {
                ret.push_back(c);
            }
            p = pb;
        }
        return ret;
    }

    /**
     * @brief 文字列strを数値として読み取って，int型の値に変換する
     * @param str 文字列
     * @param idx 実際に使用されなかった要素のインデックス
     * @param base 基数
     * @return 数値
     */
    inline int stoi(const std::u32string& str, size_t* idx = nullptr, int base = 10) {
        try {
            return std::stoi(u32tou8(str), idx, base);
        } catch (...) {
            throw;
        }
    }

    /** @brief プロセスを実行
     * @param command コマンドライン
     * @param in stdin
     * @param poutr stdout file descriptor
     * @param perrr stderr file descriptor
     * @return プロセスの戻り値
     */
    int execute_base(
        char *const *command, 
        const char *in, 
        int& poutr,
        int& perrr);

    /** @brief プロセスを実行
     * @param command コマンドライン
     * @param in stdin
     * @param out stdout
     * @param out stderr
     * @return プロセスの戻り値
     */
    int execute_low(
        char *const *command, 
        const char *in, 
        char*& out, 
        char*& err);

    /** @brief プロセスを実行
     * @param command コマンドライン
     * @param in stdin
     * @param out stdout
     * @param err stderr
     * @return プロセスの戻り値
     */
    int execute(
        char * const *command,
        const std::u32string& in,
        std::u32string& out,
        std::u32string& err);
};

