//#######################################################################
//## ClockTime.c
//#######################################################################

//**************************************************
// include
//**************************************************
#include <stdio.h>
#include <stdlib.h>
//#include <sys/time.h>
#include <App/AppLogger.h>
#include <time.h>
#include <memory.h>
#include <string>
#include <Util/SysTime.h>
#include "ClockTime.h"


//**************************************************
// define
//**************************************************
//#define TIMESHIFT_TIMESTAMP -9*60*60 (sec)
#define TIMESHIFT_TIMESTAMP -32400


//==================================================
// ClockTime
//==================================================

#define NTP_EPOCH            (86400U * (365U * 70U + 17U))

ClockTime::ClockTime(int id)
{
	char nameBuf[256];
	sprintf(nameBuf,"送出時刻管理");
	m_name = std::string(nameBuf);
	m_nameForLog = "[" + m_name + "]";
	m_id = id;

	Init();
}

ClockTime::~ClockTime()
{
	Reset();
}

void ClockTime::Init()
{
	m_StartTime.Now();
	m_StartTimeStamp = GetSysTimeToTimeStamp(m_StartTime);
	m_LastTime = m_StartTime;
	m_TimeStamp=0;
	m_ThreadStop=0;
}

void ClockTime::Reset()
{
	ThreadStop();
	m_StartTime.Clear();
	m_StartTime.Now();
	m_StartTimeStamp = GetSysTimeToTimeStamp(m_StartTime);
	m_LastTime = m_StartTime;
	m_TimeStamp=0;
}

void ClockTime::Update()
{
#if 0

	SysTime tNowTime;
	unsigned long long i64TempStamp;

	tNowTime.Now();
	i64TempStamp = GetSysTimeToTimeStamp(tNowTime);

	Lock();
	m_LastTime = tNowTime;
	m_TimeStamp = i64TempStamp - m_StartTimeStamp;
	Unlock();

#else
	SysTime tNowTime;
	unsigned long long i64TempStamp;

	tNowTime.Now();
	i64TempStamp = GetNtpToTimeStamp();

//	Lock();
	m_LastTime = tNowTime;
	m_TimeStamp = i64TempStamp - m_StartTimeStamp;
//	Unlock();
#endif
}

unsigned long long ClockTime::GetTimeStamp()
{
	return m_TimeStamp;
}

unsigned long ClockTime::GetNtpShortForm(int offset_usec)
{
	unsigned short usSec  = 0;
	unsigned short usUsec = 0;
	unsigned long NtpShortForm  = 0;
	unsigned long long tTimeStamp = 0;

	tTimeStamp = m_TimeStamp + offset_usec;

	usSec  = (unsigned short)(tTimeStamp/1000000);
	usUsec = (unsigned short)(tTimeStamp%1000000);

	NtpShortForm = ((((unsigned long)usSec)<<16)&0xFFFF0000) | (((unsigned long)usUsec)&0x0000FFFF);

	return NtpShortForm;
}

unsigned long long ClockTime::GetNtpSLongForm(int offset_usec){

	unsigned long long ulSec  = 0;
	unsigned long long ulUsec = 0;
	unsigned long long tTimeStamp = 0;
	unsigned long long NtpLongForm  = 0;

	tTimeStamp = m_TimeStamp + offset_usec;

	ulSec  = tTimeStamp/1000000;
	ulUsec = tTimeStamp%1000000;

	NtpLongForm = (((ulSec)<<32)&0xFFFFFFFF00000000) | ((ulUsec)&0x00000000FFFFFFFF);

	return NtpLongForm;
}

unsigned long long ClockTime::GetNtpToTimeStamp()
{
	unsigned long long i64TempStamp;

#if 0
	unsigned long i64TempStampSeconds;
	unsigned long i64TempStampMSeconds;
//	unsigned long i32TempStampMsecCount;
//	unsigned long i32TempStampMsec;
//	i32TempStampMsecCount = GetNowNtpSecondsFraction32();
//	i32TempStampMsec = (unsigned long)((double)((double)i32TempStampMsecCount/(double)(65.536)));

	//秒数取得
	i64TempStampSeconds = GetNowNtpSeconds();
	//マイクロ秒取得
	i64TempStampMSeconds = GetNowNtpMicroSeconds();
	//タイムスタンプ化(1MHz)
	i64TempStamp = (unsigned long long)((unsigned long long)i64TempStampSeconds*(unsigned long long)1000000) + (unsigned long long)i64TempStampMSeconds;
#else
	i64TempStamp = GetNtpToTimeStampMicroSeconds();
#endif

	return i64TempStamp;
}

