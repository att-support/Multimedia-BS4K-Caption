/*
 * SysTime.cpp
 *
 *  Created on: 2017/07/04
 *      Author: attractor
 */

#include <vector>
#include "StringUtil.h"
#include "Conv.h"
#include "SysTime.h"

SysTime::SysTime()
: year(0),month(0),day(0),hour(0),minutes(0),second(0),msec(0),usec(0)
{
	m_isMin = false;
	m_isMax = false;
}

SysTime::SysTime(const SysTime& obj){
	this->year = obj.year;
	this->month = obj.month;
	this->day = obj.day;
	this->hour = obj.hour;
	this->minutes = obj.minutes;
	this->second = obj.second;
	this->msec = obj.msec;
	this->usec = obj.usec;
	this->m_isMin = obj.m_isMin;
	this->m_isMax = obj.m_isMax;
}

SysTime::SysTime(int year_, int month_, int day_, int hour_, int minutes_,
		int second_, int msec_) {
	Set(year_,month_,day_,hour_,minutes_,second_,msec_);
}

SysTime::SysTime(const std::string& yyyymmddhhmmss) {
	Set(yyyymmddhhmmss);
}

SysTime::SysTime(const time_t& tm) {
	Set(tm);
}

SysTime::~SysTime() {
}

bool SysTime::IsEmpty() {
	return ((year==0)&&(month==0)&&(day==0)&&(hour==0)&&(minutes==0)&&(second==0)&& !m_isMin && !m_isMax);
}

void SysTime::Clear() {
	year=0;
	month=0;
	day=0;
	hour=0;
	minutes=0;
	second=0;
	msec = 0;
	usec = 0;
	m_isMin = false;
	m_isMax = false;
}

std::string SysTime::Now()
{
	struct tm* tmp;
	//現在時刻取得
#if defined(WIN32) || defined(WIN64)
	_ftime(&m_tv);
	//m_tv.timezone = 9;
	//tmp = gmtime(&m_tv.time);
	tmp = localtime(&m_tv.time);
	msec = m_tv.millitm;
	usec = m_tv.millitm * 1000;
#else
	struct tm tmpTm;
	gettimeofday(&m_tv, NULL);
	localtime_r(&m_tv.tv_sec, &tmpTm);
	tmp = &tmpTm;
	msec = m_tv.tv_usec / 1000;
	usec = m_tv.tv_usec;
#endif
	year = tmp->tm_year + 1900;
	month = tmp->tm_mon + 1;
	day = tmp->tm_mday;
	hour = tmp->tm_hour;
	minutes = tmp->tm_min;
	second = tmp->tm_sec;
	//minutes = msec;
	//second = usec;

	m_isMin = false;
	m_isMax = false;

	return StringUtil::Format("%04d/%02d/%02d %02d:%02d:%02d.%03d",
			year,month,day,hour,minutes,second,msec);
}

SysTime SysTime::GetNow() {
	SysTime tm;
	tm.Now();
	return tm;
}

std::string SysTime::GetNowTimestamp()
{
	SysTime tm;
	return tm.Now();
}

std::string SysTime::GetYYYYMMDD() {
	if(IsEmpty()){
		return std::string(8,' ');
	}
	else{
		return StringUtil::Format("%04d%02d%02d",year,month,day);
	}
}

unsigned long SysTime::GetDateBcd() {
	unsigned long bcdDate = 0;
	bcdDate = (Conv::ULToBcd4((unsigned long)year) << 16) & 0xFFFF0000;
	bcdDate |= (Conv::ByteToBcd2((unsigned char)month) << 8) & 0x0000FF00;
	bcdDate |= Conv::ByteToBcd2((unsigned char)day) & 0x000000FF;
	return bcdDate;
}

unsigned long SysTime::GetHHMMBcd() {
	unsigned long bcdHHMM = 0;
	bcdHHMM = (Conv::ByteToBcd2((unsigned char)hour) << 8) & 0x0000FF00;
	bcdHHMM |= Conv::ByteToBcd2((unsigned char)minutes) & 0x000000FF;
	return bcdHHMM;
}

std::string SysTime::GetHHMMSS() {
	if(IsEmpty()){
		return std::string(6,' ');
	}
	else{
		return StringUtil::Format("%02d%02d%02d",hour,minutes,second);
	}
}

