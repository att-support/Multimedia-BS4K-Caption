/*
 * Mutex.h
 *
 *  Created on: 2015/03/31
 *      Author: Attractor T.Unno
 */

#ifndef UTIL_MUTEX_H_
#define UTIL_MUTEX_H_

#include <string>
#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#else
#include <pthread.h>
#endif

#if defined(WIN32) || defined(WIN64)
typedef int MUTEX_ID;
#else
typedef pthread_mutex_t MUTEX_ID;
#endif

/*
 *
 */
class Mutex {
public:
	Mutex();
	Mutex(const std::string& name);
	virtual ~Mutex();

	void Lock();
	void Unlock();
	std::string GetMutexName() const;
	void SetMutexName(const std::string& name);
	MUTEX_ID GetMutexId() const;

protected:
#if defined(WIN32) || defined(WIN64)
	CRITICAL_SECTION _cs;
	void SetMutexId(int mutexId);
#endif

	MUTEX_ID _mutexId;
	std::string _name;

	void Init();

};

#endif /* UTIL_MUTEX_H_ */
