/*
 * TextFile.h
 *
 *  Created on: 2017/07/11
 *      Author: root
 */

#ifndef UTIL_TEXTFILE_H_
#define UTIL_TEXTFILE_H_

#include <Util/File.h>

#include <string>
#include <vector>
#include "File.h"

typedef std::vector<std::string> TextFileDataType;

class TextFile: public File {
public:
	TextFile();
	virtual ~TextFile();
	int WriteCrLf(const std::string& path);		//CRLF書き出し（行末にCR・LFを付加して書き出す）
	void AddLine(const std::string& line);		//行データ追加（書き出し用データに1行分の文字列を追加する）
	void ClearData();								//ファイルデータクリア
	void SetOverWriteMode();						//上書きモード
	void EnableEmptyFile();						//空ファイルを許可
	TextFileDataType fileData;						//ファイルデータ（文字列配列）

protected:
	bool m_isOverWriteMode;							//上書きモード
	bool m_isEnableEmptyFile;						//true:空ファイルを許可

	virtual void ReadOpen(const std::string& path, std::ifstream& fin);		//ファイル読み込みオープン（オーバーライド）
	virtual void WriteOpen(const std::string& path, std::ofstream& fout);		//ファイル書き出しオープン（オーバーライド）
	virtual int WriteCheck(const std::string& path);								//ファイル書き出しチェック処理（オーバーライド）
	virtual int _Read(std::ifstream& fin);											//ファイル読み込み処理（オーバーライド）
	virtual int _Write(std::ofstream& fout);										//ファイル書き出し処理（オーバーライド）
	int _WriteEx(std::ofstream& fout, bool crLf);									//ファイル書き出し処理本体
};

#endif /* UTIL_TEXTFILE_H_ */