std::string SysTime::GetYYYYMMDDHHMMSS() {
	if(IsEmpty()){
		return std::string(14,' ');
	}
	else{
		return StringUtil::Format("%04d%02d%02d%02d%02d%02d",
				year,month,day,hour,minutes,second);
	}
}

std::string SysTime::GetYYYYMMDDHHMMSSFFF() {
	if (IsEmpty()) {
		return std::string(17, ' ');
	}
	else {
		return StringUtil::Format("%04d%02d%02d%02d%02d%02d%03d",
			year, month, day, hour, minutes, second, msec);
	}
}

std::string SysTime::GetDateStr() {
	if(IsEmpty()){
		return "";
	}
	else{
		return StringUtil::Format("%04d/%02d/%02d",year,month,day);
	}
}

std::string SysTime::GetTimeStr() {
	if(IsEmpty()){
		return "";
	}
	else{
		return StringUtil::Format("%02d:%02d:%02d",hour,minutes,second);
	}
}

std::string SysTime::GetTimeStrIso8601() {
	if(IsEmpty()){
		return "00:00:00";
	}
	else{
		return StringUtil::Format("%02d:%02d:%02d",hour,minutes,second);
	}
}

std::string SysTime::GetDateStrIso8601() {
	if(IsEmpty()){
		return "1990-01-01";
	}
	else{
		return StringUtil::Format("%04d-%02d-%02d",year,month,day);
	}
}

std::string SysTime::GetDateTimeStr() {
	if(IsEmpty()){
		return "";
	}
	else{
		return StringUtil::Format("%04d/%02d/%02d %02d:%02d:%02d.%03d",
				year,month,day,hour,minutes,second,msec);
	}
}

std::string SysTime::GetDateTimeStrIso8601() {
	if(IsEmpty()){
		//return "1990-01-01 00:00:00.000";
		return "";
	}
	else{
		return StringUtil::Format("%04d-%02d-%02d %02d:%02d:%02d.%03d",
				year,month,day,hour,minutes,second,msec);
	}
}

time_t SysTime::GetTimeT() {
	struct tm tmp;
	tmp.tm_year = year - 1900;
	tmp.tm_mon = month - 1;
	tmp.tm_mday = day;
	tmp.tm_hour = hour;
	tmp.tm_min = minutes;
	tmp.tm_sec = second;
	return mktime(&tmp);
}

void SysTime::GetTimeSpec(struct timespec* time) {
	if(time == NULL) {
		return;
	}
	time->tv_sec = GetTimeT();
	time->tv_nsec = (long)msec * 1000000;
}

int SysTime::GetWeekDay() {
	struct tm tmp;
	tmp.tm_year = year - 1900;
	tmp.tm_mon = month - 1;
	tmp.tm_mday = day;
	tmp.tm_hour = hour;
	tmp.tm_min = minutes;
	tmp.tm_sec = second;
	mktime(&tmp);
	return tmp.tm_wday;
}

void SysTime::SetStructTm(const struct tm& dt) {
	year = dt.tm_year + 1900;
	month = dt.tm_mon + 1;
	day = dt.tm_mday;
	hour = dt.tm_hour;
	minutes = dt.tm_min;
	second = dt.tm_sec;
}

void SysTime::AddDays(int days) {
	this->day += days;
	Reset();
}

void SysTime::AddTime(int hour_, int minutes_, int second_) {
	this->hour += hour_;
	this->minutes += minutes_;
	this->second += second_;
	Reset();
}

void SysTime::AddSeconds(int seconds) {
	this->second += seconds;
}

void SysTime::AddMSeconds(unsigned long msec) {
	unsigned long tmp = (this->msec + msec);
	this->msec = (tmp % 1000);
	this->second += tmp / 1000;
	this->usec = this->msec * 1000;
	Reset();
}

void SysTime::SubDays(int days) {
	AddDays(-days);
}

void SysTime::SubTime(int hour_, int minutes_, int second_) {
	AddTime(-hour_,-minutes_,-second_);
}

void SysTime::SubSeconds(int seconds) {
	AddSeconds(-seconds);
}

