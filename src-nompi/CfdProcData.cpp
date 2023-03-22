/*
 * CfdProcData.cpp
 */

#include <CfdProcData.h>

#include <FileReader.h>
#include <VtkWriter.h>
#include <Logger.h>

#include <cmath>
#include <cassert>

#include <stdio.h>
#include <string>

void CfdProcData::init(Params *params, State *state, CfdCommData *commData) {
    // 必要な時にすぐに参照できるようにメンバ変数に保持する
    params_ = params;
    state_ = state;
    commData_ = commData;
}

void CfdProcData::readMeshFile() {

    FileReader rdr;

    int n_procs;
    int n_nodes;
    int n_elems;

    // 形状定義ファイルを読む
    // ファイル名は計算条件オブジェクトが保持している。

    Logger::out << "Reading mesh file " << params_->mesh_file_name_ << std::endl;

    // ファイルを開く (IoException)
    rdr.open(params_->mesh_file_name_);

    // 内容:プロセス数 ノード数 要素数
    rdr.readLine();
    rdr.readInt(n_procs, "Number of processes");
    rdr.readInt(n_nodes, "Number of nodes");
    rdr.readInt(n_elems, "Number of elements");

    // ベクタークラスの長さを伸ばす（メモリを割り当てさせる）
    nodes_.resize(n_nodes);
    elements_.resize(n_elems);

    // 節点データを読む
    int j;
    for (j = 1; j <= n_nodes; j++) {
        Node &node = nodes_[j-1];
        // node.global_index_ = j;
        // 内容: 節点番号(1～) X Y
        rdr.readLine();
        // 節点番号が期待と違ったら不整合とみなす（DataExceptionが上がる)
        rdr.readExpectedInt(j, "node index");
        rdr.readDouble(node.pos_.x_, "X");
        rdr.readDouble(node.pos_.y_, "Y");
    }

    // 四角形要素データを読む
    int i;
    for (i = 1; i <= n_elems; i++) {
        QuadElement &element = elements_[i-1];
        element.global_index_ = i;// 使われてないのでほんとは不要だがあってもたいして変わらないので置いとく
        int n1, n2, n3, n4;
        int rank;
        // 内容: 要素番号(1～) n1 n2 n3 n4 rank(0～)
        rdr.readLine();
        rdr.readExpectedInt(i, "element index");
        rdr.readInt(n1, "node1");
        rdr.readInt(n2, "node2");
        rdr.readInt(n3, "node3");
        rdr.readInt(n4, "node4");
        rdr.readInt(rank, "rank");

        // node番号の値がおかしかったり、rankの値がおかしかったら
        // DataExceptionを上げるべき。

        // 指定されたnodeのアドレスを記録する。
        element.nodes_[0] = &nodes_[n1-1];
        element.nodes_[1] = &nodes_[n2-1];
        element.nodes_[2] = &nodes_[n3-1];
        element.nodes_[3] = &nodes_[n4-1];
        // rank番号を記録する。このメソッドの中では四隅の節点にrank番号を記録している
        element.setRank(rank);
    }

    // 読み込み終了なのでクローズ
    rdr.close();

    Logger::out << "Finished reading mesh file" << std::endl;
}

void CfdProcData::readBoundaryFile() {
    // 境界条件ファイルを読む
    FileReader rdr;

    Logger::out << "Reading boundary file " << params_->boundary_file_name_ << std::endl;
    // ファイル名は計算条件オブジェクトが保持している
    rdr.open(params_->boundary_file_name_);

    // 内容: 境界条件の個数(0～)
    rdr.readLine();
    int num_boundaries;
    rdr.readInt(num_boundaries, "number of boundaries");
    // num_boundariesが下限(0)、上限(仕様として決める）の範囲に入っているか、
    // チェックすることが望ましい。範囲外ならばDataException
    boundaries_.resize(num_boundaries);
    int i;
    for (i = 0; i < num_boundaries; i++) {
        Boundary *b = & boundaries_[i];
        int num_nodes;
        // 内容: 境界番号(1～) 節点数(1～)
        rdr.readLine();
        rdr.readExpectedInt(i+1, "boundary index");
        rdr.readInt(num_nodes, "number of nodes");
        int j;
        // 内容: 節点番号(1～) 節点番号 ...
        rdr.readLine();
        for (j = 0; j < num_nodes; j++) {
            int node_index;
            rdr.readInt(node_index, "node index");
            // node_indexの値の範囲チェックをすべき。

            Node *node = &nodes_[node_index - 1];
            // 指定された節点が、当プロセスで計算対象となっている場合だけ
            // boundaryオブジェクトに登録する。（境界条件の計算対象になる）
            if (node->isOnRank(params_->my_rank_)) {
                b->addNode(node);
            }
        }
        // 内容: 速度 u(x,y)の係数
        rdr.readLine();
        rdr.readDouble(b->a0_, "a0");
        rdr.readDouble(b->a1_, "a1");
        rdr.readDouble(b->a2_, "a2");
        rdr.readDouble(b->a3_, "a3");
        rdr.readDouble(b->a4_, "a4");
        rdr.readDouble(b->a5_, "a5");

        // 内容: 速度 v(x,y)の係数
        rdr.readLine();
        rdr.readDouble(b->b0_, "b0");
        rdr.readDouble(b->b1_, "b1");
        rdr.readDouble(b->b2_, "b2");
        rdr.readDouble(b->b3_, "b3");
        rdr.readDouble(b->b4_, "b4");
        rdr.readDouble(b->b5_, "b5");
    }
    rdr.close();
    Logger::out << "Finished reading boundary file" << std::endl;
}

