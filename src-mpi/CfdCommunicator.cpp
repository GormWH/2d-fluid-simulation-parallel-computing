/*
 * CfdCommunicator.cpp
 */
#include <CfdCommunicator.h>
#include <mpi.h>
#include <cassert>
#include <Logger.h>

void CfdCommunicator::init(Params *params, State *state, CfdCommData *commData) {
    params_ = params;
    state_ = state;
    commData_ = commData;

}

/*
 * 隣接プロセス数の最大値
 * 単純な四角い格子状のプロセス配置でも、周辺プロセス数は8個に及ぶ。
 * 格子の形状の都合により、鋭角状の領域分割がなされるとさらに増える。
 * ここでは最大値を100と決めて実装し、もし100を越えるデータが与えられたら
 * それを検知してエラー終了するようにする。
 */
#define MAX_NEIGHBORS 100

/*
 * 全ての隣接プロセスと、共有している節点のデータを授受する。
 */
void CfdCommunicator::exchangeBoundaryValues() {
    /*
     * 隣接プロセス数が想定内に収まっているのか確認する。
     */
    assert(commData_->peer_buffers_.size() <= MAX_NEIGHBORS);

    requests.resize(2*commData_->peer_buffers_.size());
    statuses.resize(2*commData_->peer_buffers_.size());

    // MPI_Barrier(MPI_COMM_WORLD);
    // 全隣接プロセスについてのループ
    for (i = 0; i < commData_->peer_buffers_.size(); i++) {
        // 通信準備
        sendDataMPIPre();

        // 通信処理の本体
        sendDataMPI();
    }

    // 同期
    MPI_Waitall(requests.size(),&requests[0], &statuses[0]);
}

// 通信処理の準備
void CfdCommunicator::sendDataMPIPre(){
    // 隣接プロセスとの通信バッファを取得する。
    peer = &commData_->peer_buffers_[i];
    //
    // 通信相手先のランク番号
    other_rank = peer->rank_;

    // 送信データアドレスは、データを保持しているvectorの、先頭要素のアドレスを取得する。
    // vectorの先頭アドレスと、vectorが割り当ててくれた、ヒープ上のdoubleの配列領域の
    // アドレスは別物なので、vectorのアドレスを使わないこと（使ったグループがいた）。
    send_data_addr = &peer->send_buffer_[0];
    // printf("%lf\n",send_data_addr[0]);
    send_data_count = (int) peer->send_buffer_.size();

    // 受信データアドレスも同様
    recv_data_addr = &peer->recv_buffer_[0];
    recv_data_count = (int) peer->recv_buffer_.size();

    // 送受信するデータ数は同一でないとおかしい。
    assert(send_data_count == recv_data_count);
}

// 通信処理の本体
void CfdCommunicator::sendDataMPI(){
    // 非同期で送信
    // Logger::out << "other_rank=" << other_rank << std::endl;
    MPI_Isend(send_data_addr, send_data_count, MPI_DOUBLE, other_rank, 0, MPI_COMM_WORLD, &requests[2*i]);

    // 非同期で受信待機
    MPI_Irecv(recv_data_addr, recv_data_count, MPI_DOUBLE, other_rank, 0, MPI_COMM_WORLD, &requests[2*i+1]);

}