std::string SysTime::GetNowYYYYMMDDHHMMSS()
{
	SysTime tm;
	tm.Now();
	char str[32] = {0};
	sprintf(str,"%04d%02d%02d%02d%02d%02d",
			tm.year,tm.month,tm.day,tm.hour,tm.minutes,tm.second);
	return std::string(str);
}

double SysTime::GetDiffSec(SysTime& time1, SysTime& time2) {
	return difftime(time1.GetTimeT(),time2.GetTimeT());
}

double SysTime::GetDiffMsec(SysTime& time1, SysTime& time2) {
	double diff = GetDiffSec(time1,time2);
	double diffMsec;
	if (time1.msec >= time2.msec) {
		diffMsec = double(double((time1.msec - time2.msec)) / (double)1000.0);
	}
	else {
		diff -= 1.0;
		diffMsec = double(double(((time1.msec + 1000) - time2.msec)) / (double)1000.0);
	}
	return diff + diffMsec;
}

double SysTime::GetDiffUsSec(SysTime& time1, SysTime& time2) {
	double diff = GetDiffSec(time1, time2);
	double diffUsec;
	if (time1.usec >= time2.usec) {
		diffUsec = double(double((time1.usec - time2.usec)) / (double)1000000.0);
	}
	else {
		diff -= 1.0;
		diffUsec = double(double(((time1.usec + 1000000) - time2.usec)) / (double)1000000.0);
	}
	return diff + diffUsec;
}


void SysTime::Set(int year_, int month_, int day_, int hour_, int minutes_,
		int second_, int msec_) {
	this->year = year_;
	this->month = month_;
	this->day = day_;
	this->hour = hour_;
	this->minutes = minutes_;
	this->second = second_;
	this->msec = msec_;
	this->m_isMin = false;
	this->m_isMax = false;
}

void SysTime::Set(const std::string& yyyymmddhhmmss) {
	if(yyyymmddhhmmss == std::string(14,' ')){
		Clear();
	}
	else{
		if(yyyymmddhhmmss.size() == 14){
			if(yyyymmddhhmmss == std::string(14,' ')){

			}
			else{
				std::string yyyymmdd = yyyymmddhhmmss.substr(0,8);
				std::string hhmmss = yyyymmddhhmmss.substr(8,6);
				SetYYYYMMDD(yyyymmdd);
				SetHHMMSS(hhmmss);
			}
		}
		msec = 0;
		usec = 0;
		m_isMin = false;
		m_isMax = false;
	}
}

void SysTime::SetYYYYMMDD(const std::string& dateStr) {
	if(dateStr == std::string(8,' ')){
		Clear();
	}
	else{
		if(dateStr.size() == 8){
			std::string yyyy = dateStr.substr(0,4);
			std::string mm = dateStr.substr(4,2);
			std::string dd = dateStr.substr(6,2);

			year = Conv::DecStrToInt(yyyy);
			month = Conv::DecStrToInt(mm);
			day = Conv::DecStrToInt(dd);
		}
		else{
			year = 0;
			month = 0;
			day = 0;
		}
	}
}

void SysTime::SetHHMMSS(const std::string& timeStr) {
	if(timeStr == std::string(6,' ')){
		Clear();
	}
	else{
		if(timeStr.size() == 6){
			std::string hh = timeStr.substr(0,2);
			std::string mm = timeStr.substr(2,2);
			std::string ss = timeStr.substr(4,2);

			hour = Conv::DecStrToInt(hh);
			minutes = Conv::DecStrToInt(mm);
			second = Conv::DecStrToInt(ss);
		}
		else{
			hour = 0;
			minutes = 0;
			second = 0;
		}
	}
}

void SysTime::Set(const time_t& tm) {
	if(tm == 0){
		Clear();
	}
	else{
		time_t tmp = tm;
		struct tm* dt = localtime(&tmp);
		if(dt == NULL){
			Clear();
			return;
		}
		SetStructTm(*dt);
		msec = 0;
		usec = 0;
		m_isMin = false;
		m_isMax = false;
	}
}

