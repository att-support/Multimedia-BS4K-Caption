#include <iostream>
#include "zlib.h"
#include "zconf.h"
#include <time.h>
#include "FileInfo.h"
#include <vector>

#ifdef _DEBUG
#pragma comment(lib, "Modules/zlibd.lib")
#else
#pragma comment(lib, "Modules/zlib.lib")
#endif

using namespace std;

#define ZIP_HEADER_SIZE 30
#define CENT_HEADER_SIZE 46
#define TTML_FILE_HEADER_SIZE 39
#define OPT_FILE_HEADER_SIZE 31
#define TTML_FILE_CENT_HEADER_SIZE (46 + 9)
#define OPT_FILE_CENT_HEADER_SIZE (46 + 1)
#define END_CENT_HEADER_SIZE 22
#define TTML_COMP_HEADER_SIZE TTML_FILE_HEADER_SIZE + (OPT_FILE_HEADER_SIZE * 4) + TTML_FILE_CENT_HEADER_SIZE + (OPT_FILE_HEADER_SIZE * 4) + END_CENT_HEADER_SIZE

typedef struct _ZipHeader 
{
	unsigned int signature;
	unsigned short needver;
	unsigned short option;
	unsigned short comptype;
	unsigned short filetime;
	unsigned short filedate;
	unsigned int crc32;
	unsigned int compsize;
	unsigned int uncompsize;
	unsigned short fnamelen;
	unsigned short extralen;
	unsigned char *filename;
} ZipHeader;

typedef struct _CentralDirHeader 
{
	unsigned int signature;
	unsigned short madever;
	unsigned short needver;
	unsigned short option;
	unsigned short comptype;
	unsigned short filetime;
	unsigned short filedate;
	unsigned int crc32;
	unsigned int compsize;
	unsigned int uncompsize;
	unsigned short fnamelen;
	unsigned short extralen;
	unsigned short commentlen;
	unsigned short disknum;
	unsigned short inattr;
	unsigned int outattr;
	unsigned int headerpos;
	unsigned char *filename;
} CentralDirHeader;

typedef struct _EndCentDirHeader
{
	unsigned int signature;
	unsigned short disknum;
	unsigned short startdisknum;
	unsigned short diskdirentry;
	unsigned short direntry;
	unsigned int dirsize;
	unsigned int startpos;
	unsigned short commentlen;
} EndCentDirHeader;

unsigned short GetDate(int year, int month, int day);
unsigned short GetTime(int hour, int minute, int second);

//static unsigned int table[256];

static vector<FileInfo *> *fileList;

void InitCRC32();
unsigned int GetCRC32(unsigned char *buffer, unsigned int bufferlen, unsigned int crc32_start);

void CopyFileHeader(CentralDirHeader *centHeader, ZipHeader *zipHeader);
void CreateHeader(CentralDirHeader *centheader, ZipHeader *zipheader, char *filename, unsigned short fnamelen, unsigned int filesize, unsigned int compFileSize, unsigned short compType, unsigned int crc32);


unsigned int WriteZipHeader(unsigned char *target, unsigned int position, ZipHeader *zipheader);

unsigned int WriteCentralDirHeader(unsigned char *target, unsigned int position, CentralDirHeader *centheader);

unsigned int WriteEndCentDirHeader(unsigned char *target, unsigned int position, EndCentDirHeader *endcentheader);

int ttmlComp(unsigned char *dst, unsigned int distsize, unsigned char *src, unsigned int srcsize,
	unsigned char *opt1, unsigned int opt1size,
	unsigned char *opt2, unsigned int opt2size,
	unsigned char *opt3, unsigned int opt3size,
	unsigned char *opt4, unsigned int opt4size);

int fileComp(unsigned char *dst, unsigned int dstsize, vector<FileInfo *> *fileList, int compLevel);

int fileCompInit();

int fileCompDist();

int fileCompAdd(const char *fileName, unsigned int fileNameLen, unsigned char *fileData, unsigned int len);

int fileCompExec(unsigned char *dst, unsigned int dstsize, int compLevel);