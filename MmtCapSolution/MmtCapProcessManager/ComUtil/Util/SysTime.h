/*
 * SysTime.h
 *
 *  Created on: 2017/07/04
 *      Author: attractor
 */

#ifndef UTIL_SYSTIME_H_
#define UTIL_SYSTIME_H_

#if defined(WIN32) || defined(WIN64)
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif

#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <string>
class SysTime {
public:
	int year;
	int month;
	int day;
	int hour;
	int minutes;
	int second;
	int msec;
	int usec;

	SysTime();											//コンストラクタ（デフォルト）
	SysTime(const SysTime& obj);					//コピーコンストラクタ
	SysTime(int year_,int month_,int day_,int hour_,int minutes_,int second_,int msec_);	//コンストラクタ（年月日時分秒ミリ秒指定）
	SysTime(const std::string& yyyymmddhhmmss);		//コンストラクタ（yyyymmddhhmmss形式年月日時分秒文字列指定）
	SysTime(const time_t& tm);						//コンストラクタ（time_t型）
	virtual ~SysTime();									//デストラクタ

	//==比較演算子のオーバーライド
	bool operator==(const SysTime& obj){
		if(m_isMin){
			return obj.m_isMin ? true : false;
		}
		if(m_isMax){
			return obj.m_isMax ? true : false;
		}
		if(obj.m_isMin){
			return m_isMin ? true : false;
		}
		if(obj.m_isMax){
			return m_isMax ? true : false;
		}
		return ((year == obj.year) &&
				(month == obj.month) &&
				(day == obj.day) &&
				(hour == obj.hour) &&
				(minutes == obj.minutes) &&
				(second == obj.second) &&
				(msec == obj.msec));
	}
	//!=比較演算子のオーバーライド
	bool operator!=(const SysTime& obj){
		return !(*this == obj);
	}
	//>比較演算子のオーバーライド
	bool operator>(SysTime& obj){
		if(m_isMin){
			return obj.m_isMin ? false : false;
		}
		if(m_isMax){
			return obj.m_isMax ? false : true;
		}
		if(obj.m_isMin){
			return m_isMin ? false : true;
		}
		if(obj.m_isMax){
			return m_isMax ? false : false;
		}
		if(GetTimeT() == obj.GetTimeT()){
			return (msec > obj.msec);
		} else {
			return (GetTimeT() > obj.GetTimeT());
		}
	}
	//>=比較演算子のオーバーライド
	bool operator>=(SysTime& obj){
		if(m_isMin){
			return obj.m_isMin ? true : false;
		}
		if(m_isMax){
			return obj.m_isMax ? true : true;
		}
		if(obj.m_isMin){
			return m_isMin ? true : true;
		}
		if(obj.m_isMax){
			return m_isMax ? true : false;
		}
		if(GetTimeT() == obj.GetTimeT()){
			return (msec >= obj.msec);
		} else {
			return (GetTimeT() >= obj.GetTimeT());
		}
	}
	//<比較演算子のオーバーライド
	bool operator<(SysTime& obj){
		if(m_isMin){
			return obj.m_isMin ? false : true;
		}
		if(m_isMax){
			return obj.m_isMax ? false : false;
		}
		if(obj.m_isMin){
			return m_isMin ? false : false;
		}
		if(obj.m_isMax){
			return m_isMax ? false : true;
		}
		if(GetTimeT() == obj.GetTimeT()){
			return (msec < obj.msec);
		} else {
			return (GetTimeT() < obj.GetTimeT());
		}
	}
	//<=比較演算子のオーバーライド
	bool operator<=(SysTime& obj){
		if(m_isMin){
			return obj.m_isMin ? true : true;
		}
		if(m_isMax){
			return obj.m_isMax ? true : false;
		}
		if(obj.m_isMin){
			return m_isMin ? true : false;
		}
		if(obj.m_isMax){
			return m_isMax ? true : true;
		}
		if(GetTimeT() == obj.GetTimeT()){
			return (msec <= obj.msec);
		} else {
			return (GetTimeT() <= obj.GetTimeT());
		}
	}
	void SetMin(){
		Clear();
		m_isMin = true;
	}
	void SetMax(){
		Clear();
		m_isMax = true;
	}
	bool IsMin() const {
		return m_isMin;
	}
	bool IsMax() const {
		return m_isMax;
	}

