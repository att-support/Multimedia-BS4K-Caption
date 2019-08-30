/*
 * SysClock.cpp
 *
 *  Created on: 2017/07/04
 *      Author: attractor
 */

#include <string.h>
#include <errno.h>
#include <time.h>
#include "SysClock.h"

SysClock::SysClock() {
}

SysClock::~SysClock() {
}

#ifndef WIN32
SYSCLK SysClock::GetClock() {
	struct timespec ts;
	if(clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
		return 0;
	}
	return ConvToSysClock(&ts);
}
#endif

SYSCLK SysClock::ConvToSysClockFromSec(unsigned long second) {
	return (SYSCLK)((SYSCLK)second * 1000000000);
}

SYSCLK SysClock::ConvToSysClockFromMSec(unsigned long msec) {
	return (SYSCLK)((SYSCLK)msec * 1000000);
}

unsigned long SysClock::ConvMSec(SYSCLK sysClock) {
	SYSCLK msec = (SYSCLK)(sysClock / 1000000);
	if(msec > 0xFFFFFFFF){
		return 0;
	}
	return (unsigned long)msec;
}

unsigned long SysClock::ConvUSec(SYSCLK sysClock) {
	SYSCLK usec = (SYSCLK)(sysClock / 1000);
	if(usec > 0xFFFFFFFF){
		return 0;
	}
	return (unsigned long)usec;
}

SYSCLK SysClock::ConvMSecToSysClock(unsigned long msec) {
	return (SYSCLK)(((SYSCLK)msec) * 1000000);
}

SYSCLK SysClock::ConvUSecToSysClock(unsigned long usec) {
	return (SYSCLK)(((SYSCLK)usec) * 1000);
}

SYSCLK SysClock::ConvToSysClock(struct timespec* ts) {
	if(ts == NULL) return 0;

	return (SYSCLK)(((SYSCLK)(ts->tv_sec)) * 1000000000) + (SYSCLK)ts->tv_nsec;
}
