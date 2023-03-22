/*
 * CfdDriver_sp.cpp
 */

#include <CfdDriver_sp.h>
#include <Logger.h>

CfdDriver_sp::~CfdDriver_sp() {
}

CfdDriver_sp::CfdDriver_sp() {
}

void CfdDriver_sp::init(int np, int rank, const char *filename) {
    // ログファイルを開き、最初のメッセージを出力。
    Logger::openLog("abmac2d_sp", rank);
    Logger::out << "Starting." << std::endl;
    // 計算条件ファイルを読み、自身のrankを記録する
    params_.init(np, rank, filename);
    // 通信バッファを初期化する
    commData_.init(&params_, &state_);
    // プロセスデータを初期化する。
    procData_.init(&params_, &state_, &commData_);
    // 計算の経過を初期化する
    state_.reset();
}

void CfdDriver_sp::readDataFile() {
    // 形状データファイルを読む
    procData_.readMeshFile();
    // 読み込んだ形状データの中から、自プロセスが担当するデータを特定する
    procData_.findOwnData();
    // 境界条件データを読む
    procData_.readBoundaryFile();
}

// リスタートファイルがあれば変数に読み込み，なければ0で初期化
void CfdDriver_sp::initVariables() {
    procData_.readTemporalData();
}

void CfdDriver_sp::calcInvariants() {
    // ループ不変量の計算その1: 隣接四角形要素と計算値の合算が必要ない範囲での計算
    procData_.calcInvariants1();

    // (省略): プロセス境界を越えた計算値（節点の質量値）の合算のための通信

    // ループ不変量の計算その2：隣接四角形要素との合算後のデータを踏まえた計算
    procData_.calcInvariants2();
}

void CfdDriver_sp::timeLoop() {
    while(true){
        double t = state_.getT();
        double duration = params_.duration_;
        if(t > duration){
            Logger::out << "Successivly ended time loop" << std::endl;
            break;
        }
        doStep();
    }
}

void CfdDriver_sp::doStep() {
    procData_.calcVelocityPrediction();

    // 通信処理を省略
    // procData_.gatherVelocityDelta();
    // communicator_.exchangeBoundaryValues();
    // procData_.distributeVelocityDelta();

    procData_.applyVelocityDeltaAndClear();
    procData_.applyBoundaryConditions();

    correctVelocity();

    if(state_.getRound()%params_.n_interval_ == 0){
        procData_.writeFieldData();
    }
    Logger::out << "Now at round " << state_.getRound() << std::endl;
    state_.nextRound(params_.delta_t_);
}

void CfdDriver_sp::correctVelocity() {
    bool isNotDivergence;
    // bool isNotDivergenceAll;
    int i, j;
    int max_corrections = params_.max_corrections_;
    for(i = 0; i < max_corrections; i++){
        // 閾値を超える要素があれば補正してtrueを返す
        isNotDivergence=procData_.calcDivergenceAndCorrect();

        // MPI_Allreduce(void* send_data,void* recv_data,int count,MPI_Datatype datatype,MPI_Op op,MPI_Comm communicator)
        // MPI_Allreduce(&isNotDivergence, &isNotDivergenceAll, 1, MPI_LOGICAL, MPI_LOR, MPI_COMM_WORLD);
        if(!isNotDivergence){
            procData_.clearVelocityDelta();
            Logger::out << "Ended at correction step : " << i << std::endl;
            break;//全ての要素が収束していれば速度補正量を初期化してループ終了．
        }

        // procData_.gatherVelocityDelta();
        // communicator_.exchangeBoundaryValues();
        // procData_.distributeVelocityDelta();

        procData_.applyVelocityDeltaAndClear();
        procData_.applyBoundaryConditions();
    }
}

// リスタートファイルの書き出し
void CfdDriver_sp::outputVariables() {
    procData_.writeTemporalData();
}

void CfdDriver_sp::finalize() {
    // 最後まで達したことをログに記録してクローズ
    Logger::out << "Ending." << std::endl;
    Logger::closeLog();
}