unsigned long long ClockTime::GetSysTimeToTimeStamp(SysTime tTime)
{
	int iHour,iMin,iSec,iMSec,iUSec;
	unsigned long long i64TempStamp,i64TempDay,i64TempTime;
	int iPassDays = 0;

	//2015/01/01/00:00:00からの差分を計算する
	//2015/01/01からの経過日
	iPassDays = GetPassDays(tTime);

	iHour = tTime.hour;
	iMin  = tTime.minutes;
	iSec  = tTime.second;
	iMSec = tTime.msec;
	//iUSec = iMSec*1000;
	iUSec = tTime.usec;

	i64TempDay = iPassDays*86400000000;

#if 0 //SK01
	i64TempTime  = iHour*60*60*1000000;
	i64TempTime += iMin*60*1000000;
	i64TempTime += iSec*1000000;
	i64TempTime += iUSec;
#else
	i64TempTime  = (unsigned long long)((double)(iHour*60*60)*(double)1000000);
	i64TempTime += (unsigned long long)((double)(iMin*60)*(double)1000000);
	i64TempTime += (unsigned long long)((double)iSec*(double)1000000);
	i64TempTime += (unsigned long long)iUSec;
#endif

	i64TempStamp = i64TempDay + i64TempTime;

	return i64TempStamp;
}

//廃止
#if 0
unsigned long long ClockTime::GetNowNtpSeconds(){
	unsigned long long i64TempTime;
//	struct tm time_base;
	//struct tm time_now;
//	time_base.tm_sec   = 0;
//	time_base.tm_min   = 0;
//	time_base.tm_hour  = 0;
//	time_base.tm_mday  = 1;
//	time_base.tm_mon   = 1-1;
//	time_base.tm_year  = 1900-1900;
//	time_base.tm_isdst = -1;
	time_t nowTime;
	time( &nowTime );

//	double d64TempTime=0;
//	time_t timeBase = mktime(&time_base);
//	d64TempTime = difftime(nowTime, timeBase);
//	i64TempTime = (unsigned long long)d64TempTime;
	//i64TempTime = i64TempTime + TIMESHIFT_TIMESTAMP;

	i64TempTime = nowTime + NTP_EPOCH;

	return i64TempTime;
}
#endif

//廃止
#if 0
unsigned long long ClockTime::GetNowNtpMicroSeconds(){

	timeval t_tv;
	unsigned long long i64TempTime;

//	unsigned long long i64TempTimeSec1;
//	unsigned long long i64TempTimeSec2;
//	time_t nowTime;
//	time( &nowTime );
//	//i64TempTimeSec1 = nowTime + NTP_EPOCH;
//	i64TempTimeSec1 = nowTime;

	gettimeofday(&t_tv, NULL);
//	//i64TempTimeSec2 = t_tv.tv_sec + NTP_EPOCH;
//	i64TempTimeSec2 = t_tv.tv_sec;
	i64TempTime = t_tv.tv_usec;

//	DBGLOG(m_nameForLog,"##### GetNowNtp()　i64TempTimeSec1=%llu i64TempTimeSec2=%llu",i64TempTimeSec1,i64TempTimeSec2);

	return i64TempTime;
}
#endif

//廃止
#if 0
unsigned long long ClockTime::GetNowNtpSecondsFraction64(){
	double dSecondsFraction=0;
	unsigned long long i64TempTime;
//	gettimeofday(&m_tv, NULL);
//	i64TempTime = m_tv.tv_usec;
	i64TempTime = GetNowNtpMicroSeconds();
	//-------------------------------
	//Fraction
	//-------------------------------
	//1[sec] =4294967296(count)
	//1[msec]=4294967.296(count)
	//1[usec]=4294.967296(count)
	//i64TempTime[usec]*4294.967296=COUNT
	dSecondsFraction = (double)i64TempTime*(double)(4294.967296); // 1/4294967296(sec)=1/
	return (unsigned long long)dSecondsFraction;
}
#endif

