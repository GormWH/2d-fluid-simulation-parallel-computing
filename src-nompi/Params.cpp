/*
 * Params.cpp
 */
#include <Params.h>
#include <cassert>
#include <FileReader.h>
#include <Logger.h>
#include <sstream>
#include <cerrno>
#include <cstring>

void Params::init(int np, int rank, const char *filename) {
    assert(filename != NULL);
    // 引数をメンバ変数に記録する。
    num_procs_ = np;
    my_rank_ = rank;

    /*
     * 計算条件ファイルを読み込む。
     */
    FileReader rdr; // FileReaderクラスを用いてファイルを読む
    // このクラスはエラーがあった場合に IoExceptionやDataExceptionを挙げるので、
    // 本 init メソッドはそれらの例外を挙げる可能性がある。本メソッドを呼ぶ
    // 処理ではそれに備えている必要がある。

    // ファイルをオープンする
    rdr.open(filename);

    // ファイルの各行を入力する
    // ここで期待している順番にもれなく行が並んでいないと、例外が挙がる。
    rdr.readLabeledDoubleLine("Re", re_);
    rdr.readLabeledDoubleLine("delta_t", delta_t_);
    rdr.readLabeledDoubleLine("T", duration_);
    rdr.readLabeledDoubleLine("T_ramp", t_ramp_);
    rdr.readLabeledIntLine("N_interval", n_interval_);
    rdr.readLabeledDoubleLine("epsilon", epsilon_);
    rdr.readLabeledIntLine("max_corrections", max_corrections_);
    rdr.readLabeledDoubleLine("relaxation", relaxation_);
    rdr.readLabeledStringLine("mesh", mesh_file_name_);
    rdr.readLabeledStringLine("boundary", boundary_file_name_);
    rdr.readLabeledStringLine("outfile", output_file_name_);
    rdr.readLabeledStringLine("tmpfile", temporal_file_name_);

    // ファイルをクローズする
    rdr.close();
}
