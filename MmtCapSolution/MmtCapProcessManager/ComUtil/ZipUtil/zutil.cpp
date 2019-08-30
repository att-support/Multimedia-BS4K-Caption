//#include "pch.h"
#include "zutil.h"

unsigned short GetDate(int year, int month, int day)
{
	return (unsigned short)(((unsigned)(year - 1980) << 9) | ((unsigned)month << 5) | (unsigned)day);
}

unsigned short GetTime(int hour, int minute, int second)
{
	return (unsigned short)(((unsigned)hour << 11) | ((unsigned)minute << 5) | ((unsigned)second >> 1));
}

static unsigned int table[256];

void InitCRC32()
{
	unsigned int poly = 0xEDB88320, u, i, j;

	for (i = 0; i < 256; i++) {
		u = i;
		for (j = 0; j < 8; j++) {
			if (u & 0x1) {
				u = (u >> 1) ^ poly;
			}
			else {
				u >>= 1;
			}
		}

		table[i] = u;
	}
}

unsigned int GetCRC32(unsigned char *buffer, unsigned int bufferlen, unsigned int crc32_start) 
{
	unsigned int result = crc32_start;
	for (unsigned int i = 0; i < bufferlen; i++) {
		result = (result >> 8) ^ table[buffer[i] ^ (result & 0xFF)];
	}
	return ~result;
}

void CopyFileHeader(CentralDirHeader *centHeader, ZipHeader *zipHeader)
{
	centHeader->needver = zipHeader->needver;
	centHeader->option = zipHeader->option;
	centHeader->comptype = zipHeader->comptype;
	centHeader->filetime = zipHeader->filetime;
	centHeader->crc32 = zipHeader->crc32;
	centHeader->filedate = zipHeader->filedate;
	centHeader->compsize = zipHeader->compsize;
	centHeader->uncompsize = zipHeader->uncompsize;
	centHeader->fnamelen = zipHeader->fnamelen;
	centHeader->extralen = zipHeader->extralen;
	centHeader->filename = zipHeader->filename;
}

void CreateHeader(CentralDirHeader *centheader, ZipHeader *zipheader, char *filename, unsigned short fnamelen, unsigned int filesize, unsigned int compFileSize, unsigned short compType, unsigned int crc32)
{
	time_t now_t;
	struct tm now_tm;

	zipheader->signature = 0x04034B50;		// PK0304
	zipheader->needver = 10;

	now_t = time(NULL);
	localtime_s(&now_tm, &now_t);
	zipheader->filedate = GetDate(now_tm.tm_year + 1900, now_tm.tm_mon + 1, now_tm.tm_mday);
	zipheader->filetime = GetTime(now_tm.tm_hour, now_tm.tm_min, now_tm.tm_sec);

	zipheader->comptype = compType;

	zipheader->fnamelen = fnamelen;
	zipheader->filename = (unsigned char *)filename;

	zipheader->uncompsize = filesize;
	zipheader->compsize = compFileSize;

	zipheader->crc32 = crc32;

	centheader->signature = 0x02014B50;

	CopyFileHeader(centheader, zipheader);
}

unsigned int WriteZipHeader(unsigned char *target, unsigned int position, ZipHeader *zipheader)
{
	unsigned int ret = 0;
	unsigned char *crt = target + position;
	unsigned int uintsize = sizeof(unsigned int);
	unsigned short ushortsize = sizeof(unsigned short);

	memcpy((void *)crt, (void *)&zipheader->signature, sizeof(unsigned int));
	ret += uintsize;
	crt += uintsize;

	memcpy(crt, (void *)&zipheader->needver, sizeof(unsigned short));
	ret += ushortsize;
	crt += ushortsize;

	memcpy(crt, (void *)&zipheader->option, sizeof(unsigned short));
	ret += ushortsize;
	crt += ushortsize;

	memcpy(crt, (void *)&zipheader->comptype, sizeof(unsigned short));
	ret += ushortsize;
	crt += ushortsize;
	memcpy(crt, (void *)&zipheader->filetime, sizeof(unsigned short));
	ret += ushortsize;
	crt += ushortsize;
	memcpy(crt, (void *)&zipheader->filedate, sizeof(unsigned short));
	ret += ushortsize;
	crt += ushortsize;
	memcpy(crt, (void *)&zipheader->crc32, sizeof(unsigned int));
	ret += uintsize;
	crt += uintsize;
	memcpy(crt, (void *)&zipheader->compsize, sizeof(unsigned int));
	ret += uintsize;
	crt += uintsize;
	memcpy(crt, (void *)&zipheader->uncompsize, sizeof(unsigned int));
	ret += uintsize;
	crt += uintsize;
	memcpy(crt, (void *)&zipheader->fnamelen, sizeof(unsigned short));
	ret += ushortsize;
	crt += ushortsize;
	memcpy(crt, (void *)&zipheader->extralen, sizeof(unsigned short));
	ret += ushortsize;
	crt += ushortsize;
	memcpy(crt, (void *)zipheader->filename, zipheader->fnamelen);
	ret += zipheader->fnamelen;

	return ret;
}

