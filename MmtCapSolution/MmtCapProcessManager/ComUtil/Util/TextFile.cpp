/*
 * TextFile.cpp
 *
 *  Created on: 2017/07/11
 *      Author: root
 */

#include <errno.h>
#include <string.h>
#include <Util/TextFile.h>
#include <Util/DbgPrint.h>

TextFile::TextFile() {
	m_isOverWriteMode = false;
	m_isEnableEmptyFile = false;
}

TextFile::~TextFile() {
}

void TextFile::SetOverWriteMode() {
	m_isOverWriteMode = true;
}

//空ファイルを許可
void TextFile::EnableEmptyFile() {
	m_isEnableEmptyFile = true;
}


int TextFile::WriteCrLf(const std::string& path){
	if(WriteCheck(path) != 0){
		return -1;
	}

	std::ofstream fout;
	WriteOpen(path, fout);

    if (fout.fail()) {
        COUT("failed to open the file. errno=%d (%s) path=%s",errno, strerror(errno), path.c_str());
        return -1;
    }

    int retVal = _WriteEx(fout,true);
    if(retVal != 0){
    	COUT("failed to write the file. path=%s",path.c_str());
    }

    fout.close();

    return retVal;
}

void TextFile::AddLine(const std::string& line){
	fileData.push_back(line);
}

void TextFile::ReadOpen(const std::string& path, std::ifstream& fin) {
	fin.open( path.c_str(), std::ios::in );
}

void TextFile::WriteOpen(const std::string& path, std::ofstream& fout) {
	if(m_isOverWriteMode){
		fout.open(path.c_str(), std::ios::out | std::ios::trunc );
	}
	else{
		fout.open(path.c_str(), std::ios::out | std::ios::app );
	}
}

int TextFile::WriteCheck(const std::string& path) {
	if(!m_isEnableEmptyFile && fileData.empty()){
    	COUT("failed to write the file. (nothing write data) path=%s",path.c_str());
		return -1;
	}
	return 0;
}

int TextFile::_Read(std::ifstream& fin) {
    fileData.clear();
	std::string lineData;
    while (getline(fin, lineData)) {
    	if(lineData.size() > 0){
        	if(lineData.at(lineData.size()-1) == '\r'){
        		lineData.erase(--lineData.end());
        	}
          fileData.push_back(lineData);
    	}
    }
    if(!fileData.empty()){
    	//BOM削除
       if( fileData[0].size() >= 3) {
       	std::string bom = fileData[0].substr(0,3);
       	if(((unsigned char)bom[0] == 0xEF) &&
       		((unsigned char)bom[1] == 0xBB) &&
				((unsigned char)bom[2] == 0xBF))
       		{
       		fileData[0] = fileData[0].substr(3);
       		}
        }
    }
   return 0;
}

int TextFile::_Write(std::ofstream& fout) {
	return _WriteEx(fout,false);
}

int TextFile::_WriteEx(std::ofstream& fout, bool crLf) {
	for(TextFileDataType::iterator itr = fileData.begin();
			itr != fileData.end(); itr++){
		if(crLf){
			fout << ((*itr)+"\r") << std::endl;
		} else {
			fout << (*itr) << std::endl;
		}
	}
   return 0;
}

void TextFile::ClearData() {
	fileData.clear();
}
