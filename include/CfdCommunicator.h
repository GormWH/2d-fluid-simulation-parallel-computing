/*
 * CfdCommunicator.h
 */

#ifndef _CFDCOMMUNICATOR_H
#define _CFDCOMMUNICATOR_H

#include <Params.h>
#include <State.h>
#include <CfdCommData.h>
#include <mpi.h>

/*
 * 通信処理を実行するクラス。
 *
 * 通信すべきデータは全てCfdCommDataが保持しており、このクラスはMPIの関数を呼び出して
 * 通信を実行することを役割とする。
 */
class CfdCommunicator {

    // 計算条件クラス
    Params *params_;

    // 計算経過クラス
    State *state_;

    // 通信バッファ
    CfdCommData *commData_;

    // std::vectorの要素についてループする時は制御変数は size_t 型がよい。intだと警告が出る。
    size_t i;
    std::vector<MPI_Request> requests;
    std::vector<MPI_Status> statuses;

    int tag;
    int rank,num_procs;
    CfdCommPeerBuffer *peer;
    int other_rank;
    double *send_data_addr;
    int send_data_count;
    double *recv_data_addr;
    int recv_data_count;
public:

    // 初期化関数
    // 渡されたポインタをメンバ変数に格納する。
    void init(Params *params, State *state, CfdCommData *commData);

    // CfdCommDataが保持するCfdCommPeerBufferが保持する送信データを
    // 全peerプロセスと送受信する。質量データを送受信する場合も、
    // 速度データを送受信する場合も、このメソッドを使う。
    void exchangeBoundaryValues();
    // void getRankAndProcs();
    void sendDataMPIPre();
    void sendDataMPI();
};

#endif
