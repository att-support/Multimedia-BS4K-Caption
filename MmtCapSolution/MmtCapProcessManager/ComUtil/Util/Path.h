/*
 * Path.h
 *
 *  Created on: 2017/07/12
 *      Author: attractor
 */

#ifndef UTIL_PATH_H_
#define UTIL_PATH_H_

#include <string>
#include <vector>
#include <Util/SysTime.h>

class Path {
public:
	Path();
	virtual ~Path();
	static bool IsExist(const std::string& filePath);											//パス存在確認（TRUE:存在する）
	static bool IsDir(const std::string& filePath);												//ディレクトリ確認（TRUE:引数のパスはディレクトリ）
	static int CopyFile(const std::string& srcPath, const std::string& dstPath);				//ファイルコピー
	static int CopyFileExe(const std::string& srcPath, const std::string& dstPath);				//ファイルコピー
	static int RemoveFile(const std::string& filePath);											//ファイル削除
	static int RemoveAllFile(const std::string& dirPath);										//全ファイル削除（指定ディレクトリ下のファイルを全て削除）
	static int RenameFile(const std::string& srcPath, const std::string& dstPath);			//ファイルリネーム
	static int MakeDir(const std::string& dirPath);												//ディレクトリ作成
	static int CopyDir(const std::string& srcPath, const std::string& dstPath);				//ディレクトリコピー
	static int RemoveDir(const std::string& dirPath);											//ディレクトリ削除（指定ディレクトリ以下を全て削除）
	static int RenameDir(const std::string& srcPath, const std::string& dstPath);			//ディレクトリリネーム
	static int GetFileList(const std::string& path, std::vector<std::string>& fileList);	//ファイルリスト取得（指定ディレクトリ下のファイル名一覧を取得(ディレクトリは除く)）
	static int GetDirList(const std::string& path, std::vector<std::string>& fileList);		//ディレクトリリスト取得（指定ディレクトリ下のディレクトリ名一覧を取得(ファイルは除く)）
	static std::string Combine(const std::string& path1, const std::string& path2);			//パス結合
	static std::string GetFileName(const std::string& path);									//パスからファイル名を取得
	static std::string GetFileNameWithoutExtension(const std::string& path);					//パスから拡張子を除いたファイル名を取得
	static std::string GetExtension(const std::string& path);									//パスから拡張子を取得
	static std::string GetCurrentDir();															//カレントディレクトリパス取得
	static bool IsEmptyDir(const std::string& path);												//ディレクトリが空かどうかを検査（TRUE:空）
	static SysTime GetTimestamp(const std::string& path);									//タイムスタンプ取得
	static int GetFileSize(const std::string& path);
	static void CashClear(const std::string& path);												//キャッシュクリア

private:
	static int GetPathList(const std::string& path, std::vector<std::string>& fileList, bool isFile);	//パス一覧取得
};

#endif /* UTIL_PATH_H_ */
