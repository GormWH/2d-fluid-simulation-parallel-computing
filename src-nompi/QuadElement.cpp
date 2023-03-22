/*
 * QuadElement.cpp
 */

#include <QuadElement.h>
#include <Vector4.h>
#include <Matrix4.h>
#include <cassert>

void QuadElement::setRank(int rank) {
    // 当要素の領域番号（MPIのrank番号を記録する)
    rank_ = rank;
    // 四隅の節点に対して、その節点が当該rankに含まれているか、あるいは接していることを伝える。
    for (int i = 0; i < 4; i++) {
        nodes_[i]->addRank(rank);
    }
}

//形状関数の設定
double QuadElement::ai_[4] = { 0.25,  0.25, 0.25,  0.25};
double QuadElement::bi_[4] = {-0.25,  0.25, 0.25, -0.25};
double QuadElement::ci_[4] = {-0.25, -0.25, 0.25,  0.25};
double QuadElement::di_[4] = { 0.25, -0.25, 0.25, -0.25};

/*
 * ループ不変量を計算し、メンバ変数に格納する
 *   a_Nx_,a_Ny_,b_Nx_,b_Ny_,r_Nx_,r_Ny_,d_,hx_, hy_,hx_by_a_,hy_by_a_,size_,m_
 */
void QuadElement::calcInvariants1(double Re) {
    // 節点データのVector4クラスでの保存
    Vector4 x, y;
    for(int i = 0; i < 4; i++){
        x.set(i, nodes_[i]->pos_.x_);
        y.set(i, nodes_[i]->pos_.y_);
    }
    double a_xy = (x.get(0)-x.get(2))*(y.get(1)-y.get(3)) + (x.get(1)-x.get(3))*(y.get(2)-y.get(0)); // alpha_xy = (x1-x3)(y2-y4)+(x2-x4)(y3-y1)
    double b_xy = (x.get(2)-x.get(3))*(y.get(0)-y.get(1)) + (x.get(0)-x.get(1))*(y.get(3)-y.get(2)); // beta_xy  = (x3-x4)(y1-y2)+(x1-x2)(y4-y3)
    double r_xy = (x.get(1)-x.get(2))*(y.get(0)-y.get(3)) + (x.get(0)-x.get(3))*(y.get(2)-y.get(1)); // gamma_xy = (x2-x3)(y1-y4)+(x1-x4)(y3-y2)

    // 形状関数N
    //setShapeMatrix();

    // alpha_Niy, alpha_Nix, beta_Niy, beta_Nix, gamma_Niy, gamma_Nix の計算
    setAlphaBetaGamma_Nxy(x,y);

    // 集中化質量 m をnodeに加算
    addMass(a_xy, b_xy, r_xy);

    // 拡散項行列 D
    setDiffusionMatrix(a_xy, b_xy, r_xy, Re);

    // 圧力ベクトル Hx, Hy
    setPressureVector();

    // 要素の大きさsize_の計算
    setSize(x,y);

    // 判別式の計算に必要なHx/A, Hy/Aを計算
    setPressureVectorBySize();
}

/***** ここからはcalcInvariants1() の付属関数 *****/


// a_Nx_, a_Ny_, b_Nx_, b_Ny_, r_Nx_, r_Ny_ の計算
void QuadElement::setAlphaBetaGamma_Nxy(const Vector4 &x, const Vector4 &y){
    Matrix4 E;
    E.unit();

    int j;
    double ax,ay,bx,by,rx,ry;
    for(j = 0; j < 4; j++){
        ax = (E.get(0,j)-E.get(2,j))*(x.get(1)-x.get(3)) + (E.get(1,j)-E.get(3,j))*(x.get(2)-x.get(0));
        ay = (E.get(0,j)-E.get(2,j))*(y.get(1)-y.get(3)) + (E.get(1,j)-E.get(3,j))*(y.get(2)-y.get(0));
        bx = (E.get(2,j)-E.get(3,j))*(x.get(0)-x.get(1)) + (E.get(0,j)-E.get(1,j))*(x.get(3)-x.get(2));
        by = (E.get(2,j)-E.get(3,j))*(y.get(0)-y.get(1)) + (E.get(0,j)-E.get(1,j))*(y.get(3)-y.get(2));
        rx = (E.get(1,j)-E.get(2,j))*(x.get(0)-x.get(3)) + (E.get(0,j)-E.get(3,j))*(x.get(2)-x.get(1));
        ry = (E.get(1,j)-E.get(2,j))*(y.get(0)-y.get(3)) + (E.get(0,j)-E.get(3,j))*(y.get(2)-y.get(1));
        a_Nx_.set(j,ax);
        a_Ny_.set(j,ay);
        b_Nx_.set(j,bx);
        b_Ny_.set(j,by);
        r_Nx_.set(j,rx);
        r_Ny_.set(j,ry);
    }
}

