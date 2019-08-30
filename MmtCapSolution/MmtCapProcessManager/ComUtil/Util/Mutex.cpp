/*
 * Mutex.cpp
 *
 *  Created on: 2018/05/01
 *      Author: Attractor T.Unno
 */

#if defined(WIN32) || defined(WIN64)
#else
#include <errno.h>
#include <string.h>
#endif
#include "Mutex.h"

Mutex::Mutex() {
	_name = "";
	Init();
}

Mutex::Mutex(const std::string& name) {
	_name = name;
	Init();
}

void Mutex::Init()
{
#if defined(WIN32) || defined(WIN64)
	InitializeCriticalSection(&_cs);
#else
	pthread_mutex_init(&_mutexId, NULL);
#endif
}

Mutex::~Mutex() {
#if defined(WIN32) || defined(WIN64)
	DeleteCriticalSection(&_cs);
#else
	int ret = pthread_mutex_destroy(&_mutexId);
	if( ret != 0 ){
		if(ret == EBUSY){
			Unlock();
			int ret_ = pthread_mutex_destroy(&_mutexId);
			if(ret_ != 0){
				printf("%s mutex destroy error. (pthread_mutex_destroy()=%d)",
						_name.c_str(), ret);
			}
		}
		else{
			printf("%s mutex destroy error. (pthread_mutex_destroy()=%d)",
					_name.c_str(), ret);
		}
	}
#endif
}
void Mutex::Lock()
{
#if defined(WIN32) || defined(WIN64)
	EnterCriticalSection(&_cs);
#else
	int ret = pthread_mutex_lock(&_mutexId);
	if( ret != 0 ){
		if(ret == EINVAL){
			printf("%s mutex lock error. (uninitialized) (pthread_mutex_lock()=%d)",
					_name.c_str(), ret);
			return;
		}
	}
#endif
}
void Mutex::Unlock()
{
#if defined(WIN32) || defined(WIN64)
	LeaveCriticalSection(&_cs);
#else
	int ret = pthread_mutex_unlock(&_mutexId);
	if( ret != 0 ){
		if(ret == EINVAL){
			printf("%s mutex unlock error. (uninitialized) (pthread_mutex_unlock())");
			return;
		}
	}
#endif
}

MUTEX_ID Mutex::GetMutexId() const
{
	return _mutexId;
}

std::string Mutex::GetMutexName() const
{
	return _name;
}

void Mutex::SetMutexName(const std::string& name){
	_name = name;
}

#if defined(WIN32) || defined(WIN64)
void Mutex::SetMutexId(int mutexId) {
	_mutexId = mutexId;
}
#endif

