/*
 * QuadElement.h
 */

#ifndef QUADELEMENT_H_
#define QUADELEMENT_H_

#include <Node.h>
#include <Matrix4.h>
#include <Vector4.h>

/*
 * 四角形要素の情報を保持するクラス
 */
class QuadElement {
public:

    /*
     * [part 1] 入力ファイルから直接求まる情報
     */

    // 四隅の節点へのポインタ。
    // この配列は一旦は入力ファイルの登場順そのままで格納するが、
    // 計算においては反時計回りに並んでいる必要がある。
    // ループ不変量を計算すると反時計回りになっているかどうか分かるので
    // その時点でもしも時計回りになっていることが検知された場合には
    // この配列の登録順を反転させるものとする。
    Node *nodes_[4];

    // 領域分割の番号(0はじまり)
    int rank_;

    // ファイルに記載の要素番号
    int global_index_;

    /*
     * [part 2] ループ不変量
     */
    // 形状関数
    static double ai_[4];
    static double bi_[4];
    static double ci_[4];
    static double di_[4];

    // calcInvariants1()
    Vector4 a_Ny_, a_Nx_;
    Vector4 b_Ny_, b_Nx_;
    Vector4 r_Ny_, r_Nx_;
    Vector4 hx_, hy_;
    Vector4 hx_by_a_, hy_by_a_;
    Matrix4 d_;
    double size_;
    // calcInvariants2()
    Vector4 dt_hx_by_m_, dt_hy_by_m_;
    double lambda_relaxation_;

    /*
     * [part 3] 状態変数や、中間的な計算値
     */
    // 移流項A
    Matrix4 A_;
    // 判別式D
    double D_;
    // 圧力
    double p_;
    // 圧力補正値
    double div_;

    // コンストラクタ、デストラクタはコンパイラが生成するデフォルトのものをそのまま使う

    /*
     * 要素の領域番号を指定する。
     */
    void setRank(int rank);

    /*
     * ループ不変量の計算
     */
    // 集中化質量の確定前
    void calcInvariants1(double Re);
        // calcInvariants1()の付属関数
        void setAlphaBetaGamma_Nxy(const Vector4 &x, const Vector4 &y);
        void addMass(double a_xy, double b_xy, double r_xy);
        void setDiffusionMatrix(double a_xy, double b_xy, double r_xy, double Re);
        void setPressureVector();
        void setSize(const Vector4 &x, const Vector4 &y);
        void setPressureVectorBySize();
    // 集中化質量の確定後
    void calcInvariants2(double delta_t, double relaxation);
        // calcInvariants2()の付属関数
        void setDtHxyByM(double delta_t);
        void setLambda(double delta_t, double relaxation);

    /*
     * 速度の予測値の計算
     */
    void calcVelocityPrediction();
        // calcVelocityPrediction() の付属関数
        void calcConvectionMatrix();
        void addVelocityDelta(const Vector4 &u, const Vector4 &v);

    /*
     * 判別式を計算
     */
    void calcDiscriminant();
    /*
     * 速度補正値の計算
     */
    void correctVelocity();

};

//void setShapeMatrix(QuadElement &element);

// Vector4 QuadElement::ai_( 0.25, 0.25,0.25, 0.25);
// Vector4 QuadElement::bi_(-0.25, 0.25,0.25,-0.25);
// Vector4 QuadElement::ci_(-0.25,-0.25,0.25, 0.25);
// Vector4 QuadElement::di_( 0.25,-0.25,0.25,-0.25);

#endif /* QUADELEMENT_H_ */
