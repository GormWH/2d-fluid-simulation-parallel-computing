/*
 * DataException.h
 */

#ifndef _DATA_EXCEPTION_H
#define _DATA_EXCEPTION_H

#include <iostream>
#include <sstream>

/*
 * 入力データの中の問題を伝達するための例外クラス
 * エラーに遭遇したソースファイルのパス名、行番号と、問題の内容を説明した文字列を保持する。
 * エラーが見出された入力ファイルの名称や行番号は msg_str_ に格納する。
 *
 * catch したら、std::cout などに << でメッセージを出力させることができる。
 */
class DataException {
public:

    // 例外を投げるソースファイルのパス名
    const char *source_file_;

    // ソースファイルの行番号
    int source_line_;

    // エラーを説明するメッセージ。
    std::string msg_str_;

    // コンストラクタ
    // file : エラーに遭遇したソースファイルのパス名。呼び出し箇所では組み込みマクロ変数 __FILE__ を引数に渡す。
    // line : エラーに遭遇したソースファイルのパス名。呼び出し箇所では組み込みマクロ変数 __LINE__ を引数に渡す。
    // msg_str : エラーを説明するメッセージ。ここにデータファイルのパス名、行番号、エラーの性質などをなるべく詳細に記述する。
    // 例：「ファイル "abc.dat" の 104 行めの要素98番は、面積がゼロです。頂点は #100(0,0),#101(1,0),#102(2,0),#103(3,0) です。」
    DataException(const char *file, int line, const std::string &msg_str) {
        source_file_ = file;
        source_line_ = line;
        msg_str_ = msg_str;
    }

    // streamに例外のメッセージを出力するメソッド。 operator<< の中から呼ぶ。
    void writeTo(std::ostream &os) const {
        os << "DataException : " << msg_str_;
        os << ", at \"" << source_file_ << "\", line " << source_line_ << " ";
    }
};

// ストリームに例外のメッセージを出力するオペレータ。
// ヘッダの中に関数定義を書いているので、staticリンケージを指定しないと重複関数定義エラー
// になることに注意。
static inline std::ostream &operator<<(std::ostream &os, const DataException &exp) {
    exp.writeTo(os);
    return os;
}

#endif
