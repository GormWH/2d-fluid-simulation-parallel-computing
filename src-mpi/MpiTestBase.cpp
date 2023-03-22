/*
 * MpiTestBase.cpp
 */

#include <MpiTestBase.h>

MpiTestBase::~MpiTestBase()
{
    // MPIの終了処理
    MPI_Finalize();
}

MpiTestBase::MpiTestBase()
{

}

void MpiTestBase::initMpi(int argc, char *argv[])
{
    // MPIの初期化
    MPI_Init(&argc, &argv);

    // 自身のランクを取得する
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank_);

    // 総プロセス数を取得する
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs_);

    // ログファイルの名前を、自身のプログラム名に基づいて作成する

    // 自身のプログラム名（起動時のパス名）を取得する
    std::string path = argv[0];
    // パス名だった場合に、最後の '/' より後の部分を取得する。
    size_t pos = path.find_last_of('/');
    if (pos != std::string::npos) {
        path = path.substr(pos+1);
    }

    // ロガーのログファイルを開く
    Logger::openLog(path.c_str(), my_rank_);

    // テストの出力を、ログファイルに向ける
    setOut(&Logger::out);
}
