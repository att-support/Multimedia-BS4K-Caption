/*
 * Thread.h
 *
 *  Created on: 2017/07/06
 *      Author: attractor
 */

#ifndef UTIL_THREAD_H_
#define UTIL_THREAD_H_

#include <string>
#ifdef WIN32
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif /*_WINSOCKAPI_*/
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif

#ifdef WIN32
typedef HANDLE THREAD_ID;
#else
typedef pthread_t THREAD_ID;
#endif

 //状態
typedef enum {
	TH_STAT_STOPPED,				//未起動
	TH_STAT_BOOTING,				//初期化中
	TH_STAT_RUNNING,				//稼働中
	TH_STAT_TERMINATING,			//終了中
	TH_STAT_REQUEST_WATING			//処理要求待ち
}TH_STATUS;

class Thread {
public:
	Thread();
	Thread(int id, const std::string& name);
	virtual ~Thread();
	virtual int StartThread();						//スレッド開始
	virtual int StopThread();						//スレッド停止
	virtual int StopThread(int timeOutSec);			//スレッド停止
	void WaitThreadEnd();								//スレッド終了待ち
	void SetThName(const std::string& thName);			//スレッド名設定
	std::string GetThName();							//スレッド名取得
	std::string GetThDetail();							//スレッド詳細取得
	int GetThId() const;								//スレッドID取得
	unsigned int GetSysThreadId() const;			//スレッドID(syscall)取得
	THREAD_ID GetPThreadId() const;					//スレッドID(pthread)取得
	bool IsRunning();									//スレッド起動中検査（TRUE:スレッド起動中）
	TH_STATUS GetThStatus() const;					//スレッド状態取得
	int GetPrivateId() const;						//プライベートID取得
	void SetPrivateId(int id);						//プライベートID設定

#ifdef WIN32
	static unsigned __stdcall ThreadMain(void* threadBaseInstance);	//スレッドメインルーチン
#else
	static void* ThreadMain(void* threadBaseInstance);	//スレッドメインルーチン
#endif
	static void Sleep(int msec);							//スレッドスリープ

protected:
	int _thId;												//スレッドID
	unsigned int _sysThreadId;							//スレッドID(syscall)
	THREAD_ID _pthreadId;									//スレッドID(pthread)
#ifndef WIN32
	pthread_attr_t _pthreadAttr;							//スレッド属性(pthread)
#endif
	std::string _thName;									//スレッド名
	std::string _thDetail;									//スレッド詳細
	TH_STATUS _thStatus;									//スレッド状態
	int _threadLoopIntervalMsec;							//スレッドループ間隔(msec)
	int _privateId;											//プライベートID（各スレッドで自由に使用）
	bool _ThreadRun;									//スレッド起動中フラグ（TRUE:スレッド起動中）
	bool _inhiSig;


	virtual int ThreadProc();						//スレッドメインループ処理
	virtual int ThreadInitProc();					//スレッド起動時処理（スレッド起動直後に実行される。必要に応じて継承先で実装すること。）
	virtual int ThreadTermProc();					//スレッド終了時処理（スレッド終了直前に実行される。必要に応じて継承先で実装すること。）

private:
	bool _ThreadStopped;							//スレッド終了指示フラグ（TRUE:スレッド終了指示ON）

	int _StopThread(int timeOutSec);				//スレッド停止処理
	static void DelThread(Thread* th);
};

#endif /* UTIL_THREAD_H_ */
