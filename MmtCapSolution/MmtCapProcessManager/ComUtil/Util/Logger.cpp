/*
 * Logger.cpp
 *
 *  Created on: 2015/03/23
 *      Author: attractor
 */
#include <cstdio>
#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <Util/Path.h>
#include "Logger.h"

/**
 * Constructor
 */
Logger::Logger(const std::string& logFileNameSuffix, const std::string& logDirPath) {
	_logFileNameSuffix = logFileNameSuffix;
	_logDirPath = logDirPath;
	_isOutputScreen = false;
}

/**
 * Destructor
 */
Logger::~Logger() {
}

/**
 * Output log file.
 */
void Logger::Logging(std::string fmt, ...)
{
	if (!Path::IsExist(_logDirPath)) return;

	SysTime nowTm;
	std::string timestamp = nowTm.Now();

	va_list va;
	va_start(va, fmt);
	std::string logMsg =
			timestamp + "," +
			fmt;

	char buf[8192];
	std::string logMsgWithLn = logMsg;
	std::vsnprintf(buf,sizeof(buf),logMsgWithLn.c_str(),va);
	va_end(va);
#if 0
	std::string fileName = nowTm.GetYYYYMMDD() + _logFileNameSuffix;
#else
	std::string fileName = _logFileNameSuffix + "_" + nowTm.GetYYYYMMDD() + ".log";
#endif
	std::string filePath = Path::Combine(_logDirPath, fileName);
	_mutex.Lock();
	std::ofstream ofs( filePath.c_str(), std::ios::out | std::ios::app );
	ofs << buf << std::endl;
	_mutex.Unlock();

	if(_isOutputScreen){
		std::cout << buf << std::endl;
	}
}

void Logger::DeleteLogFiles(SysTime& date)
{
	std::vector<std::string> fileList;
	if(Path::GetFileList(_logDirPath,fileList) != 0){
		return;
	}

	SysTime todayDate;
	todayDate.Now();
	unsigned long todayDateBcd = todayDate.GetDateBcd();

	for(std::vector<std::string>::iterator itr = fileList.begin();
			itr != fileList.end(); itr++)
	{
		//ファイル名長チェック
		if((*itr).size() != (8 + _procLogFileSuffix.size())){
			continue;
		}
		//サフィックスチェック
		std::string fileSuffix = (*itr).substr(8, _procLogFileSuffix.size());
		if(fileSuffix != _procLogFileSuffix){
			continue;
		}
		//日付取得
		std::string timestampStr = (*itr).substr(0,8);
		SysTime timestamp;
		try{
			timestamp.SetDateStr(timestampStr);
		}
		catch(...){
			continue;
		}
		//指定日を含む過去日のログを全て削除
		if(timestamp <= date){
			std::string filePath = Path::Combine(_logDirPath,(*itr));
			Path::RemoveFile(filePath);
		}
		//過去日となったログファイルのキャッシュをクリア
		if(timestamp.GetDateBcd() < todayDateBcd){
			std::string filePath = Path::Combine(_logDirPath,(*itr));
			Path::CashClear(filePath);
		}
	}
}

void Logger::SetOutputScreen(bool isOutputScreen)
{
	_isOutputScreen = isOutputScreen;
}
