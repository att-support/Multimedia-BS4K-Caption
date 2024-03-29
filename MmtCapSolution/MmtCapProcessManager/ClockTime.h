//#######################################################################
// CloclTime.h
//#######################################################################
#ifndef TSSTIME_CLOCKTIME_H_
#define TSSTIME_CLOCKTIME_H_

//**************************************************
// include
//**************************************************
#include <string>
#include <Util/SysTime.h>
#include <Util/Thread.h>
//#include "MuxOutDefine.h"

//**************************************************
// define
//**************************************************

//
// 初期値=0
// 初期時刻(ミリ秒精度)
// オフセット値(外部PCR設定)
// 一時停止でタイムスタンプ保持＆再開
// →ただし初期時刻は再開時刻とする
//
// INITVALUE = IN(t)
// STARTTIME = ST(t)
// 現在時刻  = NT(t)
// TIMESTAMP = IN(t) + (NT(t)-ST(t))
// 
// 起動時 IN(t)=0 ST(t)セット
// 同期時 IN(t)=X ST(t)セット
// 停止時 タイムスタンプ更新停止=Y
// 再開時 IN(t)=Y ST(t)セット
//

// -----------------------------------------------
// ClockBase
// -----------------------------------------------
class ClockTime : public Thread
{
public:
	ClockTime(int id);
	virtual ~ClockTime();
	void Init();
	void Reset();
	void Update();
	int ThreadStop();

public:
	int m_id;
	timeval m_tv;
	SysTime m_StartTime;
	SysTime m_LastTime;
	unsigned long long m_StartTimeStamp;
	unsigned long long m_TimeStamp;
	//27MHz=27000000/sec
	//27MHz=27000/msec
	//27MHz=27/usec
	//XXMHz=         1/usec
	//XXMHz=      1000/msec
	//1MHz=    1000000/sec
	//1MHz=   60000000/min
	//1MHz= 3600000000/hour
	//1MHz=86400000000/day
	//64bit=0x7FFFFFFF=9223372036854775807
	//9223372036854775807/86400000000=106751991[day]=292471[year]

protected:
	int m_ThreadStop;
	virtual int ThreadProc();
	virtual int ThreadInitProc();
	virtual int ThreadTermProc();
	std::string m_name;
	std::string m_nameForLog;

private:
	unsigned long long GetSysTimeToTimeStamp(SysTime tTime);
	int GetPassDays(SysTime tTime);
	unsigned long long GetNtpToTimeStamp();

	//unsigned long long GetNowNtpSeconds();
	//unsigned long long GetNowNtpMicroSeconds();
	//unsigned long long GetNowNtpSecondsFraction64();
	//unsigned long GetNowNtpSecondsFraction32();

public:
	unsigned long long GetTimeStamp();
	unsigned long GetNtpShortForm(int offset_usec=0);
	unsigned long long GetNtpSLongForm(int offset_usec=0);

	unsigned long long GetNtpToTimeStampMicroSeconds();
	unsigned long long GetNowNtpTimeStampLong();
	unsigned long GetNowNtpTimeStampShort();

	int gettimeofday(struct timeval * tp, struct timezone * tzp);
};

#endif//  MMTMUXOUT_CLOCKTIME_H_

