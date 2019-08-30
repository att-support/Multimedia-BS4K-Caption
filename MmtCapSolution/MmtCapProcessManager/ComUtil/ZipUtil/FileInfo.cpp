//#include "pch.h"
#include "FileInfo.h"

FileInfo::FileInfo(string fileName, unsigned int fileSize)
{
	this->fileName = fileName;
	this->fileSize = fileSize;
}

FileInfo::~FileInfo()
{
}

void FileInfo::SetCompFileInfo(unsigned int compFileSize, unsigned char * compFileData)
{
	this->compFileSize = compFileSize;
	this->compFileData = compFileData;
}
