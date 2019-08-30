/*
 * Path.cpp
 *
 *  Created on: 2017/07/12
 *      Author: attractor
 */

#if defined(WIN32) || defined(WIN64)
#include <direct.h>
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif /*_WINSOCKAPI_*/
#include <windows.h>
#else
#include <unistd.h>
#include <dirent.h>
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <Util/StringUtil.h>
#include <Util/Path.h>
#include <Util/DbgPrint.h>

Path::Path() {
}

Path::~Path() {
}

bool Path::IsExist(const std::string& filePath){
	struct stat st;
	return (stat(filePath.c_str(), &st) == 0);
}
bool Path::IsDir(const std::string& path){
	struct stat st;
	if(stat(path.c_str(), &st) == 0){
		if((st.st_mode & S_IFMT) == S_IFDIR){
			return true;
		}
		else{
			return false;
		}
	}
	else{
		return false;
	}
}
int Path::CopyFileExe(const std::string& srcPath,
		const std::string& dstPath) {
	if(!IsExist(srcPath)){
		COUT("Failed to copy the file. (not exist the source file.) srcPath=%s dstPath=%s",
				srcPath.c_str(),dstPath.c_str());
		return -1;
	}
	int ret = 0;
	try {
		std::ifstream ifs(srcPath.c_str(), std::ios_base::binary);
		std::ofstream ofs(dstPath.c_str(), std::ios_base::binary);
		ofs << ifs.rdbuf();
		if (ifs.fail()||ofs.fail()){
			//0バイトファイルコピーのときofsがエラーとなるので、コピー先ファイルが存在していない場合にエラーとする
			if(!IsExist(dstPath)){
				COUT("Failed to copy the file. srcPath=%s dstPath=%s",
						srcPath.c_str(),dstPath.c_str());
				ret = -1;
			}
		}
		else{
		}
	}
	catch (std::exception& ex){
		COUT("Failed to copy the file. (%s) srcPath=%s dstPath=%s",
				ex.what(),srcPath.c_str(),dstPath.c_str());
		ret = -1;
	}
	return ret;
}
int Path::CopyFile(const std::string& srcPath,
		const std::string& dstPath) {
	if(!IsExist(srcPath)){
		COUT("Failed to copy the file. (not exist the source file.) srcPath=%s dstPath=%s",
				srcPath.c_str(),dstPath.c_str());
		return -1;
	}
	int ret = 0;
	try {
		std::ifstream ifs(srcPath.c_str(), std::ios_base::binary);
		std::ofstream ofs(dstPath.c_str(), std::ios_base::binary);
		ofs << ifs.rdbuf();
		if (ifs.fail()||ofs.fail()){
			//0バイトファイルコピーのときofsがエラーとなるので、コピー先ファイルが存在していない場合にエラーとする
			if(!IsExist(dstPath)){
				COUT("Failed to copy the file. srcPath=%s dstPath=%s",
						srcPath.c_str(),dstPath.c_str());
				ret = -1;
			}
		}
		else{
		}
	}
	catch (std::exception& ex){
		COUT("Failed to copy the file. (%s) srcPath=%s dstPath=%s",
				ex.what(),srcPath.c_str(),dstPath.c_str());
		ret = -1;
	}
	return ret;
}

int Path::RemoveFile(const std::string& filePath) {
	//キャッシュクリア
	Path::CashClear(filePath);
	if(remove(filePath.c_str()) != 0){
		COUT("Failed to delete the file. (errno=%d (%s)) filePath=%s",
				errno, strerror(errno), filePath.c_str());
		return -1;
	}
	return 0;
}

int Path::RemoveAllFile(const std::string& dirPath) {
	std::vector<std::string> fileList;
	GetFileList(dirPath, fileList);
	int ret = 0;
	for(std::vector<std::string>::iterator itr = fileList.begin();
			itr != fileList.end(); itr++ ){
		std::string path = Combine(dirPath,*itr);
		if(RemoveFile(path) != 0){
			COUT("Failed to delete the file. (path=%s)",path.c_str());
			ret = -1;
		}
	}
	return ret;
}

int Path::RenameFile(const std::string& srcPath,
		const std::string& dstPath) {
	if(!IsExist(srcPath)){
		COUT("Failed to rename the file. (not exist the source file.) srcPath=%s dstPath=%s",
				srcPath.c_str(),dstPath.c_str());
		return -1;
	}
	if(rename(srcPath.c_str(),dstPath.c_str()) != 0){
		COUT("Failed to rename the file. (errno=%d (%s)) srcPath=%s dstPath=%s",
				errno, strerror(errno), srcPath.c_str(),dstPath.c_str());
		return -1;
	}
	return 0;
}

