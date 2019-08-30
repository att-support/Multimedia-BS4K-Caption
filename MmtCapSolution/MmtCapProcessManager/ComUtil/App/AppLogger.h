/*
 * AppLogger.h
 *
 *  Created on: 2018/08/02
 *      Author: attractor
 */

#ifndef APPLOGGER_H_
#define APPLOGGER_H_

#include <Util/Logger.h>

#define APPLOG(fmt,...) if(AppLogger::logger != NULL) AppLogger::logger->Logging(fmt, ## __VA_ARGS__)

class AppLogger {
public:
	static void Init(const std::string& logDirPath);
	static void Init(const std::string& logDirPath, const std::string& logBaseName);
	static void Destroy();
	static Logger* logger;

private:
	AppLogger();
	virtual ~AppLogger();
};

#endif /* APPLOGGER_H_ */
