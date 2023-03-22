/*
 * Logger.cpp
 */
#include <Logger.h>
#include <sstream>

// クラススタティック変数の定義。
// クラススタティック変数をメモリ上に配置するためには、いずれかのソースに定義を書く必要がある。
// そのソースをコンパイルして得られるオブジェクトファイルが、その変数の所属ファイルとなる。
std::fstream Logger::out;

void Logger::openLog(const char *progname, int rank) {
    // ファイル名を組み立てるための stringstream をローカルに作成する
    std::stringstream ss;
    // ログファイルのファイル名を組み立てる
    ss << progname << ".log." << rank << ".txt";
    // ログファイルを開く。outは、クラススタティック変数。
    out.open(ss.str().c_str(), std::ios::out);
    // ここではログファイルを開けなかった場合のエラー処理をしていない。
    // 存在しないディレクトリや、書き込み権限のないディレクトリをパス名のディレクトリとして
    // 指定した場合には、ログも残らないことになってしまう。
    // それを防ごうとする場合には、この時点で標準エラー出力に、ログファイルが開けなかった
    // 旨のメッセージを出力して、例外を挙げてプログラムを止めてしまうことも考えられる。
}

void Logger::closeLog() {
    // ログファイルをクローズする。
    out.close();
}
