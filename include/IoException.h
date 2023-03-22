/*
 * IoException.h
 */
#ifndef _IO_EXCEPTION_H
#define _IO_EXCEPTION_H

#include <string>
#include <iostream>
#include <cerrno>
#include <cstring>

/*
 * 入出力操作の失敗を伝達するための例外クラス
 * 入出力の失敗はOSのシステムコールの失敗の形を取るので、システムコールのエラー番号 errno も記録する。
 * エラーに遭遇したソースファイルのパス名、行番号と、errno、そしてエラーを引き起こしたファイル名を保持する。
 *
 * catch したら、std::cout などに << でメッセージを出力させることができる。
 */
class IoException {
    // エラーに遭遇したソースファイルのファイル名
    const char *source_file_;

    // エラーに遭遇したソースファイルの行数
    int source_line_;

    // 自動的にエラーコードが設定される errno のコピー
    int errno_;

    // エラーを引き起こしたファイルの名前
    std::string name_;

public:

    // コンストラクタ
    // source_file : エラーに遭遇したソースファイルのパス名。呼び出し箇所では組み込みマクロ変数 __FILE__ を引数に渡す。
    // source_line : エラーに遭遇したソースファイルのパス名。呼び出し箇所では組み込みマクロ変数 __LINE__ を引数に渡す。
    // name : エラーを引き起こしたものの名称。通常はパス名。
    IoException(const char *source_file, int source_line, const char *name) {
        source_file_ = source_file;
        source_line_ = source_line;
        name_ = name;
        errno_ = errno; // 右辺はOSから渡される、直前に起きたエラーのエラー番号。みかけは変数だが実際には複雑なマクロ。
    }

    IoException(const char *source_file, int source_line, const std::string &name) {
        source_file_ = source_file;
        source_line_ = source_line;
        name_ = name;
        errno_ = errno; // 右辺はOSから渡される、直前に起きたエラーのエラー番号。みかけは変数だが実際には複雑なマクロ。
    }

    // streamに例外のメッセージを出力するメソッド。 operator<< の中から呼ぶ。
    void writeTo(std::ostream &os) const {
        // strerror は、errno のエラー番号に対する説明文を返す関数
        os << "IoException : " << strerror(errno_) << ":" << name_;
        os << ", at " << source_file_ << "(" << source_line_ << ")\n";
    }
};

// ストリームに例外のメッセージを出力するオペレータ。
// ヘッダの中に関数定義を書いているので、staticリンケージを指定しないと重複関数定義エラー
// になることに注意。
static inline std::ostream &operator<<(std::ostream &os, const IoException &exp) {
    exp.writeTo(os);
    return os;
}

#endif
