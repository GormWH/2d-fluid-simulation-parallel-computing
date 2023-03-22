/*
 * VtkWriter.h
 */
#ifndef _VTK_WRITER_H
#define _VTK_WRITER_H

#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include <Node.h>
#include <QuadElement.h>

#include <IoException.h>
#include <DataException.h>

/*
 * ParaViewを用いた計算データの可視化のために'vtk'形式の出力ファイルを書く。
 */

class VtkWriter {
private:
    std::string file_name_;
    std::fstream out_;
    std::stringstream cur_line_;

    std::vector<Node *> my_nodes_;
    std::vector<QuadElement *> my_elements_;

public:
    // constructor, destructor
    VtkWriter();
    ~VtkWriter();
    // 現在の節点と要素の情報を渡して初期化
    void init(const std::vector<Node *> my_nodes, const std::vector<QuadElement *> my_elements);
    // ファイルを生成して開く ファイル名にプロセッサー番号と時間発展回数が入る
    void open(const std::string filename, const int rank, const int round);
    // ファイルを閉じる
    void close();
    // vtkファイルのヘッダーを書く
    void writeHeader();
    // 節点の位置を設定する
    void writePoints();
    // 節点と要素を結びつける
    void writeCells();
    // 速度場を記録する
    void writeVelocityData();
    // 圧力場を記録する
    void writePressureData();
};

std::string format_string(const std::string format, ...);

#endif
