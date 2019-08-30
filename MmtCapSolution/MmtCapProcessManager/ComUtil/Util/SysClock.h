/*
 * SysClock.h
 *
 *  Created on: 2017/07/04
 *      Author: attractor
 */

#ifndef UTIL_SYSCLOCK_H_
#define UTIL_SYSCLOCK_H_

#include <sys/types.h>
#include <time.h>

typedef unsigned long long SYSCLK;

class SysClock {
public:
	SysClock();
	virtual ~SysClock();

#ifndef WIN32
	static SYSCLK GetClock();
#endif
	static SYSCLK ConvToSysClockFromSec(unsigned long second);
	static SYSCLK ConvToSysClockFromMSec(unsigned long msec);
	static unsigned long ConvMSec(SYSCLK sysClock);
	static unsigned long ConvUSec(SYSCLK sysClock);
	static SYSCLK ConvMSecToSysClock(unsigned long msec);
	static SYSCLK ConvUSecToSysClock(unsigned long usec);

private:
	static SYSCLK ConvToSysClock(struct timespec* ts);
};

#endif /* UTIL_SYSCLOCK_H_ */
