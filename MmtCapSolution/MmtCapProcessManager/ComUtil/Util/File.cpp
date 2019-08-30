/*
 * File.cpp
 *
 *  Created on: 2017/07/11
 *      Author: root
 */

#include <errno.h>
#include <string.h>
#include <Util/DbgPrint.h>
#include "File.h"

File::File() {
	_path = "";
}

File::~File() {
}

std::string File::GetFilePath() {
	return _path;
}

void File::SetFilePath(const std::string& path) {
	_path = path;
}

int File::Read(const std::string& path) {
	int pos;
	return Read(path, pos);
}

int File::Read(const std::string& path, int& pos) {
	return Read(path, 0, pos);
}

int File::Read(const std::string& path, int beginPos, int& pos) {
	_path = path;
	pos = 0;
    std::ifstream fin;

    ReadOpen(path, fin);

    if (fin.fail()){
        COUT("failed to open the file. errno=%d (%s) path=%s",errno, strerror(errno), path.c_str());
        return -1;
    }

    //シーク
    if(beginPos > 0){
        fin.seekg((std::ifstream::pos_type)beginPos, std::ios_base::beg);
    }
    //読み込み開始
    int retVal = _Read(fin);
    if(retVal == 0){
    	fin.clear();
    	fin.seekg((std::ifstream::pos_type)0, std::ios_base::end);
    	pos = (int)fin.tellg();
    }
    else{
        COUT("failed to read the file. path=%s",path.c_str());
    }

    fin.close();

    return retVal;
}

int File::Write() {
	if(_path.empty() || (_path == "")){
		return -1;
	}
	return Write(_path);
}

int File::Write(const std::string& path) {
	if(WriteCheck(path) != 0){
		return -1;
	}

	std::ofstream fout;
	WriteOpen(path, fout);

    if (fout.fail()) {
        COUT("failed to open the file. errno=%d (%s) path=%s",errno, strerror(errno), path.c_str());
        return -1;
    }

    int retVal = _Write(fout);
    if(retVal != 0){
    	COUT("failed to write the file. path=%s",path.c_str());
    }

    fout.close();

    return retVal;
}

void File::ReadOpen(const std::string& path, std::ifstream& fin) {
}

void File::WriteOpen(const std::string& path, std::ofstream& fout) {
}

int File::WriteCheck(const std::string& path) {
	return -1;
}

int File::_Read(std::ifstream& fin) {
	return -1;
}

int File::_Write(std::ofstream& fout) {
	return -1;
}