unsigned int WriteCentralDirHeader(unsigned char *target, unsigned int position, CentralDirHeader *centheader)
{
	unsigned int ret = 0;
	unsigned char *crt = target + position;
	unsigned int uintsize = sizeof(unsigned int);
	unsigned short ushortsize = sizeof(unsigned short);

	memcpy(crt, (void *)&centheader->signature, sizeof(unsigned int));
	ret += uintsize;
	crt += uintsize;
	memcpy(crt, (void *)&centheader->madever, sizeof(unsigned short));
	ret += ushortsize;
	crt += ushortsize;
	memcpy(crt, (void *)&centheader->needver, sizeof(unsigned short));
	ret += ushortsize;
	crt += ushortsize;
	memcpy(crt, (void *)&centheader->option, sizeof(unsigned short));
	ret += ushortsize;
	crt += ushortsize;
	memcpy(crt, (void *)&centheader->comptype, sizeof(unsigned short));
	ret += ushortsize;
	crt += ushortsize;
	memcpy(crt, (void *)&centheader->filetime, sizeof(unsigned short));
	ret += ushortsize;
	crt += ushortsize;
	memcpy(crt, (void *)&centheader->filedate, sizeof(unsigned short));
	ret += ushortsize;
	crt += ushortsize;
	memcpy(crt, (void *)&centheader->crc32, sizeof(unsigned int));
	ret += uintsize;
	crt += uintsize;
	memcpy(crt, (void *)&centheader->compsize, sizeof(unsigned int));
	ret += uintsize;
	crt += uintsize;
	memcpy(crt, (void *)&centheader->uncompsize, sizeof(unsigned int));
	ret += uintsize;
	crt += uintsize;
	memcpy(crt, (void *)&centheader->fnamelen, sizeof(unsigned short));
	ret += ushortsize;
	crt += ushortsize;
	memcpy(crt, (void *)&centheader->extralen, sizeof(unsigned short));
	ret += ushortsize;
	crt += ushortsize;
	memcpy(crt, (void *)&centheader->commentlen, sizeof(unsigned short));
	ret += ushortsize;
	crt += ushortsize;
	memcpy(crt, (void *)&centheader->disknum, sizeof(unsigned short));
	ret += ushortsize;
	crt += ushortsize;
	memcpy(crt, (void *)&centheader->inattr, sizeof(unsigned short));
	ret += ushortsize;
	crt += ushortsize;
	memcpy(crt, (void *)&centheader->outattr, sizeof(unsigned int));
	ret += uintsize;
	crt += uintsize;
	memcpy(crt, (void *)&centheader->headerpos, sizeof(unsigned int));
	ret += uintsize;
	crt += uintsize;
	memcpy(crt, (void *)centheader->filename, centheader->fnamelen);
	ret += centheader->fnamelen;

	return ret;
}

unsigned int WriteEndCentDirHeader(unsigned char *target, unsigned int position, EndCentDirHeader *endcentheader)
{
	unsigned int ret = 0;
	unsigned char *crt = target + position;
	unsigned int uintsize = sizeof(unsigned int);
	unsigned short ushortsize = sizeof(unsigned short);

	memcpy(crt, (void *)&endcentheader->signature, sizeof(unsigned int));
	ret += uintsize;
	crt += uintsize;
	memcpy(crt, (void *)&endcentheader->disknum, sizeof(unsigned short));
	ret += ushortsize;
	crt += ushortsize;
	memcpy(crt, (void *)&endcentheader->startdisknum, sizeof(unsigned short));
	ret += ushortsize;
	crt += ushortsize;
	memcpy(crt, (void *)&endcentheader->diskdirentry, sizeof(unsigned short));
	ret += ushortsize;
	crt += ushortsize;
	memcpy(crt, (void *)&endcentheader->direntry, sizeof(unsigned short));
	ret += ushortsize;
	crt += ushortsize;
	memcpy(crt, (void *)&endcentheader->dirsize, sizeof(unsigned int));
	ret += uintsize;
	crt += uintsize;
	memcpy(crt, (void *)&endcentheader->startpos, sizeof(unsigned int));
	ret += uintsize;
	crt += uintsize;
	memcpy(crt, (void *)&endcentheader->commentlen, sizeof(unsigned short));
	ret += ushortsize;
	crt += ushortsize;

	return ret;
}

