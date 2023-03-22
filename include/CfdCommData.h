/*
 * CfdCommData.h
 */

#ifndef CFDCOMMDATA_H_
#define CFDCOMMDATA_H_

#include <Params.h>
#include <State.h>
#include <Node.h>
#include <vector>

/*
 * 通信相手１プロセスごとのバッファクラス
 * CFDでは通信相手プロセスの個数がデータ次第で変わるので、必要な数だけ作成され、
 * CfdCommDataが保持する。
 */
class CfdCommPeerBuffer {
public:

    // デストラクタ、デフォルトコンストラクタ、コピーコンストラクタは
    // コンパイラが生成するデフォルトのものを使用するので、定義しない。

    // 相手プロセスのrank番号
    int rank_;

    // 相手プロセスとの境界上にあるNodeの一覧
    std::vector<Node *> nodes_;

    // 送信バッファ。質量を送信する場合には質量値を保持する
    // 速度を送信する場合には x成分, y成分 を交互に保持する
    // データの並び順はnodes_の並び順と同じにする
    std::vector<double> send_buffer_;

    // 受信バッファ。送信バッファと同じ使い方をする
    std::vector<double> recv_buffer_;

    // この通信相手との境界上にあると判明したノードを追加する
    void addNode(Node *node) {
        nodes_.push_back(node);
    }

    // 境界上のノードの質量値をsend_buffer_に集める
    void gatherBoundaryNodeMass();

    // recv_bufer_に格納された質量値を境界上のノードに分配（加算）する
    void distributeBoundaryNodeMass();

    // 境界上のノードの質量値をsend_buffer_に集める
    void gatherBoundaryNodeVelocityDelta();

    // recv_bufer_に格納された質量値を境界上のノードに分配（加算）する
    void distributeBoundaryNodeVelocityDelta();
};

/*
 * 通信バッファ全体を統括するクラス
 * 隣接するノードとの通信のためのCfdCommPeerBufferを保持することに加えて、
 * 全プロセスとの通信の際に使う送受信バッファも保持する。
 */
class CfdCommData {
public:

    // 計算条件クラス
    Params *params_;

    // 計算経過クラス
    State *state_;

    // 通信相手ごとのバッファオブジェクトの一覧
    // CfdCommPeerBufferのコピーコンストラクタが呼ばれる
    std::vector<CfdCommPeerBuffer> peer_buffers_;

    // バッファオブジェクトを初期化する。引数に渡されたポインタを記憶するだけ。
    void init(Params *params, State *state);

    // 隣接プロセスとの境界にあると判明したノードを登録する。
    // この関数の中ではrankに対応するpeer bufferを必要に応じて作成し
    // そのpeer buffer にnodeを登録する。
    void addBoundaryNode(int rank, Node *node);

    // rankに対応するpeer buffer を検索する。
    // 初出の場合は、新規に作成したうえで返す。(find, or otherwise create)
    CfdCommPeerBuffer *findOrCreatePeerBufferForRank(int rank);

    // 全peerについて、境界ノードの質量をpeer buffer に集め、送信に備える。
    void gatherBoundaryNodeMass();

    // 全peerについて、peer bufferに受信した質量を境界上のノードに加算する。
    void distributeBoundaryNodeMass();

    // 全peerについて、境界ノードの速度補正値をpeer buffer に集め、送信に備える。
    void gatherBoundaryNodeVelocityDelta();

    // 全peerについて、peer bufferに受信した速度補正値を境界上のノードに加算する。
    void distributeBoundaryNodeVelocityDelta();
};


#endif /* CFDCOMMDATA_H_ */
