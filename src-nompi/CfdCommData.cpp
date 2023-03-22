/*
 * CfdCommData.cpp
 */
#include <CfdCommData.h>
#include <Logger.h>

/*
 * CfdCommData のメソッドの実装
 */
void CfdCommData::init(Params *params, State *state) {
    // 後に呼ばれるメソッドの中で使えるようにparamsとstateを保存しておく。
    params_ = params;
    state_ = state;
}

void CfdCommData::addBoundaryNode(int rank, Node *node) {
    // rank番号に対応するpeer bufferを取得する。
    // 初めて登場したrank番号の場合は、このタイミングでpeer bufferを作成する。
    CfdCommPeerBuffer *peer_buffer = findOrCreatePeerBufferForRank(rank);

    // そのpeer bufferにnodeを登録する。
    peer_buffer->addNode(node);
}

CfdCommPeerBuffer *CfdCommData::findOrCreatePeerBufferForRank(int rank) {
    CfdCommPeerBuffer *peer_buffer;
    size_t i;
    // 作成済みのpeer bufferに、求められているrankのものがないか探索する。
    // これは線形探索なので、探索アルゴリズムとしては性能が悪いが、初期化処理の中でしか
    // 呼ばれないので、この検索時間が顕著に問題とならない限りは改良せずにおく。
    for (i = 0; i < peer_buffers_.size(); i++) {
        CfdCommPeerBuffer *peer_buffer = &peer_buffers_[i];
        if (peer_buffer->rank_ == rank) {
            // 求められているrankのbufferが見つかった。
            return peer_buffer;
        }
    }
    // 見つからなかったので、この場で作成する。
    CfdCommPeerBuffer new_buffer;
    new_buffer.rank_ = rank;
    peer_buffers_.push_back(new_buffer);
    peer_buffer = & peer_buffers_.back();
    return peer_buffer;
}

void CfdCommData::gatherBoundaryNodeMass() {
    // 全peer bufferに、境界上の節点の質量値を送信バッファに取り込むことを命じる
    size_t i;
    for (i = 0; i < peer_buffers_.size(); i++) {
        peer_buffers_[i].gatherBoundaryNodeMass();
    }
}

void CfdCommData::distributeBoundaryNodeMass() {
    // 全peer bufferに、受信バッファ上の質量値を節点の質量に加えることを命じる
    size_t i;
    for (i = 0; i < peer_buffers_.size(); i++) {
        peer_buffers_[i].distributeBoundaryNodeMass();
    }
}

void CfdCommData::gatherBoundaryNodeVelocityDelta() {
    // 全peer bufferに、境界上の節点の質量値を送信バッファに取り込むことを命じる
    size_t i;
    for (i = 0; i < peer_buffers_.size(); i++) {
        peer_buffers_[i].gatherBoundaryNodeVelocityDelta();
    }
}
void CfdCommData::distributeBoundaryNodeVelocityDelta() {
    // 全peer bufferに、受信バッファ上の質量値を節点の質量に加えることを命じる
    size_t i;
    for (i = 0; i < peer_buffers_.size(); i++) {
        peer_buffers_[i].distributeBoundaryNodeVelocityDelta();
    }
}






/*
 * ここから先は CfdCommPeerBufferのメソッド定義
 */

void CfdCommPeerBuffer::gatherBoundaryNodeMass() {

    // 送信バッファの長さを節点の数に合わせる。
    send_buffer_.resize(nodes_.size());
    // 同数のデータを相手から受信するはずなので、受信バッファの長さも同じ数に合わせる。
    recv_buffer_.resize(nodes_.size());

    // 節点の質量値を取得して、送信バッファの該当個所に格納する。
    size_t j;
    for (j = 0; j < nodes_.size(); j++) {
        send_buffer_[j] = nodes_[j]->m_;
    }
}

void CfdCommPeerBuffer::distributeBoundaryNodeMass() {
    // 受信バッファの質量値を、対応する節点の質量値に加える。
    size_t j;
    for (j = 0; j < nodes_.size(); j++) {
        nodes_[j]->m_ += recv_buffer_[j];
    }
}

void CfdCommPeerBuffer::gatherBoundaryNodeVelocityDelta() {
    // 送信バッファの長さを節点の数に合わせる。
    send_buffer_.resize(nodes_.size()*2);
    // 同数のデータを相手から受信するはずなので、受信バッファの長さも同じ数に合わせる。
    recv_buffer_.resize(nodes_.size()*2);

    // 節点の速度補正値を取得して、送信バッファの該当個所に格納する。
    size_t j;
    // Logger::out << "start set buffer" << std::endl;

    for (j = 0; j < nodes_.size(); j++) {
        send_buffer_[2*j] = nodes_[j]->d_vel_.x_;
        send_buffer_[2*j+1] = nodes_[j]->d_vel_.y_;
    }
}

void CfdCommPeerBuffer::distributeBoundaryNodeVelocityDelta() {
    // 受信バッファの速度補正値を、対応する節点の速度補正値に加える。
    size_t j;
    for (j = 0; j < nodes_.size(); j++) {
        nodes_[j]->d_vel_.x_ += recv_buffer_[2*j];
        nodes_[j]->d_vel_.y_ += recv_buffer_[2*j+1];
    }
}
