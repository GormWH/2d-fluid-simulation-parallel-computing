/*
 * VtkWriter.cpp
 */
#include <VtkWriter.h>
#include <Logger.h>
#include <stdarg.h>
#include <fstream>
#include <iostream>
#include <string>

VtkWriter::VtkWriter() {}
VtkWriter::~VtkWriter(){
    if (out_.is_open())    {
        out_.close();
    }
}

void VtkWriter::init(const std::vector<Node *> my_nodes, const std::vector<QuadElement *> my_elements) {
    my_nodes_ = my_nodes;
    my_elements_ = my_elements;
}

std::string format_string(const std::string format, ...){
    int size = ((int)format.size()) * 2;
    std::string buffer;
    va_list ap;
    while (1) {
        buffer.resize(size);
        va_start(ap, format);
        int n = vsnprintf((char*)buffer.data(), size, format.c_str(), ap);
        va_end(ap);
        if (n > -1 && n < size) {
        buffer.resize(n);
        return buffer;
        }
        if (n > -1)
        size = n + 1;
        else
        size *= 2;
    }
    return buffer;
}

void VtkWriter::open(const std::string file_name, int rank, int round)
{
    file_name_ = format_string(file_name, rank, round);
    out_.open(file_name_.c_str(), std::ios::out);
    if(! out_.is_open())    {
        throw IoException(__FILE__, __LINE__, file_name_);
    }
    Logger::out << "File opened: " << file_name_ << std::endl;
}

void VtkWriter::close() {
    out_.close();
    Logger::out << "File closed: " << file_name_ << std::endl;
}

void VtkWriter::writeHeader() {
    Logger::out << "start VtkWriter::writeHeader() in " << file_name_ << std::endl;
    out_ << "# vtk DataFile Version 2.0" << std::endl;
    out_ << file_name_ << std::endl;
    out_ << "ASCII" << std::endl;
    out_ << "DATASET UNSTRUCTURED_GRID" << std::endl;
    Logger::out << "end VtkWriter::writeHeader() in " << file_name_ << std::endl;
}

void VtkWriter::writePoints() {
    Logger::out << "start VtkWriter::writePoints() in " << file_name_ << std::endl;
    out_ << "POINTS " << my_nodes_.size() << " double" << std::endl;
    for(int i = 0; i < my_nodes_.size(); i++){
        out_ << my_nodes_[i]->pos_.x_ << " " << my_nodes_[i]->pos_.y_ << " 0.0" << std::endl;
    }
    out_ << std::endl;
    Logger::out << "end VtkWriter::writePoints() in " << file_name_ << std::endl;
}

void VtkWriter::writeCells() {
    Logger::out << "start VtkWriter::writeCells() in " << file_name_ << std::endl;
    out_ << "CELLS " << my_elements_.size() << " " << my_elements_.size()*5 << std::endl;
    for(int i = 0; i < my_elements_.size(); i++){
        out_ << "4 ";
        for(int j = 0; j < 4; j++){
            out_ << my_elements_[i]->nodes_[j]->local_index_ << " ";
        }
        out_ << std::endl;
    }
    out_ << std::endl;

    out_ << "CELL_TYPES " << my_elements_.size() << std::endl;
    for(int i = 0; i < my_elements_.size(); i++){
        out_ << "9" << std::endl;
    }
    out_ << std::endl;
    Logger::out << "end VtkWriter::writeCells() in " << file_name_ << std::endl;
}

void VtkWriter::writeVelocityData() {
    Logger::out << "start VtkWriter::writeVelocityData() in " << file_name_ << std::endl;
    out_ << "POINT_DATA " << my_nodes_.size() << std::endl;
    out_ << "VECTORS velocity double" << std::endl;
    for(int i = 0; i < my_nodes_.size(); i++){
        out_ << my_nodes_[i]->vel_.x_ << " " << my_nodes_[i]->vel_.y_ << " 0" << std::endl;
    }
    out_ << std::endl;
    Logger::out << "end VtkWriter::writeVelocityData() in " << file_name_ << std::endl;
}

void VtkWriter::writePressureData() {
    Logger::out << "start VtkWriter::writePressureData() in " << file_name_ << std::endl;
    out_ << "CELL_DATA " << my_elements_.size() << std::endl;
    out_ << "SCALARS pressure double" << std::endl;
    out_ << "LOOKUP_TABLE default" << std::endl;
    for(int i = 0; i < my_elements_.size(); i++){
        out_ << my_elements_[i]->p_ << std::endl;
    }
    out_ << std::endl;
    Logger::out << "end VtkWriter::writePressureData() in " << file_name_ << std::endl;
}

