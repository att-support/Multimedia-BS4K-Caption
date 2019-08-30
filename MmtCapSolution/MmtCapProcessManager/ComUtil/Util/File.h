/*
 * File.h
 *
 *  Created on: 2017/07/11
 *      Author: root
 */

#ifndef UTIL_FILE_H_
#define UTIL_FILE_H_

#include <iostream>
#include <fstream>
#include <string>

class File {
public:
	File();
	virtual ~File();
	int Read(const std::string& path);		//ファイル読み込み
	int Read(const std::string& path, int& pos);		//ファイル読み込み(最終位置返却)
	int Read(const std::string& path, int beginPos, int& pos);	//位置指定ファイル読み込み(最終位置返却)
	int Write(const std::string& path);	//ファイル書き出し
	int Write();								//ファイル書き出し（上書き）
	std::string GetFilePath();
	void SetFilePath(const std::string& path);

protected:
	std::string _path;						//ファイルパス

	//サブクラスで実装
	virtual void ReadOpen(const std::string& path, std::ifstream& fin);		//ファイル読み込みオープン（サブクラスでオーバーライド実装）
	virtual void WriteOpen(const std::string& path, std::ofstream& fout);		//ファイル書き出しオープン（サブクラスでオーバーライド実装）
	virtual int WriteCheck(const std::string& path);								//ファイル書き出しチェック（必要に応じてサブクラスでオーバーライド実装）
	virtual int _Read(std::ifstream& fin);											//ファイル読み込み処理（サブクラスでオーバーライド実装）
	virtual int _Write(std::ofstream& fout);										//ファイル書き出し処理（サブクラスでオーバーライド実装）
};

#endif /* UTIL_FILE_H_ */