int Path::MakeDir(const std::string& dirPath) {
	if(IsExist(dirPath)){
		return 0;
	}
	std::vector<std::string> dirs;
	StringUtil::Split(dirPath,"/",dirs);
	std::string tmpDir = "";
	for(std::vector<std::string>::iterator itr = dirs.begin(); itr != dirs.end(); itr++)
	{
		tmpDir += *itr;
		if((*itr != "") && !IsExist(tmpDir)){
			int ret_;
#if defined(WIN32) || defined(WIN64)
			ret_ = _mkdir(tmpDir.c_str());
#else
			ret_ = mkdir(tmpDir.c_str(), 0755);
#endif
			if(ret_ != 0){
				COUT("Failed to make the file. (errno=%d (%s)) dirPath=%s",
						errno, strerror(errno), dirPath.c_str());
				return -1;
			}
		}
		tmpDir += "/";
	}
	return 0;
}

int Path::CopyDir(const std::string& srcPath,
		const std::string& dstPath) {
	std::vector<std::string> dirList;
	if(GetDirList(srcPath, dirList) != 0){
		return -1;
	}
	std::vector<std::string> fileList;
	if(GetFileList(srcPath, fileList) != 0){
		return -1;
	}
	if(MakeDir(dstPath) != 0){
		return -1;
	}
	for(std::vector<std::string>::iterator itrDir = dirList.begin(); itrDir != dirList.end(); itrDir++)
	{
		std::string childSrcDir = Combine(srcPath,*itrDir);
		std::string childDstDir = Combine(dstPath,*itrDir);
		if(CopyDir(childSrcDir,childDstDir) != 0){
			return -1;
		}
	}
	for(std::vector<std::string>::iterator itrFile = fileList.begin(); itrFile != fileList.end(); itrFile++)
	{
		std::string childSrcPath = Combine(srcPath,*itrFile);
		std::string childDstPath = Combine(dstPath,*itrFile);
		if(CopyFile(childSrcPath,childDstPath) != 0){
			return -1;
		}
	}

	return 0;
}

int Path::RemoveDir(const std::string& dirPath) {
	std::vector<std::string> dirList;
	if(GetDirList(dirPath, dirList) != 0){
		COUT("Failed to remove the directory. path=%s",dirPath.c_str());
		return -1;
	}
	for(std::vector<std::string>::iterator itrDir = dirList.begin(); itrDir != dirList.end(); itrDir++)
	{
		std::string childDir = Combine(dirPath,*itrDir);
		if(RemoveDir(childDir) != 0){
			return -1;
		}
	}
	if(RemoveAllFile(dirPath) != 0){
		COUT("Failed to remove the directory. path=%s",dirPath.c_str());
		return -1;
	}

	if(IsEmptyDir(dirPath)){
		if (RemoveDirectory(dirPath.c_str()) == 0) {
			COUT("Failed to remove the directory. (errno=%d (%s)) path=%s",
				errno, strerror(errno), dirPath.c_str());
			return -1;
		}
		//if(remove(dirPath.c_str()) != 0){
		//	COUT("Failed to remove the directory. (errno=%d (%s)) path=%s",
		//			errno, strerror(errno), dirPath.c_str());
		//	return -1;
		//}
	}

	return 0;
}

int Path::RenameDir(const std::string& srcPath,
		const std::string& dstPath) {
	return RenameFile(srcPath,dstPath);
}

int Path::GetFileList(const std::string& path, std::vector<std::string>& fileList){
	return GetPathList(path,fileList,true);
}
int Path::GetDirList(const std::string& path, std::vector<std::string>& fileList){
	return GetPathList(path,fileList,false);
}

