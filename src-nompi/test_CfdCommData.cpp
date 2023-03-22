/*
 * test_CfdCommData.cpp
 */


#include <TestBase.h>
#include <CfdCommData.h>

// CfdCommData向けのテスト用クラス
//
class TestCfdCommData : public TestBase {

    // テスト対象クラスのオブジェクト
    CfdCommData commData_;

    // テスト対象クラスのオブジェクトを作る上で要求されるオブジェクト
    Params params_;
    State state_;
    Node nodes_[4];

public:
    void setup();
    void testBoundaryNode();
    void run();
};

void TestCfdCommData::setup()
{
    // 本テスト専用の計算条件ファイルを読み込む。
    // 本プロセスのrank = 0, 並列プロセス数は 4 という設定でテストする。
    params_.init(0, 4, "testdata/cfdcommdata/case.txt");
    state_.reset();
    commData_.init(&params_, &state_);
    // テスト用の node データにダミーのデータを設定する
    // 全部同じ内容だったり、内容が0だったりすると、値のコピー処理などに
    // バグがあった時に気づきにくいので、規則的で個性のあるデータを入れる。
    for (int i = 0; i < 4; i++) {
        nodes_[i].pos_.set(i*10, i*2);
        nodes_[i].m_ = 1 + i * 0.01;
        nodes_[i].vel_.set(i*0.1, i*0.2);
    }
    // 当プロセスのrankは0だが、rank=1のプロセスと接する境界上に、nodes_[0] が存在する旨を登録する。
    commData_.addBoundaryNode(1, &nodes_[0]);
    // rank=2 のプロセスと接する境界上に、nodes_[1,2,3] が存在する旨を登録する
    commData_.addBoundaryNode(2, &nodes_[1]);
    commData_.addBoundaryNode(2, &nodes_[2]);
    commData_.addBoundaryNode(2, &nodes_[3]);
}

void TestCfdCommData::testBoundaryNode()
{
    // rank=1のプロセスと授受すべきデータを保持するPeerBufferができているはずなので、それを取得する。
    CfdCommPeerBuffer *buff1 = commData_.findOrCreatePeerBufferForRank(1);
    // そこにはnodeが一つだけ登録されていることを確認する。
    int_equals(buff1->nodes_.size(), 1);

    // rank=2のプロセスと授受すべきデータを保持するPeerBufferができているはずなので、それを取得する。
    CfdCommPeerBuffer *buff2 = commData_.findOrCreatePeerBufferForRank(2);
    // そこにはnodeが3つ登録されていることを確認する。
    int_equals(buff2->nodes_.size(), 3);

    // 全境界上の質量データを送信バッファに集めさせる。
    commData_.gatherBoundaryNodeMass();
    int_equals(buff1->send_buffer_.size(), 1);
    dbl_equals(buff1->send_buffer_[0], 1.00);
    int_equals(buff2->send_buffer_.size(), 3);
    dbl_equals(buff2->send_buffer_[0], 1.01);

    // commData_ の他のメソッドも確認すべき。

}

void TestCfdCommData::run()
{
    setup();
    testBoundaryNode();
}

// このテストプログラムのmain関数。
int main(int argc, char *argv[])
{
    // 上記のテストオブジェクトを一つ作成して、そのメソッドを呼ぶことでテストを進める。
    TestCfdCommData test;
    try {
        test.run();
    } catch (IoException &exp) {
        std::cout << exp << std::endl;
    } catch (DataException &exp) {
        std::cout << exp << std::endl;
    }
    return test.report();
}