int ttmlComp(unsigned char *dst, unsigned int dstsize, unsigned char *src, unsigned int srcsize,
	unsigned char *opt1, unsigned int opt1size,
	unsigned char *opt2, unsigned int opt2size,
	unsigned char *opt3, unsigned int opt3size,
	unsigned char *opt4, unsigned int opt4size)
{
	//ZipHeader zipheader[5];
	ZipHeader *zipheader;
	CentralDirHeader archeader[5];
	zipheader = (ZipHeader *)malloc(sizeof(ZipHeader) * 5);
	memset(zipheader, 0, sizeof(ZipHeader) * 5);
	memset(archeader, 0, sizeof(CentralDirHeader) * 5);
	unsigned int count = 1;

	char *ttmlfilename = (char *)"page.ttml";
	CreateHeader(&archeader[0], &zipheader[0], ttmlfilename, 9, srcsize, srcsize, 0, GetCRC32(src, srcsize, 0xffffffff));

	if (opt1size > 0) {
		count++;
		char *opt1filename = (char *)"1";
		CreateHeader(&archeader[1], &zipheader[1], opt1filename, 1, opt1size, opt1size, 0, GetCRC32(opt1, opt1size, 0xffffffff));
	}

	if (opt2size > 0) {
		count++;
		char *opt2filename = (char *)"2";
		CreateHeader(&archeader[2], &zipheader[2], opt2filename, 1, opt2size, opt2size, 0, GetCRC32(opt2, opt2size, 0xffffffff));
	}

	if (opt3size > 0) {
		count++;
		char *opt3filename = (char *)"3";
		CreateHeader(&archeader[3], &zipheader[3], opt3filename, 1, opt3size, opt3size, 0, GetCRC32(opt3, opt3size, 0xffffffff));
	}

	if (opt4size > 0) {
		count++;
		char *opt4filename = (char *)"4";
		CreateHeader(&archeader[4], &zipheader[4], opt4filename, 1, opt4size, opt4size, 0, GetCRC32(opt4, opt4size, 0xffffffff));
	}

	unsigned int index = 0;
	unsigned int ret = 0;

	archeader[0].headerpos = index;
	ret = WriteZipHeader(dst, dstsize, &zipheader[0]);
	index += ret;
	unsigned char *zipDst = dst + index;
	memcpy(zipDst, src, srcsize);
	index += srcsize;

	if (opt1size > 0) {
		archeader[1].headerpos = index;
		ret = WriteZipHeader(dst, dstsize, &zipheader[1]);
		index += ret;
		zipDst = dst + index;
		memcpy(zipDst, opt1, opt1size);
		index += opt1size;
	}

	if (opt2size > 0) {
		archeader[2].headerpos = index;
		ret = WriteZipHeader(dst, dstsize, &zipheader[2]);
		index += ret;
		zipDst = dst + index;
		memcpy(zipDst, opt2, opt2size);
		index += opt2size;
	}

	if (opt3size > 0) {
		archeader[3].headerpos = index;
		ret = WriteZipHeader(dst, dstsize, &zipheader[3]);
		index += ret;
		zipDst = dst + index;
		memcpy(zipDst, opt3, opt3size);
		index += opt3size;
	}

	if (opt4size > 0) {
		archeader[4].headerpos = index;
		ret = WriteZipHeader(dst, dstsize, &zipheader[4]);
		index += ret;
		zipDst = dst + index;
		memcpy(zipDst, opt4, opt4size);
		index += opt4size;
	}

	unsigned int dirstartpos = index;

	archeader[0].madever = 10;
	ret = WriteCentralDirHeader(dst, index, &archeader[0]);
	index += ret;

	if (opt1size > 0) {
		archeader[1].madever = 10;
		ret = WriteCentralDirHeader(dst, index, &archeader[1]);
		index += ret;
	}

	if (opt2size > 0) {
		archeader[2].madever = 10;
		ret = WriteCentralDirHeader(dst, index, &archeader[2]);
		index += ret;
	}

	if (opt3size > 0) {
		archeader[3].madever = 10;
		ret = WriteCentralDirHeader(dst, index, &archeader[3]);
		index += ret;
	}

	if (opt4size > 0) {
		archeader[3].madever = 10;
		ret = WriteCentralDirHeader(dst, index, &archeader[4]);
		index += ret;
	}

	EndCentDirHeader endheader;
	memset(&endheader, 0, sizeof(EndCentDirHeader));
	endheader.signature = 0x06054B50;
	endheader.direntry = count;
	endheader.diskdirentry = endheader.direntry;
	endheader.startpos = dirstartpos;
	endheader.dirsize = index - dirstartpos;

	ret = WriteEndCentDirHeader(dst, index, &endheader);
	index += ret;

	return index;
}

