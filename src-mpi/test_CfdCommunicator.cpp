/*
 * test_CfdCommunicator.cpp
 * run command: mpirun -np 4 test_CfdCommunicator
 */

#include <MpiTestBase.h>
#include <CfdCommunicator.h>

// CfdCommunicatorの単体テストドライバークラス
// このファイルの最後にmain関数がある。
// MPIの制御下で、複数プロセスでこのプログラムを実行する。
// 各rankにおいてそれぞれ同じテストプログラムを実行する。
// rank間でテストデータの送受信が行われる。

class TestCfdCommunicator : public MpiTestBase {

    // テスト対象：コミュニケータ
    CfdCommunicator comm_;

    // CfdCommunicatorを動かすために必要なオブジェクト

    // 計算条件
    Params params_;

    // 計算経過
    State state_;

    // 通信バッファ
    CfdCommData data_;

public:

    // テストの準備
    void setup();

    // 項目別テスト（１）exchangeBoundaryValuesのテスト
    void testExchange();

    // 項目別テスト（2）収束を全体で確認するテスト
    void testDivergence(int);

    // 全テストを実行する
    void run();
};

void TestCfdCommunicator::setup()
{
    params_.num_procs_ = num_procs_;
    params_.my_rank_ = my_rank_;

    comm_.init(&params_, &state_, &data_);

}

void TestCfdCommunicator::testExchange()
{
    /* prepare buffers and data for all other ranks except myself */
    int i;
    for (i = 0; i < num_procs_; i++) {
        if (i == my_rank_) {
            /* skip rank for myself */
            continue;
        }
        CfdCommPeerBuffer *peer = data_.findOrCreatePeerBufferForRank(i);
        peer->send_buffer_.clear();
        /*
         * dummy send data
         * 10000 * sender_rank + 100 * receiver_rank + array index
         */
        peer->send_buffer_.push_back(10000*my_rank_ + 100*i + 0);
        peer->send_buffer_.push_back(10000*my_rank_ + 100*i + 1);
        peer->send_buffer_.push_back(10000*my_rank_ + 100*i + 2);
        /*
         * set the receive buffer to the same size
         */
        peer->recv_buffer_.resize(3);
    }

    /* MPI communication. send out send_buffers and receive to recv_buffers */
    comm_.exchangeBoundaryValues();

    /* check that we received correct data. */
    for (i = 0; i < num_procs_; i++) {
        if (i == my_rank_) {
            /* skip rank for myself */
            continue;
        }
        CfdCommPeerBuffer *peer = data_.findOrCreatePeerBufferForRank(i);
        /*
         * check received dummy data
         * 10000 * sender_rank + 100 * receiver_rank + array index
         */
        dbl_equals(peer->recv_buffer_[0], 10000*i + 100*my_rank_ + 0);
        dbl_equals(peer->recv_buffer_[1], 10000*i + 100*my_rank_ + 1);
        dbl_equals(peer->recv_buffer_[2], 10000*i + 100*my_rank_ + 2);
    }
}

void TestCfdCommunicator::testDivergence(int my_rank_)
{
    bool isNotDivergence;
    bool isNotDivergenceAll=1;
    size_t i=0;
    while(isNotDivergenceAll){//収束してない間
        isNotDivergence=(my_rank_>i);
        i++;
        // Logger::out << my_rank_ << i << isNotDivergence << std::endl;
        MPI_Allreduce(&isNotDivergence, &isNotDivergenceAll, 1, MPI_LOGICAL, MPI_LOR, MPI_COMM_WORLD);
        // Logger::out << my_rank_ << i << isNotDivergenceAll << std::endl << std::endl;
    }
}

void TestCfdCommunicator::run()
{
    setup();
    testExchange();
    // testDivergence(my_rank_);
    // Logger::out << my_rank_ << std::endl;
}

/*
 * Run this test under MPI, with process numbers 3 or above.
 */
int main(int argc, char *argv[])
{
    TestCfdCommunicator test;
    test.initMpi(argc, argv);
    test.run();
    return test.report();
}
