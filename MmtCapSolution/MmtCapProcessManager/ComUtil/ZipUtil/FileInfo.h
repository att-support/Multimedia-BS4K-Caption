#pragma once
#include <string>

using namespace std;

class FileInfo
{
public:
	FileInfo(string fileName, unsigned int fileSize);
	~FileInfo();

public:
	void SetFileData(unsigned char *fileData) { this->fileData = fileData; }
	const char *GetFileName() { return this->fileName.c_str(); }
	unsigned short GetFileNameSize() { return (unsigned short)this->fileName.size(); }
	unsigned int GetFileSize() { return this->fileSize; }
	unsigned char *GetFileData() { return this->fileData; }
	void SetCompFileInfo(unsigned int compFileSize, unsigned char *compFileData);
	unsigned int GetCompFileSize() { return this->compFileSize; }
	unsigned char *GetCompFileData() { return this->compFileData; }
private:
	unsigned int fileSize;
	string fileName;
	unsigned char *fileData;
	unsigned int compFileSize;
	unsigned char *compFileData;
};