	bool IsEmpty();												//未初期化検査（TRUE:未初期化）
	void Clear();													//日時クリア（未初期化の状態に設定）
	void Set(int year_,int month_,int day_,int hour_,int minutes_,int second_,int msec_);	//日時設定（年月日時分秒ミリ秒）
	void Set(const std::string& yyyymmddhhmmss);				//日時設定（yyyymmddhhmmss形式年月日時分秒文字列）
	void Set(const time_t& tm);								//日時設定（time_t型）
	void SetDateTimeStr(const std::string& dateTimeStr);			//日時設定（日時文字列 yyyy/mm/dd hh:mm:ss.fff）
	void SetDateTimeStrIso8601(const std::string& dateTimeStr);			//日時設定（ISO8601形式日時文字列 yyyy-mm-dd hh:mm:ss.fff）
	void SetYYYYMMDD(const std::string& dateStr);				//日付設定（yyyymmdd形式日付文字列）
	void SetDateStr(const std::string& dateStr);				//日付設定（日付文字列 yyyy/mm/dd）
	void SetDateStrIso8601(const std::string& dateStr);		//日付設定（ISO8601形式日付文字列 yyyy-mm-dd）
	void SetHHMMSS(const std::string& timeStr);				//時刻設定（hhmmss形式時刻文字列）
	void SetTimeStr(const std::string& timeStr);				//時刻設定（時刻文字列 hh:mm:ss）
	void SetTimeStrIso8601(const std::string& timeStr);		//時刻設定（ISO8601形式時刻文字列 hh:mm:ss）
	void SetTimeMsecStr(const std::string& hhmmssfff);		//ミリ秒までの時刻設定（hhmmssfff形式時刻文字列）
	std::string Now();											//現在時刻取得（自身のメンバに設定）
	std::string GetDateStr();									//日付文字列取得（yyyy/mm/dd形式）
	std::string GetDateStrIso8601();							//日付文字列取得（ISO8601形式 yyyy-mm-dd）
	std::string GetYYYYMMDD();									//日付文字列取得（yyyymmdd形式）
	unsigned long GetDateBcd();								//日付文字列取得（yyyymmdd形式BCD値）
	unsigned long GetHHMMBcd();								//時分文字列取得（hhmm形式BCD値）
	std::string GetHHMMSS();									//時刻文字列取得（hhmmss形式）
	std::string GetTimeStr();									//時刻文字列取得（hh:mm:ss形式）
	std::string GetTimeStrIso8601();							//時刻文字列取得（ISO8601形式 hh:mm:ss）
	std::string GetHHMMSSFFF();									//ミリ秒までの時刻文字列取得（hhmmssfff形式）
	std::string GetYYYYMMDDHHMMSS();							//日時文字列取得（yyyymmddhhmmss形式）
	std::string GetYYYYMMDDHHMMSSFFF();							//日時文字列取得（yyyymmddhhmmss形式）
	std::string GetDateTimeStr();								//日時文字列取得（yyyy/mm/dd hh:mm:ss.fff形式）
	std::string GetDateTimeStrIso8601();						//日時文字列取得（ISO8601形式 yyyy-mm-dd hh:mm:ss.fff形式）
	time_t GetTimeT();											//時刻取得（time_t型）
	void GetTimeSpec(struct timespec* time);					//時刻取得（timespec型）
	int GetWeekDay();												//曜日取得（日曜日:0）
	void SetStructTm(const struct tm& dt);						//時刻設定（struct tm型）
	void AddDays(int days);										//日数加算
	void AddTime(int hour_, int minutes_, int second_);		//時間加算
	void AddSeconds(int second);								//秒加算
	void AddMSeconds(unsigned long msec);								//ミリ秒加算
	void SubDays(int days);										//日数減算
	void SubTime(int hour_, int minutes_, int second_);		//時間減算
	void SubSeconds(int seconds);								//秒減算

	static SysTime GetNow();								//現在時刻取得（SysTimeオブジェクト）
	static std::string GetNowTimestamp();						//現在時刻取得（yyyy/mm/dd hh:mm:ss.fff形式）
	static std::string GetNowYYYYMMDDHHMMSS();				//現在時刻取得（yyyymmddhhmmss形式）
	static double GetDiffSec(SysTime& time1, SysTime& time2);	//秒差算出（time1 - time2）
	static double GetDiffMsec(SysTime& time1, SysTime& time2);	//ミリ秒差算出（time1 - time2）
	static double GetDiffUsSec(SysTime& time1, SysTime& time2);	//マイクロ秒差算出（time1 - time2）

private:
#if defined(WIN32) || defined(WIN64)
	_timeb m_tv;
#else
	struct timeval m_tv;											//時刻データ（struct timeval型）
#endif
	bool m_isMin;
	bool m_isMax;

	void Reset();													//時刻初期化
};

#endif /* UTIL_SYSTIME_H_ */
