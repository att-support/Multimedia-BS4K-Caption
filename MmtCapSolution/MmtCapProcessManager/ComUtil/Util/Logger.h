/*
 * Logger.h
 *
 *  Created on: 2018/05/02
 *      Author: Attractor
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <Util/Mutex.h>
#include <Util/SysTime.h>

class Logger {
public:
	Logger(const std::string& logFileNameSuffix, const std::string& logDirPath);
	virtual ~Logger();

	void Logging(std::string fmt, ...);
	void DeleteLogFiles(SysTime& date);
	void SetOutputScreen(bool isOutputScreen);

private:
	std::string _procLogFileSuffix;
	std::string _logFileNameSuffix;
	std::string _logDirPath;
	Mutex _mutex;
	bool _isOutputScreen;
};

#endif /* LOGGER_H_ */