// 集中化質量の加算
void QuadElement::addMass(double a_xy, double b_xy, double r_xy){
    for(int i = 0; i < 4; i++){
        nodes_[i]->m_ += (3*a_xy*ai_[i] + b_xy*bi_[i] + r_xy*ci_[i])/6.0;
    }
}

// 拡散項行列 Dの計算
void QuadElement::setDiffusionMatrix(double a_xy, double b_xy, double r_xy, double Re){
    for(int i = 0; i < 4; i++){
        double val = 0;
        for(int j = 0; j < 4; j++){
            val = (/**//**/
                    3*a_Ny_.get(i)*a_Ny_.get(j) + 3*a_Nx_.get(i)*a_Nx_.get(j)
                    + b_Ny_.get(i)*b_Ny_.get(j) + b_Nx_.get(i)*b_Nx_.get(j)
                    + r_Ny_.get(i)*r_Ny_.get(j) + r_Nx_.get(i)*r_Nx_.get(j)
                    - b_xy/a_xy*(/**/
                                    a_Ny_.get(i)*b_Ny_.get(j) + a_Ny_.get(j)*b_Ny_.get(i)
                                    + a_Nx_.get(i)*b_Nx_.get(j) + a_Nx_.get(j)*b_Nx_.get(i)
                                /**/)
                    - r_xy/a_xy*(/**/
                                    a_Ny_.get(i)*r_Ny_.get(j) + a_Ny_.get(j)*r_Ny_.get(i)
                                    + a_Nx_.get(i)*r_Nx_.get(j) + a_Nx_.get(j)*r_Nx_.get(i)
                                /**/)
            /**//**/)/(Re*6*a_xy);
            d_.set(i, j, val);
        }
    }
}

void QuadElement::setPressureVector(){
    for(int i = 0; i < 4; i++){
        hx_.set(i, 0.5*a_Ny_.get(i));
        hy_.set(i, -0.5*a_Nx_.get(i));
    }
}

void QuadElement::setSize(const Vector4 &x, const Vector4 &y){
    double x1,y1,x2,y2;
    double val = 0.0;
    for(int i = 0; i < 4; i++){
        // 二つの接点(x1,y1),(x2,y2)の値を得る．
        x1 = x.get(i);
        y1 = y.get(i);
        if( i == 3){
            x2 = x.get(0);
            y2 = y.get(0);
        }else{
            x2 = x.get(i+1);
            y2 = y.get(i+1);
        }
        val += (x1-x2)*(y1+y2)/2.0;
    }
    assert(val>0);
    size_ = val;
}

void QuadElement::setPressureVectorBySize(){
    for(int i = 0; i < 4; i ++){
        hx_by_a_.set(i, hx_.get(i)/size_);
        hy_by_a_.set(i, hy_.get(i)/size_);
    }
}
/***** ここまでが calcInvariants1() の付属関数 *****/

void QuadElement::calcInvariants2(double delta_t, double relaxation) {
    /*
     * inv_mを必要とするループ不変量の計算
     * dt_hx_by_m_, dt_hy_by_m_, lambda_
     */
    // dt_hx_by_m_, dt_hy_by_m_ の計算
    setDtHxyByM(delta_t);

    //lambda_ の計算
    setLambda(delta_t,relaxation);
}

void QuadElement::setDtHxyByM(double delta_t){
    //delta_t * Hx * M(-1)
    double val;
    for(int i = 0; i < 4; i++){
        val = delta_t * (nodes_[i]->inv_m_) * hx_.get(i);
        dt_hx_by_m_.set(i,val);
        val = delta_t * (nodes_[i]->inv_m_) * hy_.get(i);
        dt_hy_by_m_.set(i,val);
    }
}

void QuadElement::setLambda(double delta_t, double relaxation){
    double hx_m_hx, hy_m_hy;
    Matrix4 inv_m;
    inv_m.unit();
    for(int i = 0; i < 4; i++){
        inv_m.set(i,i,nodes_[i]->inv_m_);
    }
    hx_m_hx = hx_.dot(inv_m*hx_);
    hy_m_hy = hy_.dot(inv_m*hy_);
    lambda_relaxation_ = size_*relaxation / (delta_t * (hx_m_hx + hy_m_hy));
}