int Path::GetPathList(const std::string& path, std::vector<std::string>& fileList, bool isFile){
	if(!IsExist(path)){
		COUT("Failed to get the path. (not exist the path.) path=%s",path.c_str());
		return -1;
	}
	fileList.clear();
#if defined(WIN32) || defined(WIN64)
	char temp[_MAX_PATH];
	WIN32_FIND_DATA lp;

	std::string path_ = path;
	if (path_.at(path_.size()-1) != '/') {
		path_ += "/";
	}
	path_ += "*";

	strcpy(temp, path_.c_str());
	HANDLE h = FindFirstFile(temp, &lp);

	do
	{
		if ((strcmp(lp.cFileName, "..")) == 0 || (strcmp(lp.cFileName, ".") == 0))
		{
			continue;
		}

		if (isFile) {
			if ((lp.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
			{
				fileList.push_back(lp.cFileName);
			}
		}
		else {
			if (lp.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				fileList.push_back(lp.cFileName);
			}
		}
	} while (FindNextFile(h, &lp));

	FindClose(h);
#else
	DIR *dp;       // ディレクトリへのポインタ
	dirent* entry; // readdir() で返されるエントリーポイント

	dp = opendir(path.c_str());
	if (dp==NULL){
		COUT("Failed to get the path. (errno=%d (%s)) path=%s",
				errno, strerror(errno), path.c_str());
		return -1;
	}
	errno = 0;
	do {
		entry = readdir(dp);
		if (entry != NULL){
			if(isFile && (entry->d_type == DT_REG)){
				std::string dName = entry->d_name;
				fileList.push_back(dName);
			}
			else if(!isFile && (entry->d_type == DT_DIR)){
				if(entry->d_name[0] != '.'){
					std::string dName = entry->d_name;
					fileList.push_back(dName);
				}
			}
			else{
				;
			}
		}
		else{
			if(errno != 0){
				COUT("Failed to get the path. (errno=%d (%s)) path=%s",
						errno, strerror(errno), path.c_str());
				closedir(dp);
				return -1;
			}
		}
	} while (entry != NULL);
	closedir(dp);
#endif
	return 0;
}

std::string Path::Combine(const std::string& path1, const std::string& path2){
	if(path1.size() == 0){
		return path2;
	}
	if(path2.size() == 0){
		return path1;
	}
	if(path1.at(path1.size()-1) != '/'){
		return (path1 + "/" + path2);
	}
	else{
		return (path1 + path2);
	}
}

std::string Path::GetFileName(const std::string& path) {
	std::vector<std::string> dirs;
	StringUtil::Split(path,"/",dirs);
	if(dirs.size() == 0){
		return path;
	}
	return dirs[dirs.size()-1];
}

std::string Path::GetFileNameWithoutExtension(const std::string& path) {
	std::string fileName = GetFileName(path);
	std::vector<std::string> parts;
	StringUtil::Split(fileName,".",parts);
	if(parts.size() == 0){
		return fileName;
	}
	return parts[0];
}

std::string Path::GetExtension(const std::string& path) {
	std::string fileName = GetFileName(path);
	std::vector<std::string> parts;
	StringUtil::Split(fileName,".",parts);
	if(parts.size() <= 1){
		return "";
	}
	return parts[parts.size() - 1];
}

std::string Path::GetCurrentDir() {
	char path[512] = {0};
	// カレントディレクトリ取得
#if defined(WIN32) || defined(WIN64)
	_getcwd(path, 512);
#else
	getcwd(path, 512);
#endif
	return std::string(path);
}

bool Path::IsEmptyDir(const std::string& path) {
	std::vector<std::string> fileList;
	GetFileList(path, fileList);
	return fileList.empty();
}

SysTime Path::GetTimestamp(const std::string& path) {
	SysTime timestamp;
#ifndef WIN32
	struct stat stat_buf;
	if(stat(path.c_str(), &stat_buf) != 0){
		COUT("Failed to get time-stamp of the file. (errno=%d (%s)) path=%s",
				errno, strerror(errno), path.c_str());
		return timestamp;
	}
	timestamp.Set(stat_buf.st_mtim.tv_sec);
	timestamp.usec = stat_buf.st_mtim.tv_nsec / 1000;
	timestamp.msec = timestamp.usec / 1000;
#endif
	return timestamp;
}

int Path::GetFileSize(const std::string& path) {
	if(!IsExist(path)){
		return 0;
	}
    std::ifstream ifs(path.c_str(), std::ios::binary);
    ifs.seekg(0, std::ios::end);
    int eofpos = (int)ifs.tellg();
    ifs.clear();
    ifs.seekg(0, std::ios::beg);
    int begpos = (int)ifs.tellg();
    int fileSize = eofpos - begpos;
    ifs.close();
    return fileSize;
}

void Path::CashClear(const std::string& path)
{
#if defined(WIN32) || defined(WIN64)
#else
	int fd = open(path.c_str(), O_RDONLY);
	if(fd != -1){
		posix_fadvise(fd, 0, 0, POSIX_FADV_DONTNEED);
		close(fd);
	}
#endif
}