void SysTime::SetDateTimeStr(const std::string& dateTimeStr) {

	std::vector<std::string> dateTime;
	StringUtil::Split(dateTimeStr," ",dateTime);
	if(dateTime.size() < 2){
		Clear();
		return;
	}
	std::vector<std::string> date;
	StringUtil::Split(dateTime[0],"/",date);
	if(date.size() != 3){
		Clear();
		return;
	}
	year = Conv::DecStrToInt(date[0]);
	month = Conv::DecStrToInt(date[1]);
	day = Conv::DecStrToInt(date[2]);

	std::vector<std::string> tmEx;
	StringUtil::Split(dateTime[1],".",tmEx);
	if(tmEx.size() > 1){
		msec = Conv::DecStrToInt(tmEx[1]);
	}
	std::vector<std::string> tm;
	StringUtil::Split(tmEx[0],":",tm);
	if(tm.size() != 3){
		Clear();
		return;
	}
	hour = Conv::DecStrToInt(tm[0]);
	minutes = Conv::DecStrToInt(tm[1]);
	second = Conv::DecStrToInt(tm[2]);

}

void SysTime::SetDateTimeStrIso8601(const std::string& dateTimeStr) {

	std::vector<std::string> dateTime;
	StringUtil::Split(dateTimeStr," ",dateTime);
	if(dateTime.size() < 2){
		Clear();
		return;
	}
	std::vector<std::string> date;
	StringUtil::Split(dateTime[0],"-",date);
	if(date.size() != 3){
		Clear();
		return;
	}
	year = Conv::DecStrToInt(date[0]);
	month = Conv::DecStrToInt(date[1]);
	day = Conv::DecStrToInt(date[2]);

	std::vector<std::string> tmEx;
	StringUtil::Split(dateTime[1],".",tmEx);
	if(tmEx.size() > 1){
		msec = Conv::DecStrToInt(tmEx[1]);
	}
	std::vector<std::string> tm;
	StringUtil::Split(tmEx[0],":",tm);
	if(tm.size() != 3){
		Clear();
		return;
	}
	hour = Conv::DecStrToInt(tm[0]);
	minutes = Conv::DecStrToInt(tm[1]);
	second = Conv::DecStrToInt(tm[2]);

}

void SysTime::SetTimeMsecStr(const std::string& hhmmssfff) {
	if(hhmmssfff == std::string(9,' ')){
		Clear();
	}
	else{
		if(hhmmssfff.size() == 9){
			std::string hhmmss = hhmmssfff.substr(0,6);
			std::string msec_ = hhmmssfff.substr(6,3);
			SetHHMMSS(hhmmss);
			msec = Conv::DecStrToInt(msec_);
		}
		else{
			Clear();
		}
	}
}

std::string SysTime::GetHHMMSSFFF() {
	if(IsEmpty()){
		return std::string(9,' ');
	}
	else{
		return StringUtil::Format("%02d%02d%02d%03d",hour,minutes,second,msec);
	}
}

void SysTime::SetDateStr(const std::string& dateStr) {

	std::vector<std::string> date;
	StringUtil::Split(dateStr,"/",date);
	if(date.size() != 3){
		return;
	}
	year = Conv::DecStrToInt(date[0]);
	month = Conv::DecStrToInt(date[1]);
	day = Conv::DecStrToInt(date[2]);
}

void SysTime::SetDateStrIso8601(const std::string& dateStr) {

	std::vector<std::string> date;
	StringUtil::Split(dateStr,"-",date);
	if(date.size() != 3){
		return;
	}
	year = Conv::DecStrToInt(date[0]);
	month = Conv::DecStrToInt(date[1]);
	day = Conv::DecStrToInt(date[2]);
}

void SysTime::SetTimeStr(const std::string& timeStr) {
	std::vector<std::string> tmEx;
	StringUtil::Split(timeStr,".",tmEx);
	if(tmEx.size() > 1){
		msec = Conv::DecStrToInt(tmEx[1]);
	}
	std::vector<std::string> tm;
	StringUtil::Split(tmEx[0],":",tm);
	if(tm.size() != 3){
		return;
	}
	hour = Conv::DecStrToInt(tm[0]);
	minutes = Conv::DecStrToInt(tm[1]);
	second = Conv::DecStrToInt(tm[2]);
}

void SysTime::SetTimeStrIso8601(const std::string& timeStr) {
	SetTimeStr(timeStr);
}

void SysTime::Reset()
{
	time_t tmp = GetTimeT();
	struct tm* dt = localtime(&tmp);
	if(dt != NULL){
		SetStructTm(*dt);
	}
}
