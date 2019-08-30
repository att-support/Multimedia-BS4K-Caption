/*
 * AppLogger.cpp
 *
 *  Created on: 2018/05/02
 *      Author: attractor
 */

#include "AppLogger.h"

Logger* AppLogger::logger = NULL;

AppLogger::AppLogger()
{
}

AppLogger::~AppLogger()
{
	if(logger != NULL){
		delete logger;
	}
}

void AppLogger::Init(const std::string& logDirPath)
{
	if(logger == NULL){
		logger = new Logger("_MmtLib.log", logDirPath);
		logger->SetOutputScreen(true);
	}
}

void AppLogger::Init(const std::string& logDirPath, const std::string& logBaseName)
{
#if 0
	std::string logBaseFilename = "_" + logBaseName + ".log";
	if (logger == NULL) {
		logger = new Logger(logBaseFilename, logDirPath);
		logger->SetOutputScreen(true);
	}
#else
	std::string logBaseFilename = logBaseName;
	if (logger == NULL) {
		logger = new Logger(logBaseFilename, logDirPath);
		logger->SetOutputScreen(true);
	}
#endif
}

void AppLogger::Destroy()
{
	if(logger != NULL){
		delete logger;
	}
}
