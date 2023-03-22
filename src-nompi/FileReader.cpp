/*
 * FileReader.cpp
 */
#include <FileReader.h>
#include <Logger.h>

FileReader::FileReader() {

}

FileReader::~FileReader() {
    // ファイルからの読み込み処理の途中で例外を挙げる場合などにおいて、
    // FileReader::close() メソッドが呼ばれる前にデストラクタが
    // 起動された状況に備えて、入力用のストリームを念のためにクローズする。
    if (in_.is_open()) {
        in_.close();
    }
}


void FileReader::open(const char *file_name) {
    // ファイル名はファイルを開くための引数として必要なだけではなく、
    // 本クラスの各種readメソッドにおいてエラーメッセージに含めたい。
    // そのために、ファイル名をメンバ変数に保存しておく。
    file_name_ = file_name;  // 左辺はメンバ変数、右辺はローカル変数

    // fstream::openを呼ぶ
    in_.open(file_name_.c_str(), std::ios::in);
    // 成功したか?
    if (! in_.is_open()) {
        // 失敗した。例外を挙げる。
        // throw コンストラクタ呼び出し の構文であることに注意。
        // 無名のインスタンスが作成され、それがthrowされる。
        throw IoException(__FILE__, __LINE__, file_name_);
    }
    // 成功した。今後のエラーが生じてエラーメッセージを出す状況に備えて
    // ファイルの中の行番号を数えていく。そのカウンタを初期化する。
    line_no_ = 0;
    Logger::out << "File opened: " << file_name_ << std::endl;
}

void FileReader::close() {
    in_.close();
    Logger::out << "File closed:" << file_name_ << std::endl;
}

void FileReader::readLine() {
    std::string line_buf;

    // 改行文字まで含めて line_buf に一行読み込む。
    std::getline(in_, line_buf);
    // 一行データを読んだので、カウンタを進める。
    line_no_++;

    // stringstream オブジェクトである cur_line_ の内容をline_bufで初期化する
    cur_line_.str(line_buf);
    // cur_line_ は内部でデータ終端か否かのフラグを持っているので、それをクリアする。
    cur_line_.clear();
}

void FileReader::readLabeledDoubleLine(const char *label, double &val) {
    // ファイルを一行読む
    readLine();
    // 行バッファから文字列を読み出し、それが label と一致することを確認する。
    // 一致しなかったら、期待するデータがファイルに入っていないので、例外を挙げる。
    readKeyword(label);
    // 行バッファから double 値を読み出す。double値を取り出せなかったら例外が挙がる。
    readDouble(val, label);
}

void FileReader::readLabeledIntLine(const char *label, int &val) {
    readLine();
    readKeyword(label);
    readInt(val, label);
}

void FileReader::readLabeledStringLine(const char *label, std::string &val) {
    readLine();
    readKeyword(label);
    readString(val, label);
}

void FileReader::readKeyword(const char *keyword) {
    std::string word;
    // 行バッファから単語(ここでの意味は、空白や改行文字以外の文字が続いたもの）を一つ取り出す。
    cur_line_ >> word;
    // 単語が読み出せなかったり（空行で単語が何もなかった場合など）、
    // 読み出せたとしても単語が期待した単語と一致しなかったら例外を挙げる。
    if (cur_line_.fail() || word != keyword) {
        // エラーメッセージを組み立てるために stringstreamオブジェクトをローカルに作成する。
        std::stringstream msg;
        // エラーメッセージを組み立てる。
        msg << "Keyword '" << keyword <<"' was excpected, but '" << word << "' was found at ";
        // エラーメッセージにファイル名と行番号を書き込む
        addFileNameAndLineNoTo(msg);
        // できたエラーメッセージは msg.str() で取り出せるので、それを保持したDataExceptionの
        // インスタンスを作成して throw する。
        throw DataException(__FILE__, __LINE__, msg.str());
    }
}

void FileReader::readDouble(double &val, const char *label) {
    // 行バッファから double の値を取り出す。
    // ここで呼び出しているのは ostream& ostream::operator>>(ostream &os, double &v); というメソッド。
    cur_line_ >> val;
    // 行バッファにdoubleと解釈できるデータが存在しない場合にはエラーフラグが立つのでチェックする。
    if (cur_line_.fail()) {
        // エラーフラグが立っている。エラーメッセージを組み立てて、例外を挙げる。
        std::stringstream msg;
        msg << "floating point value for " << label << " was expected at ";
        addFileNameAndLineNoTo(msg);
        throw DataException(__FILE__, __LINE__, msg.str());
    }
}

void FileReader::readInt(int &val, const char *label) {
    // 行バッファから int の値を取り出す。
    // ここで呼び出しているのは ostream& ostream::operator>>(ostream &os, int &v); というメソッド。
    cur_line_ >> val;
    // 行バッファにintと解釈できるデータが存在しない場合にはエラーフラグが立つのでチェックする。
    if (cur_line_.fail()) {
        // エラーフラグが立っている。エラーメッセージを組み立てて、例外を挙げる。
        std::stringstream msg;
        msg << "integer value for " << label << " was expected at ";
        addFileNameAndLineNoTo(msg);
        throw DataException(__FILE__, __LINE__, msg.str());
    }
}

void FileReader::readString(std::string &val, const char *label) {
    // 行バッファから単語を取り出す。
    // ここで呼び出しているのは ostream& ostream::operator>>(ostream &os, string &v); というメソッド。
    cur_line_ >> val;
    // エラーが起きたか確認する。
    if (cur_line_.fail()) {
        // エラーフラグが立っている。エラーメッセージを組み立てて、例外を挙げる。
        std::stringstream msg;
        msg << "string for " << label << " was expected at ";
        addFileNameAndLineNoTo(msg);
        throw DataException(__FILE__, __LINE__, msg.str());
    }
}

void FileReader::readExpectedInt(int expected_val, const char *label) {
    // 行バッファから int の値を取り出す。
    int val;
    cur_line_ >> val;
    // 読み出しに失敗したり、読み出せたとしても、期待している数値と一致しない場合には例外を挙げる。
    if (cur_line_.fail() || val != expected_val) {
        std::stringstream msg;
        msg << "integer value " << expected_val << " for " << label << " was expected at ";
        addFileNameAndLineNoTo(msg);
        throw DataException(__FILE__, __LINE__, msg.str());
    }
}

void FileReader::addFileNameAndLineNoTo(std::stringstream &ss) {
    // 読み込み中のファイル名と行番号をエラーメッセージを組み立てているストリームに追加する。
    ss << "'" << file_name_ << "', line " << line_no_;
}