//廃止
#if 0
unsigned long ClockTime::GetNowNtpSecondsFraction32(){
	double dSecondsFraction=0;
	unsigned long long i64TempTime;
//	gettimeofday(&m_tv, NULL);
//	i64TempTime = m_tv.tv_usec;
	i64TempTime = GetNowNtpMicroSeconds();
	//-------------------------------
	//Fraction
	//-------------------------------
	//1[sec] =65536(count)
	//1[msec]=65.536(count)
	//1[usec]=0.065536(count)
	//i64TempTime[usec]*0.065536=COUNT
	//i64TempTime/1000[msec]*65.536=COUNT
	dSecondsFraction = (double)(i64TempTime/1000.0)*(double)(65.536); // 1/65536(sec)=1/
	return (unsigned long)dSecondsFraction;
}
#endif

//UPDATE
unsigned long long ClockTime::GetNowNtpTimeStampLong()
{
	unsigned long long i64TempStamp;
	unsigned long long i64TempFrac;

#if 0
	unsigned long long i64TempTime;
	unsigned long long i64TempFrac;
	i64TempTime = GetNowNtpSeconds();
	i64TempFrac = GetNowNtpSecondsFraction64();

	i64TempStamp = (i64TempTime<<32)&0xFFFFFFFF00000000;
	i64TempStamp+= i64TempFrac&0x00000000FFFFFFFF;
#else
	timeval t_tv;
	unsigned long long i64TempTimeSeconds;
	unsigned long long i64TempTimeDecimal;
	double dSecondsFraction=0;
	//unsigned long long i64TempTime;

	//時刻取得
	gettimeofday(&t_tv, NULL);
	//秒数取得
	i64TempTimeSeconds = t_tv.tv_sec + NTP_EPOCH;
	//マイクロ秒取得
	i64TempTimeDecimal = t_tv.tv_usec;

	//-------------------------------
	//Fraction
	//-------------------------------
	//1[sec] =4294967296(count)
	//1[msec]=4294967.296(count)
	//1[usec]=4294.967296(count)
	//i64TempTime[usec]*4294.967296=COUNT
	dSecondsFraction = (double)i64TempTimeDecimal*(double)(4294.967296); // 1/4294967296(sec)=1/
	i64TempFrac = (unsigned long)dSecondsFraction;

	i64TempStamp = (i64TempTimeSeconds<<32)&0xFFFFFFFF00000000;
	i64TempStamp+= i64TempFrac&0x00000000FFFFFFFF;
#endif

	return i64TempStamp;
}

//UPDATE
unsigned long ClockTime::GetNowNtpTimeStampShort()
{
	unsigned long i32TempStamp;
	unsigned long i32TempFrac;

#if 0
	unsigned long long i64TempSec;
	unsigned long i32TempStamp;

	i64TempSec = GetNowNtpSeconds();
	i32TempTime = GetNowNtpSecondsFraction32();

	i32TempStamp = ((unsigned long)i64TempSec<<16)&0xFFFF0000;
	i32TempStamp+= ((unsigned long)i32TempTime)&0x0000FFFF;
#else
	timeval t_tv;
	unsigned long long i64TempTimeSeconds;
	unsigned long long i64TempTimeDecimal;
	double dSecondsFraction=0;

	//時刻取得
	gettimeofday(&t_tv, NULL);
	//秒数取得
	i64TempTimeSeconds = t_tv.tv_sec + NTP_EPOCH;
	//マイクロ秒取得
	i64TempTimeDecimal = t_tv.tv_usec;

	//-------------------------------
	//Fraction
	//-------------------------------
	//1[sec] =65536(count)
	//1[msec]=65.536(count)
	//1[usec]=0.065536(count)
	//i64TempTime[usec]*0.065536=COUNT
	//i64TempTime/1000[msec]*65.536=COUNT
	dSecondsFraction = (double)(i64TempTimeDecimal/1000.0)*(double)(65.536); // 1/65536(sec)=1/
	i32TempFrac = (unsigned long)dSecondsFraction;

	i32TempStamp = ((unsigned long)i64TempTimeSeconds<<16)&0xFFFF0000;
	i32TempStamp+= ((unsigned long)i32TempFrac)&0x0000FFFF;
#endif

	return i32TempStamp;
}


