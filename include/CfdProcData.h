/*
 * ProcData.h
 */

#ifndef CFDPROCDATA_H_
#define CFDPROCDATA_H_

#include <Params.h>
#include <State.h>

#include <Node.h>
#include <QuadElement.h>
#include <Boundary.h>
#include <CfdCommData.h>

#include <vector>

/*
 * ABMAC法の計算データを1プロセス分保持するクラス
 *
 * 実際には全プロセス分のデータを保持した上で、自身のプロセスが担当する
 * 四角形要素のみを計算対象としている。
 */
class CfdProcData {

    // テストクラスから当クラスのprivateメンバーにアクセスできるようにするためのfriend宣言
    friend class TestCfdProcData;

    // 全プロセス分のノード一覧
    std::vector<Node> nodes_;

    // 当プロセスに属する四角形要素に属する節点の一覧。
    // 各ポインタの指す先は nodes_ 上の要素
    std::vector<Node *> my_nodes_;

    // 全プロセス分の四角形要素一覧
    std::vector<QuadElement> elements_;

    // 当プロセスに属する四角形要素の一覧。
    // 各ポインタの指す先は elements_ 上の要素
    std::vector<QuadElement *> my_elements_;

    // 境界条件の一覧
    std::vector<Boundary> boundaries_;

    // 計算条件クラス
    Params *params_;

    // 計算経過クラス
    State *state_;

    // 通信バッファ
    CfdCommData *commData_;

    // リスタートファイル用の名前に自分のランクを追加したもの
    std::string my_rank_temporal_file_name_;

public:
    // 初期化。
    // 引数のポインタをメンバ変数に格納する。
    void init(Params *params, State *state, CfdCommData *commData_);

    // メッシュファイルの読み込みを行う。
    // ファイルのパス名は計算条件オブジェクトから取得する。
    // 例外:
    //   IoException: ファイルが開けない場合
    //   DataException: ファイルの内容に問題があった場合
    void readMeshFile();

    // 境界条件ファイルの読み込みを行う。
    // ファイルのパス名は計算条件オブジェクトから取得する。
    // 例外:
    //   IoException: ファイルが開けない場合
    //   DataException: ファイルの内容に問題があった場合
    void readBoundaryFile();

    // 読み込んだメッシュファイルのデータと、自身のrank番号を元にして
    // 当プロセスで計算を担当すべき四角形要素と節点を特定し、
    // my_elements_, my_nodes_ に格納する。
    void findOwnData();

    // リスタートファイルの読み込み
    void readTemporalData();

    // リスタートファイルがあるか調べるメソッド
    bool checkRestartFileIsThere();

    // 全節点、全要素の速度、圧力をゼロに初期化する。
    void clearFieldData();

    // リスタートファイルから変数を読み込む
    void initFieldDataByRestartFile();

    // リスタートファイルの書き出し
    void writeTemporalData();

    // ループ不変量計算その１。隣接プロセスとの通信の手前までの計算と、
    // 隣接プロセスに渡すべき質量データを通信バッファに格納する所までを行う。
    void calcInvariants1();

    // ループ不変量計算その２。
    // 隣接プロセスから受信した質量データをバッファから受け取って（実際には
    // バッファオブジェクトにあるdistributeメソッドによって質量データを分配させて）
    // 残りのループ不変量計算を行う。
    void calcInvariants2();

    // 速度予測値を計算する。
    void calcVelocityPrediction();

    // 閾値を超える要素があれば補正してtrueを返す
    bool calcDivergenceAndCorrect();

    // 速度の変化量及び補正量を初期化する
    void clearVelocityDelta();

    // 境界の節点に対して隣接プロセスからのd_velを集める。
    void gatherVelocityDelta();

    // gatherVelocityDelta()で集めた値を加算する。
    void distributeVelocityDelta();

    // 速度の変化量を速度へ反映させた後，初期化する
    void applyVelocityDeltaAndClear();

    // 境界条件の影響を反映させる
    void applyBoundaryConditions();

    // 結果をファイルに書き込む
    void writeFieldData();
};

#endif