void QuadElement::calcVelocityPrediction(){
    // 移流項行列Aの計算
    calcConvectionMatrix();

    // 速度のVector4クラスでの保存
    Vector4 u, v;
    for(size_t i = 0; i < 4; i++){
        u.set(i, nodes_[i]->vel_.x_);
        v.set(i, nodes_[i]->vel_.y_);
    }

    // 速度の変化量を加算
    addVelocityDelta(u, v);
}

// 移流項行列の計算
void QuadElement::calcConvectionMatrix(){
    int i, j;
    double val;
    Matrix4 A_x, A_y;
    double Au = 0;
    double Av = 0;
    double Bu = 0;
    double Bv = 0;
    double Cu = 0;
    double Cv = 0;
    double Du = 0;
    double Dv = 0;
    for(i = 0; i < 4; i++){
        Au += ai_[i] * nodes_[i]->vel_.x_;
        Av += ai_[i] * nodes_[i]->vel_.y_;
        Bu += bi_[i] * nodes_[i]->vel_.x_;
        Bv += bi_[i] * nodes_[i]->vel_.y_;
        Cu += ci_[i] * nodes_[i]->vel_.x_;
        Cv += ci_[i] * nodes_[i]->vel_.y_;
        Du += di_[i] * nodes_[i]->vel_.x_;
        Dv += di_[i] * nodes_[i]->vel_.y_;
    }
    for(i = 0; i < 4; i++){
        for(j = 0; j < 4; j++){
            val = (1.0/18.0)*(
                    9*ai_[i]*Au*a_Ny_.get(j)
                    + 3*( (bi_[i]*Bu + ci_[i]*Cu) * a_Ny_.get(j)
                            + (ai_[i]*Bu + bi_[i]*Au) * b_Ny_.get(j)
                            + (ai_[i]*Cu + ci_[i]*Au) * r_Ny_.get(j)
                        )
                    + di_[i]*Du*a_Ny_.get(j)
                    + (di_[i]*Cu + ci_[i]*Du) * b_Ny_.get(j)
                    + (di_[i]*Bu + bi_[i]*Du) * r_Ny_.get(j)
            );
            A_x.set(i,j,val);

            val = -(1.0/18.0)*(
                    9*ai_[i]*Av*a_Nx_.get(j)
                    + 3*( (bi_[i]*Bv + ci_[i]*Cv) * a_Nx_.get(j)
                            + (ai_[i]*Bv + bi_[i]*Av) * b_Nx_.get(j)
                            + (ai_[i]*Cv + ci_[i]*Av) * r_Nx_.get(j)
                        )
                    + di_[i]*Dv*a_Nx_.get(j)
                    + (di_[i]*Cv + ci_[i]*Dv) * b_Nx_.get(j)
                    + (di_[i]*Bv + bi_[i]*Dv) * r_Nx_.get(j)
            );
            A_y.set(i,j,val);
        }
    }
    A_ = A_x + A_y;
}

// 速度変化量の計算(速度予測値)
void QuadElement::addVelocityDelta(const Vector4 &u, const Vector4 &v){
    Vector4 d_u, d_v;
    d_u = A_*u - hx_*p_ + d_*u; // - Fx 外力
    d_v = A_*v - hy_*p_ + d_*v; // - Fy
    for(size_t i = 0; i < 4; i++){
        nodes_[i]->d_vel_.x_ += -(nodes_[i]->delta_t_by_m_) * d_u.get(i);
        nodes_[i]->d_vel_.y_ += -(nodes_[i]->delta_t_by_m_) * d_v.get(i);
    }
}

Vector4 u_calcDiscriminant, v_calcDiscriminant;
// 収束判別式の計算
void QuadElement::calcDiscriminant(){
    size_t i;
    // 節点データの保存
    for(i = 0; i < 4; i++){
        u_calcDiscriminant.set(i, nodes_[i]->vel_.x_);
        v_calcDiscriminant.set(i, nodes_[i]->vel_.y_);
    }
    D_ = dot(hx_by_a_, u_calcDiscriminant) + dot(hy_by_a_, v_calcDiscriminant);
}


// 速度変化量の計算(速度補正値)
void QuadElement::correctVelocity(){
    size_t i;
    // 圧力変化量の計算
    div_ = -lambda_relaxation_*D_;// * (0~1)*************
    // 圧力の補正
    p_ += div_;
    // 速度変化量へ加算
    for(i = 0; i < 4; i++){
        nodes_[i]->d_vel_.x_ += dt_hx_by_m_.get(i)*div_;
        nodes_[i]->d_vel_.y_ += dt_hy_by_m_.get(i)*div_;
    }
}