void CfdProcData::findOwnData() {
    size_t i, j;
    for (i = 0; i < elements_.size(); i++) {
        QuadElement &element = elements_[i];
        if (element.rank_ == params_->my_rank_) {
            my_elements_.push_back(&element);
        }
    }
    for (i = 0; i < nodes_.size(); i++) {
        Node &node = nodes_[i];
        if (node.isOnRank(params_->my_rank_)) {
            Logger::out << "node " << i << " is local, and the local index is " << my_nodes_.size() << std::endl;
            node.local_index_ = my_nodes_.size();
            my_nodes_.push_back(&node);
            if (node.isOnBoundary()) {
                Logger::out << "node " << i << " is on boundary with ranks : ";
                for (j = 0; j < node.ranks_.size(); j++) {
                    int rank = node.ranks_[j];
                    if (rank != params_->my_rank_) {
                        Logger::out << " " << rank;
                        commData_->addBoundaryNode(rank, &node);
                    }
                }
                Logger::out << std::endl;
            }
        }
    }
}

void CfdProcData::readTemporalData()  {
    size_t i;
    bool restart_file_is_open;
    //指定されたファイル名に自分のランクを追加してリスタートファイルの名前を作る
    //sprintf(my_rank_temporal_file_name_, "%05d%s", params_->my_rank_, params_->temporal_file_name_.c_str());
    my_rank_temporal_file_name_ = format_string(params_->temporal_file_name_, params_->my_rank_);
    Logger::out << "Reading tmpdata file " << my_rank_temporal_file_name_ << std::endl;
    // リスタートファイルがあるか調べる
    restart_file_is_open = checkRestartFileIsThere();

    // 成功したか?
    if (! restart_file_is_open) {
        // 失敗した。例外を挙げる。
        // // throw コンストラクタ呼び出し の構文であることに注意。
        // // 無名のインスタンスが作成され、それがthrowされる。
        // throw IoException(__FILE__, __LINE__, file_name_);

        // ファイルがなかったものとして変数を0で初期化する．
        clearFieldData();
        Logger::out << "No tmpdata file and clearing to zero state" << std::endl;
    }
    else{
        // 成功した。変数を読み出す．
        initFieldDataByRestartFile();
        Logger::out << "Finished reading tmpdata file " << my_rank_temporal_file_name_ << std::endl;
    }

}

bool CfdProcData::checkRestartFileIsThere(){
    bool flag;//ファイルが開いていればtrue

    // fstream::openを呼ぶ
    std::fstream restart_file;
    // リスタートファイルを開いてみる
    restart_file.open(my_rank_temporal_file_name_.c_str(), std::ios::in);
    // ファイルが開いているか調べる
    flag=restart_file.is_open();
    // ファイルをクローズする
    restart_file.close();
    return flag;
}

void CfdProcData::clearFieldData(){
    size_t i;
    // ファイルがなかったものとして変数を0で初期化する．
    for (i = 0; i < my_nodes_.size(); i++) {
      my_nodes_[i]->vel_.set(0.,0.);
      my_nodes_[i]->d_vel_.set(0.,0.);
    }
    for (i = 0; i < my_elements_.size(); i++) {
      my_elements_[i]->p_=0.;
    }
}


void CfdProcData::initFieldDataByRestartFile(){
    size_t i;
    double value;

    // fstream::openを呼ぶ．読み込み専用で開く
    std::ifstream restart_file;
    restart_file.open(my_rank_temporal_file_name_.c_str(), std::ios::binary|std::ios::in);

    // 時刻を読み出す
    restart_file.read((char*)&value, sizeof(value));
    state_->initT(value, params_->delta_t_);

    // 速度を読み出す．
    for (i = 0; i < my_nodes_.size(); i++) {
        restart_file.read((char*)&value, sizeof(value));
        my_nodes_[i]->vel_.x_=value;
        restart_file.read((char*)&value, sizeof(value));
        my_nodes_[i]->vel_.y_=value;
    }
    for (i = 0; i < my_elements_.size(); i++) {
        restart_file.read((char*)&value, sizeof(value));
        my_elements_[i]->p_=value;
    }

    // ファイルをクローズする
    restart_file.close();
}

