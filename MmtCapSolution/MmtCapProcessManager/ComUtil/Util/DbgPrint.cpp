/*
 * DbgPrint.cpp
 *
 *  Created on: 2017/07/06
 *      Author: attractor
 */

#include <cstdio>
#include <stdio.h>
#include <stdarg.h>
#include "DbgPrint.h"

DbgPrint::DbgPrint() {
}

DbgPrint::~DbgPrint() {
}

void DbgPrint::Cout(const std::string fmt, ...) {
	char buf[4096] = {0}; //4KBまで
	va_list va;
	va_start(va, fmt);
	std::vsprintf(buf,fmt.c_str(),va);
	va_end(va);
	printf("%s\n",buf);
}
