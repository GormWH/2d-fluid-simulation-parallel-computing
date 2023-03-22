/*
 * CfdDriver.cpp
 */

#include <CfdDriver.h>
#include <Logger.h>

#include <mpi.h>

CfdDriver::~CfdDriver() {
}

CfdDriver::CfdDriver() {
}

void CfdDriver::init(int np, int rank, const char *filename) {
    // ログファイルを開き、最初のメッセージを出力。
    Logger::openLog("abmac2d", rank);
    Logger::out << "Starting." << std::endl;
    // 計算条件ファイルを読み、自身のrankを記録する
    params_.init(np, rank, filename);
    // 通信バッファを初期化する
    commData_.init(&params_, &state_);
    // コミュニケータを初期化する。
    communicator_.init(&params_, &state_, &commData_);
    // プロセスデータを初期化する。
    procData_.init(&params_, &state_, &commData_);
    // 計算の経過を初期化する
    state_.reset();

    correctVelocityCounter_=0;

    // 一旦同期を取る
    MPI_Barrier(MPI_COMM_WORLD);
}

void CfdDriver::readDataFile() {
    // 形状データファイルを読む
    procData_.readMeshFile();
    // 読み込んだ形状データの中から、自プロセスが担当するデータを特定する
    procData_.findOwnData();
    // 境界条件データを読む
    procData_.readBoundaryFile();
}

// リスタートファイルがあれば変数に読み込み，なければ0で初期化
void CfdDriver::initVariables() {
    procData_.readTemporalData();
}

void CfdDriver::calcInvariants() {
    // ループ不変量の計算その１：隣接四角形要素と計算値の合算が必要ない範囲での計算
    // 境界上の節点データも登録する
    procData_.calcInvariants1();
    // プロセス境界を越えた計算値（節点の質量値）の合算のための通信
    // 登録されたデータを交換
    communicator_.exchangeBoundaryValues();
    // 交換されたデータをそれぞれの節点で足し合わせる
    // ループ不変量の計算その２：隣接四角形要素との合算後のデータを踏まえた計算
    procData_.calcInvariants2();
}

void CfdDriver::doStep() {
    // 速度予測値の計算
    procData_.calcVelocityPrediction();

    // 隣接プロセッサーと速度変化量の共有
    procData_.gatherVelocityDelta();
    communicator_.exchangeBoundaryValues();
    procData_.distributeVelocityDelta();

    // 速度変化の適用
    procData_.applyVelocityDeltaAndClear();

    //境界条件の適用
    procData_.applyBoundaryConditions();

    // 速度補正ループ
    correctVelocity();

    // 周期的にデータを出力
    if(state_.getRound()%params_.n_interval_ == 0){
        procData_.writeFieldData();
    }

    // 次ステップの状態表示
    state_.nextRound(params_.delta_t_);
}

// 速度補正ループ
void CfdDriver::correctVelocity() {
    bool isNotDivergence;
    bool isNotDivergenceAll;
    size_t i = 0;
    int max_corrections = params_.max_corrections_;
    for(i = 0; i < max_corrections; i++){
        // 閾値を超える要素があれば補正してtrueを返す
        isNotDivergence=procData_.calcDivergenceAndCorrect();

        // MPI_Allreduce(void* send_data,void* recv_data,int count,MPI_Datatype datatype,MPI_Op op,MPI_Comm communicator)
        MPI_Allreduce(&isNotDivergence, &isNotDivergenceAll, 1, MPI_C_BOOL, MPI_LOR, MPI_COMM_WORLD);
        if(!isNotDivergenceAll){
            procData_.clearVelocityDelta();
            Logger::out << "Correction ended at correction step : " << i << std::endl;
            break;//収束してないのがなければ速度補正ループ終了．
        }

        // doStepと同様な操作
        procData_.gatherVelocityDelta();
        communicator_.exchangeBoundaryValues();
        procData_.distributeVelocityDelta();

        procData_.applyVelocityDeltaAndClear();

        procData_.applyBoundaryConditions();
    }
    correctVelocityCounter_+=i;
    Logger::out << "Correction Total is : " << correctVelocityCounter_ << std::endl;
}

// リスタートファイルの書き出し
void CfdDriver::outputVariables() {
    procData_.writeTemporalData();
}

void CfdDriver::finalize() {
    // 最後まで達したことをログに記録してクローズ
    Logger::out << "Ending." << std::endl;
    Logger::closeLog();
}

void CfdDriver::tryMain(double real_time_start) {
    // (2) データファイルの読み込み。
    readDataFile();
    initVariables();

    // (3) ループ不変量の計算
    calcInvariants();

    /*
     * 時間発展ループ
     */
    timeLoop(real_time_start);

    /*
     * 後処理
     */

    //リスタート用
    outputVariables();
    Logger::out << "variables output." << std::endl;

    // ドライバーの後処理。ログファイルを閉じる。
    finalize();
}

void CfdDriver::timeLoop(double real_time_start) {
    while(true){
    // if(params_.my_rank_==0)printf("main loop start time: %lf\n",MPI_Wtime()-real_time_start);
    // for(size_t i=0;i<100;i++){
        double t = state_.getT();
        double delta_t = params_.delta_t_;
        double duration = params_.duration_;

        bool isNotElapseTimePassed=0;
        bool isNotElapseTimePassedAll=0;
        isNotElapseTimePassed=!(MPI_Wtime()-real_time_start>14*60);
        MPI_Allreduce(&isNotElapseTimePassed, &isNotElapseTimePassedAll, 1, MPI_C_BOOL, MPI_LOR, MPI_COMM_WORLD);

        if(t > duration + delta_t){
            Logger::out << "Successivly ended time loop" << std::endl;
            break;
        } else if(!isNotElapseTimePassedAll){
            // 14分経過していたら終了
            Logger::out << "elapse time - 1 minute (14 minute) is passed." << std::endl;
            Logger::out << "computational time t is " << t << std::endl;
            break;
        }
        Logger::out << "MPI_Wtime-real_time_start is " << MPI_Wtime()-real_time_start << std::endl;
        doStep();
    }
    if(params_.my_rank_==0)printf("total time: %lf\n",MPI_Wtime()-real_time_start);
    if(params_.my_rank_==0)printf("total correction: %d\n",correctVelocityCounter_);
}