void CfdProcData::writeTemporalData(){
    size_t i;
    double value;

    std::string my_rank_temporal_file_name_;//指定されたファイル名に自分のランクを追加
    my_rank_temporal_file_name_ = format_string(params_->temporal_file_name_, params_->my_rank_);

    // fstream::openを呼ぶ
    std::ofstream restart_file;
    restart_file.open(my_rank_temporal_file_name_.c_str(), std::ios::binary|std::ios::out);
    // 時刻を書き出す
    value=state_->getT();
    restart_file.write((char*)&value, sizeof(value));
    // 速度を書き出す
    for (i = 0; i < my_nodes_.size(); i++) {
        value=my_nodes_[i]->vel_.x_;
        restart_file.write((char*)&value, sizeof(value));
        value=my_nodes_[i]->vel_.y_;
        restart_file.write((char*)&value, sizeof(value));
    }
    // 圧力を書き出す
    for (i = 0; i < my_elements_.size(); i++) {
        value=my_elements_[i]->p_;
        restart_file.write((char*)&value, sizeof(value));
    }
    restart_file.close();
}

void CfdProcData::calcInvariants1() {
    size_t i;
    double re = params_->re_;

    Logger::out << "CfdProcData::calcInvariants1() start" << std::endl;
    for (i = 0; i < my_nodes_.size(); i++) {
        my_nodes_[i]->clearMass();
    }
    for (i = 0; i < my_elements_.size(); i++) {
        my_elements_[i]->calcInvariants1(re);
    }

    commData_->gatherBoundaryNodeMass();
    Logger::out << "CfdProcData::calcInvariants1() end" << std::endl;
}

void CfdProcData::calcInvariants2() {
    Logger::out << "CfdProcData::calcInvariants2() start" << std::endl;
    commData_->distributeBoundaryNodeMass();

    size_t i;

    double delta_t = params_->delta_t_;
    double relaxation = params_->relaxation_;
    for (i = 0; i < my_nodes_.size(); i++) {
        my_nodes_[i]->calcInvMass();
        // Logger::out << "Node " << i << " : inv_mass " << my_nodes_[i]->inv_m_ << std::endl;
        my_nodes_[i]->calcDtByM(delta_t);
        // Logger::out << "Node " << i << " : dt/m " << my_nodes_[i]->delta_t_by_m_ << std::endl;
    }
    for (i = 0; i < my_elements_.size(); i++){
        my_elements_[i]->calcInvariants2(delta_t, relaxation);
    }
    Logger::out << "CfdProcData::calcInvariants2() end" << std::endl;
}

void CfdProcData::calcVelocityPrediction() {
    size_t i;
    for(i = 0; i < my_elements_.size(); i++){
        my_elements_[i]->calcVelocityPrediction();
    }
}

void CfdProcData::gatherVelocityDelta(){
    commData_->gatherBoundaryNodeVelocityDelta();
}

void CfdProcData::distributeVelocityDelta(){
    commData_->distributeBoundaryNodeVelocityDelta();
}

void CfdProcData::applyVelocityDeltaAndClear() {
    size_t i;
    for(i = 0; i < my_nodes_.size(); i++) {
        my_nodes_[i]->applyVelocityDelta();
        my_nodes_[i]->clearVelocityDelta();
    }
}

bool CfdProcData::calcDivergenceAndCorrect() {
    size_t i;
    double epsilon=params_->epsilon_;
    bool flag=0;
    double D;
    for(i = 0; i < my_elements_.size(); i++){
        // 判別式D_の計算
        my_elements_[i]->calcDiscriminant();
        D = my_elements_[i]->D_ ;

        // 閾値を超えた要素に対して補正を行う
        if(D > epsilon || D < (-epsilon)){
            my_elements_[i]->correctVelocity();
            flag=1;
        }
    }
    return flag;
}

void CfdProcData::clearVelocityDelta() {
    size_t i;
    for(i = 0; i < my_nodes_.size(); i++) {
        my_nodes_[i]->clearVelocityDelta();
    }
}

void CfdProcData::applyBoundaryConditions() {
    size_t i;
    double t_ramp = params_->t_ramp_;
    double t = state_->getT();
    for(i = 0; i < boundaries_.size(); i++){
        boundaries_[i].apply(t,t_ramp);
    }
}

void CfdProcData::writeFieldData(){
    // vtk形式のファイル出力クラス
    VtkWriter vtk;
    // プロセッサーの節点、要素を渡して初期化する
    vtk.init(my_nodes_, my_elements_);
    // 出力ファイルを生成して開く
    vtk.open(params_->output_file_name_, params_->my_rank_, state_->getRound());
    // vtkファイルのヘッダーを作成する
    vtk.writeHeader();
    // 節点の位置を設定する
    vtk.writePoints();
    // 要素を構成する節点を指定する
    vtk.writeCells();
    //　速度データの出力
    vtk.writeVelocityData();
    //　圧力データの出力
    vtk.writePressureData();
    // ファイルを閉じる
    vtk.close();
}