int fileComp(unsigned char *dst, unsigned int dstsize, vector<FileInfo *> *fileList, int compLevel)
{
	ZipHeader *zipheader;
	CentralDirHeader *archeader;
	
	int fileCount = (int)fileList->size();

	if (fileCount < 1) {
		return fileCount;
	}

	unsigned int tempSize = 0;
	tempSize += fileCount * ZIP_HEADER_SIZE;
	tempSize += fileCount * CENT_HEADER_SIZE;
	tempSize += END_CENT_HEADER_SIZE;
	auto itFile = fileList->begin();
	while (itFile != fileList->end())
	{
		tempSize += (*itFile)->GetFileNameSize();
		tempSize += (*itFile)->GetFileSize();
		itFile++;
	}
	if (tempSize >= dstsize) {
		return -10;
	}

	zipheader = (ZipHeader *)malloc(sizeof(ZipHeader) * fileCount);
	archeader = (CentralDirHeader *)malloc(sizeof(CentralDirHeader) * fileCount);

	memset(zipheader, 0, sizeof(ZipHeader) * fileCount);
	memset(archeader, 0, sizeof(CentralDirHeader) * fileCount);

	int index = 0;

	itFile = fileList->begin();
	while (itFile != fileList->end())
	{
		if (compLevel > 0 && compLevel < 10)
		{
			unsigned long compSize = (*itFile)->GetFileSize() + 100;
			unsigned char *compData = new unsigned char[compSize];
			int resultComp = compress(compData, &compSize, (*itFile)->GetFileData(), (unsigned long)(*itFile)->GetFileSize());

			(*itFile)->SetCompFileInfo((unsigned int)compSize, compData);
		}
		else 
		{
			(*itFile)->SetCompFileInfo((*itFile)->GetFileSize(), (*itFile)->GetFileData());
		}


		CreateHeader(&archeader[index], &zipheader[index], (char *)(*itFile)->GetFileName(), (*itFile)->GetFileNameSize(), (*itFile)->GetFileSize(), (*itFile)->GetCompFileSize(), (unsigned short)compLevel, GetCRC32((*itFile)->GetFileData(), (*itFile)->GetFileSize(), 0xffffffff));
		itFile++;
		index++;
	}

	index = 0;
	unsigned int position = 0;
	unsigned int ret = 0;

	itFile = fileList->begin();
	while (itFile != fileList->end())
	{
		archeader[index].headerpos = position;
		ret = WriteZipHeader(dst, position, &zipheader[index]);
		position += ret;
		unsigned char *zipDst = dst + position;
		//memcpy(zipDst, (*itFile)->GetFileData(), (*itFile)->GetFileSize());
		memcpy(zipDst, (*itFile)->GetCompFileData(), (*itFile)->GetCompFileSize());
		//position += (*itFile)->GetFileSize();
		position += (*itFile)->GetCompFileSize();

		index++;
		itFile++;
	}

	unsigned int dirstartpos = position;

	index = 0;
	itFile = fileList->begin();
	while (itFile != fileList->end())
	{
		archeader[index].madever = 10;
		ret = WriteCentralDirHeader(dst, position, &archeader[index]);
		position += ret;

		index++;
		itFile++;
	}

	EndCentDirHeader endheader;
	memset(&endheader, 0, sizeof(EndCentDirHeader));
	endheader.signature = 0x06054B50;
	endheader.direntry = fileCount;
	endheader.diskdirentry = endheader.direntry;
	endheader.startpos = dirstartpos;
	endheader.dirsize = position - dirstartpos;

	ret = WriteEndCentDirHeader(dst, position, &endheader);
	position += ret;

	return position;
}

int fileCompInit()
{
	fileList = new vector<FileInfo *>();
	return 0;
}

int fileCompDist()
{
	fileList = nullptr;
	return 0;
}

int fileCompAdd(const char * fileName, unsigned int fileNameLen, unsigned char * fileData, unsigned int len)
{
	string fName = string(fileName, fileNameLen);
	FileInfo *temp = new FileInfo(fName, len);
	temp->SetFileData(fileData);
	fileList->push_back(temp);
	return 0;
}

int fileCompExec(unsigned char *dst, unsigned int dstsize, int compLevel)
{
	int ret = fileComp(dst, dstsize, fileList, compLevel);
	return ret;
}
