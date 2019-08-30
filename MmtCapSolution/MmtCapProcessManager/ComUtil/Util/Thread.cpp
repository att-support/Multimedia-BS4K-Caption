/*
 * Thread.cpp
 *
 *  Created on: 2017/07/06
 *      Author: attractor
 */

#ifdef WIN32
#else
#include <unistd.h>
#include <sys/syscall.h>
#endif

#include <string>
#include <vector>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include "Thread.h"
#include "DbgPrint.h"

Thread::Thread()
{
	_thId = 0;
	_sysThreadId = 0;
	_pthreadId = 0;
	_thName = "";
	_ThreadRun = false;
	_ThreadStopped = true;
	_thStatus = TH_STAT_STOPPED;
	_privateId = 0;
	_threadLoopIntervalMsec = 10;
	_inhiSig = true;
}

Thread::Thread(int id, const std::string& name)
{
	_thId = id;
	_sysThreadId = 0;
	_pthreadId = 0;
	_thName = name;
	_ThreadRun = false;
	_ThreadStopped = true;
	_thStatus = TH_STAT_STOPPED;
	_privateId = 0;
	_threadLoopIntervalMsec = 10;
	_inhiSig = true;
}

Thread::~Thread() {
	//スレッド起動中の場合はスレッド終了
	if(IsRunning()){
		_StopThread(3); //3秒待ち
	}
}

int Thread::StartThread()
{
	if(IsRunning()){
		return 0;
	}
	_thStatus = TH_STAT_BOOTING;
	int ret = 0;

#ifdef WIN32
#else
	// スレッド属性初期化
	ret = pthread_attr_init(&_pthreadAttr);
	if(ret != 0)
	{
		COUT("%s 初期化失敗(pthread_attr_init()=%d errno=%d (%s))",
				_thName.c_str(), ret, errno, strerror(errno));
		_thStatus = TH_STAT_STOPPED;
		return -1;
	}
	// スレッド切り離し
	ret = pthread_attr_setdetachstate(&_pthreadAttr, PTHREAD_CREATE_DETACHED);
	if(ret != 0){
		COUT("%s 初期化失敗(pthread_attr_setdetachstate()=%d errno=%d (%s))",
				_thName.c_str(), ret, errno, strerror(errno));
		_thStatus = TH_STAT_STOPPED;
		return -1;
	}
#endif

	_ThreadStopped = false;
	_ThreadRun = true;
	// スレッド生成
	ret = 0;
#ifdef WIN32
	_pthreadId = (HANDLE)_beginthreadex(NULL, 0, Thread::ThreadMain, (void*)this, 0, &_sysThreadId);
	if (_pthreadId == (HANDLE)-1L) {
		ret = -1;
	}

#else
	ret = 	pthread_create(&_pthreadId, &_pthreadAttr, Thread::ThreadMain, (void*)this);
#endif
	if(ret != 0) {
		COUT("%s 初期化失敗(pthread_create()=%d errno=%d (%s))",
				_thName.c_str(), ret, errno, strerror(errno));
		_thStatus = TH_STAT_STOPPED;
		return -1;
	}

	return 0;
}

int Thread::StopThread()
{
	return StopThread(1);
}

int Thread::StopThread(int timeOutSec)
{
	if(_pthreadId == 0){
		return 0;
	}
	//ブロッキング解除のためシグナル発行
#ifndef WIN32
	if(!_inhiSig){
		pthread_kill(_pthreadId, SIGUSR1);
	}
#endif
	return _StopThread(timeOutSec);
}

int Thread::_StopThread(int timeOutSec)
{
	COUT("%s 停止処理開始", _thName.c_str());
	if(!IsRunning()){
		COUT("%s 停止中", _thName.c_str());
		return 0;
	}
	_thStatus = TH_STAT_TERMINATING;
	// スレッド停止(指示)
	_ThreadRun = false;
	//スレッド終了待ち
	for(int i = 0; i < (timeOutSec*100); i++){
		if(_ThreadStopped){
			break;
		}
		Sleep(10);
	}
	if(!_ThreadStopped){
		COUT("%s 停止失敗(タイムアウト(%d秒))", _thName.c_str(), timeOutSec);
		//pthread_cancel()はC++例外を発生させrethrowしないとabortを引き起こすため廃止
		//（またメッセージキュー待ちやソケットの受信待ちなどのブロッキング状態では効果がないため）
#ifdef WIN32
		::CloseHandle(_pthreadId);
#endif
		return -1;
	}
#ifdef WIN32
	::WaitForSingleObject(_pthreadId, INFINITE);
	::CloseHandle(_pthreadId);
#endif

	COUT("%s 停止処理終了", _thName.c_str());
	return 0;
}
bool Thread::IsRunning()
{
	return _ThreadRun;
}

