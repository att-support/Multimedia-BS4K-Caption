/*
 * DbgPrint.h
 *
 *  Created on: 2017/07/06
 *      Author: attractor
 */

#ifndef UTIL_DBGPRINT_H_
#define UTIL_DBGPRINT_H_

#include <string>

#define COUT(fmt,...) DbgPrint::Cout(fmt, ## __VA_ARGS__)

class DbgPrint {
public:
	DbgPrint();
	virtual ~DbgPrint();
	static void Cout(const std::string fmt, ...);
};

#endif /* UTIL_DBGPRINT_H_ */