//NEW
unsigned long long ClockTime::GetNtpToTimeStampMicroSeconds()
{
	unsigned long long i64TempStamp;
	//unsigned long long i64TempTime;

	timeval t_tv;
	unsigned long long i64TempTimeSeconds;
	unsigned long long i64TempTimeDecimal;
	//時刻取得
	gettimeofday(&t_tv, NULL);
	//秒数取得
	i64TempTimeSeconds = t_tv.tv_sec + NTP_EPOCH;
	//マイクロ秒取得
	i64TempTimeDecimal = t_tv.tv_usec;

	//タイムスタンプ化(1MHz)
	i64TempStamp = (unsigned long long)((unsigned long long)i64TempTimeSeconds*(unsigned long long)1000000) + (unsigned long long)i64TempTimeDecimal;

	return i64TempStamp;
}

int ClockTime::GetPassDays(SysTime tTime)
{
	//2015/01/01からの経過日

	int i;
	int BYear,BMonth,BDay,FYear,FMonth,FDay,leap;
	int monthday[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
	int day,day1,day2,day3;

	//基準年月日
	BYear = 2015;
	BMonth = 1;
	BDay = 1;

	//計算対象日
	FYear  = tTime.day;
	FMonth = tTime.month;
	FDay = tTime.day;

	//day1
	day1 = 0;
	if( (!(BYear%4)&&(BYear%100))||!(BYear%400)){ leap=1; }else{ leap=0; }
	for(i=1;i<=BMonth-1;i++){
        day1 += monthday[i];
        if(i==2){ day1 += leap; }
	}
	day1 += BDay;

	//day2
	day2 = 0;
	for(i=BYear;i<=FYear-1;i++){
		day2 += 365;
       if( (!(i%4)&&(i%100))||!(i%400) ){ leap=1; }else{ leap=0; }
       day2 += leap;
     }

	//day3
	day3 = 0;
	if( (!(FYear%4)&&(FYear%100))||!(FYear%400) ){ leap=1; }else{ leap=0; }

	for(i=1;i<=FMonth-1;i++){
		day3 += monthday[i];
		if(i==2){ day3 += leap; }
     }
    day3 += FDay;

    //経過日
    day = day2 + day3 - day1;

    return day;
}

int ClockTime::ThreadStop(){

	int iEndFlag=0;
	int iCount=0;
	m_ThreadStop = 1;

	do{
		if(m_ThreadStop==0){
			iEndFlag=0;
			break;
		}
		iCount++;
		if(iCount>1000){ //1000msec=1sec
			iEndFlag=1;
			break;
		}
		//usleep(1000); //1msec
		Sleep(1); //1msec
	}while(1);

	if(iEndFlag){
		return -1;
	}
	return 0;
}

int ClockTime::ThreadProc()
{
	int rv=0;
	int iIntervalMsec=10; //10msec

	rv =  0;

	//定期更新
	do{
		if(m_ThreadStop){
			m_ThreadStop = 0;
			break;
		}

		//-----------------------
		//タイムスタンプの更新
		//-----------------------
		Update();

		//100umsec=0.1,sec周期で実行
		Sleep(iIntervalMsec);

	}while(1);

	return 0; //スレッド終了
}

int ClockTime::ThreadInitProc() {
	APPLOG("[CLK],INF,%d,ClockTime,ThreadInitProc(),時刻スレッド開始(%s)", m_id, m_name.c_str());
	return 0;
}

int ClockTime::ThreadTermProc() {
	APPLOG("[CLK],INF,%d,ClockTime,ThreadInitProc(),時刻スレッド終了(%s)", m_id, m_name.c_str());
	return 0;
}

int ClockTime::gettimeofday(struct timeval * tp, struct timezone * tzp)
{
	// Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
	// This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
	// until 00:00:00 January 1, 1970 
	static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

	SYSTEMTIME  system_time;
	FILETIME    file_time;
	uint64_t    time;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	time = ((uint64_t)file_time.dwLowDateTime);
	time += ((uint64_t)file_time.dwHighDateTime) << 32;

	tp->tv_sec = (long)((time - EPOCH) / 10000000L);
	tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
	return 0;
}