#ifdef WIN32
unsigned Thread::ThreadMain(void* threadBaseInstance)
#else
void* Thread::ThreadMain(void* threadBaseInstance)
#endif
{
	Thread* th = (Thread*)threadBaseInstance;
	//スレッドID取得
#ifndef WIN32
	th->_sysThreadId = (unsigned int)syscall(SYS_gettid);
#endif
	th->_thStatus = TH_STAT_BOOTING;
	COUT("[0x%06X] %s start", th->_thId, th->_thName.c_str());
	//スレッド開始処理
	if( th->ThreadInitProc() != 0 ){
		//ThreadMng::DelThread(th->_threadId);
		COUT("[0x%06X] %s start error", th->_thId, th->_thName.c_str());
		//アラーム出力は各スレッドで必要に応じて実装
		DelThread(th);
#ifdef WIN32
		_endthread();
		return 0;
#else
		return NULL;
#endif
	}
	th->_ThreadStopped = false;
	th->_ThreadRun = true;
	th->_thStatus = TH_STAT_RUNNING;
	while(th->_ThreadRun){
		// スレッド処理
		try{
			if( th->ThreadProc() != 0 ){
				th->_ThreadRun = false;
				break;
			}
		}
		catch(std::exception& ex){
			COUT("[0x%06X] %s EXCEPTION:%s", th->_thId, th->_thName.c_str(), ex.what());
			//ここでキャッチされる例外は意図しないもののため、スレッドは終了させない
		}
		th->Sleep(th->_threadLoopIntervalMsec);
	}

	std::string thName = th->_thName;
	//スレッド終了処理
	if( th->ThreadTermProc() != 0 ){
		COUT("[0x%06X] %s stop error", th->_thId, thName.c_str());
		//アラーム出力は各スレッドで必要に応じて実装
	}
	COUT("[0x%06X] %s end", th->_thId, thName.c_str());

	DelThread(th);

#ifdef WIN32
	_endthread();
	return 0;
#else
	return NULL;
#endif
}

void Thread::DelThread(Thread* th ) {
	//スレッド管理からの削除はデストラクタで行なうよう変更
	//（スレッドが終わりきる前にオブジェクトが破棄されることがあるため）
	th->_thStatus = TH_STAT_STOPPED;
	th->_ThreadStopped = true;
}

int Thread::ThreadProc()
{
	// 子プロセスでオーバーライド実装
	return 0;
}
void Thread::Sleep(int msec)
{
#ifdef WIN32
	::Sleep(msec);
#else
	usleep(msec*1000);
#endif
}

void Thread::WaitThreadEnd()
{
	//スレッド起動中なら終了待ち
	unsigned long msec = 0;
	if(_ThreadRun) {
		COUT("Wait for an end of %s thread.", _thName.c_str());
		//スレッド終了待ち
		while(1){
			if(_ThreadStopped){
				COUT("%s thread end.(%lu[msec])", _thName.c_str(), (msec*10));
				break;
			}
			Sleep(10);
			msec++;
		}
	}
}
int Thread::ThreadInitProc()
{
	// 子プロセスでオーバーライド実装
	return 0;
}
int Thread::ThreadTermProc()
{
	// 子プロセスでオーバーライド実装
	return 0;
}

void Thread::SetThName(const std::string& thName)
{
	_thName = thName;
}

std::string Thread::GetThName()
{
	return _thName;
}

std::string Thread::GetThDetail()
{
	return _thDetail;
}

int Thread::GetThId() const
{
	return _thId;
}

unsigned int Thread::GetSysThreadId() const
{
	return _sysThreadId;
}

THREAD_ID Thread::GetPThreadId() const
{
	return _pthreadId;
}

TH_STATUS Thread::GetThStatus() const
{
	return _thStatus;
}

int Thread::GetPrivateId() const
{
	return _privateId;
}

void Thread::SetPrivateId(int id)
{
	_privateId = id;
}
