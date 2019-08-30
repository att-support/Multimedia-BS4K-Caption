// ConsoleApplication1.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

//#include "pch.h"
#include <iostream>
#include "zutil.h"
#include <fstream>


int main()
{
    std::cout << "Hello World!\n"; 



	int result;
	const char *filename = "page.ttml";
	ifstream inputfile(filename, ios::in);
	unsigned long srcSize = (unsigned long)inputfile.seekg(0, ios::end).tellg();
	inputfile.seekg(0, ios::beg);
	unsigned char *src = (unsigned char *)malloc(sizeof(unsigned char) * srcSize);
	unsigned long dstSize = srcSize + 500;
	unsigned char *dst = (unsigned char *)malloc(sizeof(unsigned char) * dstSize);
	memset((void *)dst, 0, dstSize);

	inputfile.read((char *)src, srcSize);

	///////////////////////////////////
	// 初期化
	///////////////////////////////////
	fileCompInit();
	///////////////////////////////////
	// ZIPするファイルを追加
	///////////////////////////////////
	fileCompAdd(filename, 9, src, srcSize);

	///////////////////////////////////
	// ZIPのアーカイブ化
	///////////////////////////////////
	result = fileCompExec(dst, dstSize, 0);
	///////////////////////////////////
	// 後処理
	///////////////////////////////////
	fileCompDist();

	printf("output = %d !!", result);

	ofstream outputfile("send.zip", ios::out | ios::binary);
	outputfile.write((const char *)dst, result);
	outputfile.close();

	free(src);
	free(dst);
	inputfile.close();

	return 0;


}
