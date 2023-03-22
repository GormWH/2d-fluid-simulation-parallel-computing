/*
 * Params.h
 */
#ifndef _PARAMS_H
#define _PARAMS_H

#include <string>
#include <iostream>
#include <IoException.h>
#include <DataException.h>

/*
 * 計算条件を保持するクラス
 */
class Params {
public:
    // 総プロセス数
    int num_procs_;

    // 自身のランク
    int my_rank_;

    // レイノルズ数
    double re_;

    // Δt（無次元時間）
    double delta_t_;

    // シミュレーション継続時間（無次元時間）
    double duration_;

    // 境界条件の速度の加速期間（無次元時間）
    double t_ramp_;

    // 速度補正処理の終了判定のε。Max|Div_e|<εが終了条件
    double epsilon_;

    // 速度補正処理の最大補正回数
    int max_corrections_;

    // 速度場のデータファイルを出力する間隔。単位は速度発展のステップ数
    int n_interval_;

    // 速度補正の緩和係数
    double relaxation_;

    // 形状ファイルのパス名
    std::string mesh_file_name_;

    // 境界条件ファイルのパス名
    std::string boundary_file_name_;

    // 出力ファイルのパス名
    std::string output_file_name_;

    // リスタートファイルのパス名
    std::string temporal_file_name_;

    // 初期化。MPIの初期化関数を呼んでから当関数を呼ぶこと。
    // np : 総プロセス数
    // rank : 自プロセスのrank
    // filename : 計算条件ファイルのファイル名
    // 例外:
    //   IoException : ファイルが開けない場合など
    //   DataException : ファイルの内容が正しくない場合
    void init(int np, int rank, const char *filename);

};

#endif
