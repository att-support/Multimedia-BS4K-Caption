#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <direct.h>
#include <memory.h>

#include "windows.h"
#include "shellapi.h"
#pragma comment(lib ,"shell32.lib")

#include <Util/StringUtil.h>
#include <Util/Path.h>
#include <Util/DbgPrint.h>
#include <Util/Conv.h>
#include <Util/SysTime.h>
#include <App/AppLogger.h>

#include <ZipUtil/zutil.h>

#ifdef _DEBUG
#pragma comment(lib, "Modules/zlibd.lib")
#else
#pragma comment(lib, "Modules/zlib.lib")
#endif

#include "MmtCapProcessManager.h"

#define MMT_MEMORY_INTERFACE 1 //1:memory 0:file

#define MMT_INTERFACE_TYPE 0 //1:file 0:memory
#define MMT_INTERFACE_SAVE 0 //1:save 0:nosave
#define MMT_INTERFACE_SAVE_ZIP 1 //1:save 0:nosave

#define MMT_PROCESS_STATUS_STOP		0
#define MMT_PROCESS_STATUS_START	1

#define MMT_PROCESS_TYPE_CAPTION_MAXCOUNT	12
#define MMT_PROCESS_TYPE_MTMEDIA_MAXCOUNT	4

#define MMT_RESOLUTION_2K_SIZE 8294400   //1920*1080*4
#define MMT_RESOLUTION_4K_SIZE 33177600  //1920*1080*4*4
#define MMT_RESOLUTION_8K_SIZE 132710400 //1920*1080*4*4*4

//#define MMT_INITIAL_NAME	"MmtCapProcessManager.ini"
//#define MMT_INITIAL_PATH	"C:\\WORK\\app\\MmtCapProcessManager.ini"
#define MMT_LOG_PATH		"C:\\WORK\\app\\Log\\MmtCap"

//asset_type
#define MMT_TYPEPID_MPU_TTMLCAPTION				0x73747070 //stpp
#define MMT_TYPEPID_SEC_MPT_ESSAGE_MPT			0x6D6D7470 //mmtp
//#define MMT_TYPEPID_MPU_MULTIMEDIA_ITEM		0x61617070 //aapp
//#define MMT_TYPEPID_SEC_MULTIMEDIA_AIT		0x61616974 //aait
//#define MMT_TYPEPID_SEC_MULTIMEDIA_DAMDDM		0x6161646d //aadm
//#define MMT_TYPEPID_SEC_MULTIMEDIA_EMT		0x61656d74 //aemt
//#define MMT_TYPEPID_SEC_MULTIMEDIA_AMT		0x61616d74 //aamt

#define MMT_PROCESS_MTDATA_SAVE_PNG 0
#define MMT_DEBUG_LOG 0
#define MMT_MAN_INFO_LOG 0

#define MMT_CAPTION_DISPLAY_OLD 1
#define MMT_CAPTION_DISPLAY_NEW 0
#define MMT_TIMESTAMP_NOCHECK 1


MmtCapProcessManager::MmtCapProcessManager(std::string InitFile, std::string LogPath, int iLogLevel)
{
	int i = 0;

	m_id = 0;
	m_type = 0;
	m_number= 0;
	m_resolution = 0;
	m_streamid = 0;
	m_serviceid = 0;
	m_busy = 0;
	m_CreateFlag = 0;
	m_release_flag = 0;

	m_INI_FILENAME = InitFile;

	m_LogPath = LogPath;

	m_LogLevel = iLogLevel;

	if (m_LogLevel) {
		APPLOG("[MAN],INF,%d,MmtCapProcessManager,MmtCapProcessManager(),Start Instance ===============", m_id);
	}

	for (i = 0; i < MMT_PROCESS_INPUT_COUNT; i++) {
		m_input_pid[i] = -1;
		m_input_type[i] = 0;
		m_input_tag[i] = -1;
	}
	m_input_count = 0;

	m_pOutputPointer = NULL;
	m_output_size = 0;
	m_status = MMT_PROCESS_STATUS_STOP;
	m_pOutInteface = NULL;
	m_out_inteface_size = 0;
	m_pOutInteface = new unsigned char[MMT_RESOLUTION_4K_SIZE];
	if (m_pOutInteface == NULL) {
		return;
	}
	memset(m_pOutInteface, NULL, MMT_RESOLUTION_4K_SIZE);
	m_out_inteface_size = MMT_RESOLUTION_4K_SIZE;
	m_resolution = 1;

	if (m_LogLevel) {
		APPLOG("[MAN],INF,%d,MmtCapProcessManager,MmtCapProcessManager(),new OutInteface[%d]", m_id, MMT_RESOLUTION_4K_SIZE);
	}

	m_vecListener.clear();

	m_ThreadFlag = 0;
	m_StartFlag = 0;
	m_DataTriggerFlag = 0;
	m_UpadteOutputFlag = 0;

	m_payloadMpuSequenceNumber = 0;
	m_FileComplete = 0;

	m_iGetStatus = -1;
	m_payloadMpuSequenceNumber = 0;
	m_mfuSubsampleNumber = 0;
	m_mfuLastSubsampleNumber = 0;

	m_pDataSubSumpleTemp = NULL;
	m_pDataSubSumpleTempLength = 0;

	m_pDataSubSumpleData0 = NULL;
	m_pDataSubSumpleLength0 = 0;
	m_pDataSubSumpleData1 = NULL;
	m_pDataSubSumpleLength1 = 0;
	m_pDataSubSumpleData2 = NULL;
	m_pDataSubSumpleLength2 = 0;
	m_pDataSubSumpleData3 = NULL;
	m_pDataSubSumpleLength3 = 0;
	m_pDataSubSumpleData4 = NULL;
	m_pDataSubSumpleLength4 = 0;

	m_sNumber.clear();
	m_sOutputImageFile.clear();
	m_OutputImageFileType = 0;
	m_DemoMode = 0;
	m_sVbsPath.clear();

	m_output_count = 0;

	m_iPushMmtCount = 0;

	m_iClearTimeMsec = 0;
	m_iClearTimeCount = 0;
	m_iClearUpdateFlag = 0;

	m_pOutputClearPointer = NULL;
	m_output_clear_size = 0;

	m_pOutputClearPointer = new unsigned char[MMT_RESOLUTION_4K_SIZE];
	if (m_pOutputClearPointer == NULL) {
		return;
	}
	memset(m_pOutputClearPointer, NULL, MMT_RESOLUTION_4K_SIZE);
	m_output_clear_size = MMT_RESOLUTION_4K_SIZE;
	m_ClearTtmlFlag = 0;

}

MmtCapProcessManager::~MmtCapProcessManager()
{
	if (m_LogLevel) {
		APPLOG("[MAN],INF,%d,MmtCapProcessManager,~MmtCapProcessManager(),Destructer START", m_id);
	}

	if (m_release_flag == 0) {
		Release(m_number, m_type);
		if (m_LogLevel) {
			APPLOG("[MAN],INF,%d,MmtCapProcessManager,~MmtCapProcessManager(),Release()", m_id);
		}
	}

	if (m_pOutInteface) {
		delete[] m_pOutInteface;
		m_pOutInteface = NULL;
		if (m_LogLevel) {
			APPLOG("[MAN],INF,%d,MmtCapProcessManager,~MmtCapProcessManager(),Delete OutInteface", m_id);
		}
	}

	if (m_pOutputClearPointer) {
		delete[] m_pOutputClearPointer;
		m_pOutputClearPointer = NULL;
		if (m_LogLevel) {
			APPLOG("[MAN],INF,%d,MmtCapProcessManager,~MmtCapProcessManager(),Delete OutputClearPointer", m_id);
		}
	}

	if (m_pDataSubSumpleTemp) {
		delete[] m_pDataSubSumpleTemp;
		m_pDataSubSumpleTemp = NULL;
		if (m_LogLevel) {
			APPLOG("[MAN],INF,%d,MmtCapProcessManager,~MmtCapProcessManager(),Delete DataSubSumpleTemp", m_id);
		}
	}

	if (m_LogLevel) {
		APPLOG("[MAN],INF,%d,MmtCapProcessManager,~MmtCapProcessManager(),Destructer END", m_id);
	}
}

void MmtCapProcessManager::InitPreview(std::string sLogPath)
{
#if MMT_PROCESS_MMCTRL
	if (Path::IsDir(sLogPath) == 1) {
		ApiInit(m_LogLevel, (char*)(sLogPath.c_str()));
		if (m_LogLevel) {
			APPLOG("[MAN],INF,%d,MmtCapProcessManager,InitPreview(),ApiInit(%d %s)", m_id, m_LogLevel, sLogPath.c_str());
		}
	}
	else {
		ApiInit(m_LogLevel, MMT_LOG_PATH);
		if (m_LogLevel) {
			APPLOG("[MAN],INF,%d,MmtCapProcessManager,InitPreview(),ApiInit(%d %s)", m_id, m_LogLevel, MMT_LOG_PATH);
		}
	}
#endif
}

void MmtCapProcessManager::ExitPreview()
{
#if MMT_PROCESS_MMCTRL
	ApiDestory();
	if (m_LogLevel) {
		APPLOG("[MAN],INF,%d,MmtCapProcessManager,ExitPreview(),ApiDestory()", m_id);
	}
#endif
}

void MmtCapProcessManager::SetResolution(int param)
{
	if (param >= 2) {
		if (m_pOutInteface) {
			delete[] m_pOutInteface;
			m_pOutInteface = NULL;
		}
		m_pOutInteface = new unsigned char[MMT_RESOLUTION_8K_SIZE];
		if (m_pOutInteface == NULL) {
			return;
		}
		memset(m_pOutInteface, NULL, MMT_RESOLUTION_8K_SIZE);
		m_out_inteface_size = MMT_RESOLUTION_8K_SIZE;
		m_resolution = 2;


		if (m_pOutputClearPointer) {
			delete[] m_pOutputClearPointer;
			m_pOutputClearPointer = NULL;
		}
		m_pOutputClearPointer = new unsigned char[MMT_RESOLUTION_8K_SIZE];
		if (m_pOutputClearPointer == NULL) {
			return;
		}
		memset(m_pOutputClearPointer, NULL, MMT_RESOLUTION_8K_SIZE);
		m_output_clear_size = MMT_RESOLUTION_8K_SIZE;
		m_ClearTtmlFlag = 0;
	}
	else {
		m_resolution = param;
	}
	if (m_LogLevel) {
		APPLOG("[MAN],INF,%d,MmtCapProcessManager,SetResolution(%d) resolution=%d", m_id, param, m_resolution);
	}

}

int MmtCapProcessManager::SetParameterInputAdd(int pid, unsigned long type, int tag)
{
	int i = 0;

	if (m_input_count >= MMT_PROCESS_INPUT_COUNT) {
		return -1;
	}

	i = m_input_count;

	m_input_pid[i] = pid;
	m_input_type[i] = type;
	m_input_tag[i] = tag;

	m_input_count++;

	return 0;
}

int MmtCapProcessManager::SetParameterInputClear()
{
	int i = 0;
	for (i = 0; i < MMT_PROCESS_INPUT_COUNT; i++) {
		m_input_pid[i] = -1;
		m_input_type[i] = 0;
		m_input_tag[i] = -1;
		//if (mp_input_capture[i]) {
		//	delete mp_input_capture[i];
		//	mp_input_capture[i] = NULL;
		//}
	}
	m_input_count = 0;

	return 0;
}

int MmtCapProcessManager::SetParameterOutput(unsigned char *pointer, int size)
{
	m_pOutputPointer = pointer;
	m_output_size = size;

	return 0;
}

long MmtCapProcessManager::Create(long number, long type, int resolution)
{
	int ret = 0;
	std::string sParam;

	std::string sGroupName;
	std::string sParamName;
	std::string sTemp;
	char sBuff[64];
	int iTemp;

	//number
	m_number = number;
	if (m_LogLevel) {
		APPLOG("[MAN],INF,%d,MmtCapProcessManager,Create(),number=%d", m_id, m_number);
	}

	//type
	m_type = type;
	if (m_LogLevel) {
		APPLOG("[MAN],INF,%d,MmtCapProcessManager,Create(),type=%d", m_id, m_type);
	}

	if (m_type == MMTLIBMAN_TYPE_CAPTION) {
		if (m_number == 1) {}
		else if ((m_number >= 2) && (m_number <= MMT_PROCESS_TYPE_CAPTION_MAXCOUNT)) {
			m_sNumber = Conv::IntToDecStr(m_number);
		}
		else {
			return -1;
		}
	}

	//resolution
	SetResolution(resolution);
	
	//InitialSetting

	if (m_type == MMTLIBMAN_TYPE_CAPTION) {
		//SubSumple
		m_pDataSubSumpleTemp = new unsigned char[0x10000];
		if (m_pDataSubSumpleTemp == NULL) {
			if (m_LogLevel) {
				APPLOG("[MAN],ERR,%d,MmtCapProcessManager,Create(),new m_pDataSubSumpleTemp[%d] Error", m_id, 0x10000);
			}
			return -1;
		}
		memset(m_pDataSubSumpleTemp, NULL, 0x10000);

		if (m_LogLevel) {
			APPLOG("[MAN],INF,%d,MmtCapProcessManager,Create(),new m_pDataSubSumpleTemp[%d]", m_id, 0x10000);
		}
	}

#if MMT_PROCESS_MMCTRL
	ret = ApiCreate(number, type, resolution);
	if (ret < 0) {
		if (m_LogLevel) {
			APPLOG("[MAN],ERR,%d,MmtCapProcessManager,Create(),ApiCreate(%d,%d,%d) Err=%d", m_id, number, type, resolution, ret);
		}
		Release(number, type);
		if(ret==-302){
		}
		return ret;
	}
	if (m_LogLevel) {
		APPLOG("[MAN],INF,%d,MmtCapProcessManager,Create(),ApiCreate(%d,%d,%d)", m_id, number, type, resolution);
	}

	if (m_type == MMTLIBMAN_TYPE_CAPTION) {
		//[INPUT]
		sGroupName = "Caption";
		//=====================
		//StartupType = 0
		{
			//StartupType
			sParamName = "StartupType";
			iTemp = GetPrivateProfileIntA((LPCTSTR)sGroupName.c_str(), (LPCTSTR)sParamName.c_str(), -1, (LPCTSTR)m_INI_FILENAME.c_str());
			if (iTemp == 0) {
				iTemp = 0;
			}
			else {
				//iTemp = 1;
				iTemp = 0;
			}
			//iTemp = 1;
		}
		//sParam = StringUtil::Format("%s#%d", "StartupType", 0);
		sParam = StringUtil::Format("%s#%d", "StartupType", iTemp);
		ret = ApiSetParam(number, type, (char*)sParam.c_str(), (int)sParam.size());
		if (ret < 0) {
			Release(number, type);
			return ret;
		}
		if (m_LogLevel) {
			APPLOG("[MAN],INF,%d,MmtCapProcessManager,Create(),ApiSetParam(%d,%d,%s)", m_id, number, type, sParam.c_str());
		}
		//=====================
		//ErrorTtmlPath = C:\WORK\RealTimeProcess\error\page.zip
		{
			sParamName = "ErrorTtmlPath";
			memset(sBuff, NULL, 64);
			iTemp = ::GetPrivateProfileStringA((LPCTSTR)sGroupName.c_str(), (LPCTSTR)sParamName.c_str(), "", (LPSTR)sBuff, 64, (LPCTSTR)m_INI_FILENAME.c_str());
			sTemp = sBuff;
			if (Path::IsExist(sTemp) == 0) {
				APPLOG("[MAN],ERR,%d,MmtCapProcessManager,Create(),ErrorTtmlPath is nothing (%s)", m_id, sTemp.c_str());
				return -1;
			}
			if (m_LogLevel) {
				APPLOG("[MAN],INF,%d,MmtCapProcessManager,Create(),ErrorTtmlPath=%s", m_id, sTemp.c_str());
			}
		}
		sParam = StringUtil::Format("%s#%s", "ErrorTtmlPath", sTemp.c_str());
		ret = ApiSetParam(number, type, (char*)sParam.c_str(), (int)sParam.size());
		if (ret < 0) {
			Release(number, type);
			return ret;
		}
		if (m_LogLevel) {
			APPLOG("[MAN],INF,%d,MmtCapProcessManager,Create(),ApiSetParam(%d,%d,%s)", m_id, number, type, sParam.c_str());
		}
		//=====================
		//ClearTtmlPath
		{
			sParamName = "ClearTtmlPath";
			memset(sBuff, NULL, 64);
			iTemp = ::GetPrivateProfileStringA((LPCTSTR)sGroupName.c_str(), (LPCTSTR)sParamName.c_str(), "", (LPSTR)sBuff, 64, (LPCTSTR)m_INI_FILENAME.c_str());
			sTemp = sBuff;
			if (Path::IsExist(sTemp) == 0) {
				APPLOG("[MAN],ERR,%d,MmtCapProcessManager,Create(),ClearTtmlPath is nothing (%s)", m_id, sTemp.c_str());
				return -1;
			}
			m_ClearTtml = sTemp;
			if (m_LogLevel) {
				APPLOG("[MAN],INF,%d,MmtCapProcessManager,Create(),ClearTtmlPath=%s", m_id, m_ClearTtml.c_str());
			}
		}
		//=====================
		//OutputImageFileType
		// 0:argb 1:png 2:bmp
		m_OutputImageFileType = 0;
		if (m_LogLevel) {
			APPLOG("[MAN],INF,%d,MmtCapProcessManager,Create(),OutputImageFileType=%d", m_id, m_OutputImageFileType);
		}
		//=====================
		//FontFamily
		{
			sParamName = "FontFamily";
			memset(sBuff, NULL, 64);
			iTemp = ::GetPrivateProfileStringA((LPCTSTR)sGroupName.c_str(), (LPCTSTR)sParamName.c_str(), "", (LPSTR)sBuff, 64, (LPCTSTR)m_INI_FILENAME.c_str());
			sTemp = sBuff;
			if (sTemp.size() == 0) {
				sTemp = "MS Gothic";
			}
		}
		//sParam = StringUtil::Format("%s#%s", "FontFamily", "MS Gothic");
		sParam = StringUtil::Format("%s#%s", "FontFamily", sTemp.c_str());
		ret = ApiSetParam(number, type, (char*)sParam.c_str(), (int)sParam.size());
		if (ret < 0) {
			if (m_LogLevel) {
				APPLOG("[MAN],ERR,%d,MmtCapProcessManager,Create(),ApiSetParam(%d,%d,%s)=%d", m_id, number, type, sParam.c_str(), ret);
			}
			Release(number, type);
			return ret;
		}
		if (m_LogLevel) {
			APPLOG("[MAN],INF,%d,MmtCapProcessManager,Create(),ApiSetParam(%d,%d,%s)", m_id, number, type, sParam.c_str());
		}
	}

	ret = ApiStart(number, type);
	if (ret < 0) {
		if (m_LogLevel) {
			APPLOG("[MAN],ERR,%d,MmtCapProcessManager,Create(),ApiStart(%d,%d) Err=%d", m_id, number, type, ret);
		}
		Release(number, type);
		return ret;
	}
	if (m_LogLevel) {
		APPLOG("[MAN],INF,%d,MmtCapProcessManager,Create(),ApiStart(%d,%d)", m_id, number, type);
	}
#endif

	m_CreateFlag = 1;

	return 0;
}


long MmtCapProcessManager::Release(long number, long type)
{
	int ret = 0;

	if (m_LogLevel) {
		APPLOG("[MAN],INF,%d,MmtCapProcessManager,Release(%d,%d) START", m_id, number, type);
	}

	//現在のスレッド処理の中止
	ThreadFlagStop();
	if (m_LogLevel) {
		APPLOG("[MAN],INF,%d,MmtCapProcessManager,Release() ThreadFlagStop(%d)", m_id, m_ThreadFlag);
	}

	//スレッド停止
	StopThread();
	if (m_LogLevel) {
		APPLOG("[MAN],INF,%d,MmtCapProcessManager,Release() StopThread()", m_id);
	}

#if MMT_PROCESS_MMCTRL
	ret = ApiRelease(number, type);
	if (ret < 0) {
		if (m_LogLevel) {
			APPLOG("[MAN],ERR,%d,MmtCapProcessManager,Release(),ApiRelease(%d,%d) Err=%d", m_id, number, type, ret);
		}
		return ret;
	}
	if (m_LogLevel) {
		APPLOG("[MAN],INF,%d,MmtCapProcessManager,Release(),ApiRelease(%d,%d)", m_id, number, type);
	}
#endif

	if (m_LogLevel) {
		APPLOG("[MAN],INF,%d,MmtCapProcessManager,Release(%d,%d) END", m_id, number, type);
	}

	m_release_flag = 1;

	return 0;
}


int MmtCapProcessManager::SetNtp(unsigned long long uNtp)
{
	int iUpdateFlag = 0;
	unsigned long long uNowNtp = 0;
	unsigned long long uNtpRange = 270000; //10msec
	unsigned long long uNowNtpUp = 0;
	unsigned long long uNowNtpDn = 0;

	return 0;

	if (m_status == MMT_PROCESS_STATUS_STOP) {
		return 0;
	}

	if (m_pClockTime == NULL) {
		return -1;
	}
	uNowNtpUp = uNowNtp + uNtpRange;
	uNowNtpDn = uNowNtp - uNtpRange;

	if (uNtp < uNowNtpDn) {
		iUpdateFlag++;
	}

	if (uNtp > uNowNtpUp) {
		iUpdateFlag++;
	}

	//±10msecを超える場合
	if (iUpdateFlag) {
		//m_pClockTime->InputTimeStampSTC(uNtp);
	}

	m_ntp = uNtp;

	return 0;
}

int MmtCapProcessManager::PushMmtData(unsigned char *pointer, int size)
{
	int i = 0;
	int ret = 0;
	MmtpPacket *pMmtpPacket = NULL;
	int pid = 0;

	if (m_CreateFlag == 0) {
		if (m_LogLevel) {
			APPLOG("[MAN],INF,%d,MmtCapProcessManager,PushMmtData(),CreateFlag=%d", m_id, m_CreateFlag);
		}
		return -1;
	}

	if (m_LogLevel&&MMT_DEBUG_LOG) {
		APPLOG("[MAN],INF,%d,MmtCapProcessManager,PushMmtData(),Start", m_id);
	}

	if (MMT_DEBUG_LOG) {
		APPLOG("[MAN],DBG,%d,MmtCapProcessManager,PushMmtData(),[ %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X ]",
			m_id,
			pointer[0], pointer[1], pointer[2], pointer[3],
			pointer[4], pointer[5], pointer[6], pointer[7],
			pointer[8], pointer[9], pointer[10], pointer[11],
			pointer[12], pointer[13], pointer[14], pointer[15]
		);
		APPLOG("[MAN],DBG,%d,MmtCapProcessManager,PushMmtData(),[ %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X ]",
			m_id,
			pointer[size-16], pointer[size-15], pointer[size-14], pointer[size-13],
			pointer[size-12], pointer[size-11], pointer[size-10], pointer[size-9],
			pointer[size-8], pointer[size-7], pointer[size-6], pointer[size-5],
			pointer[size-4], pointer[size-3], pointer[size-2], pointer[size-1]
		);
	}

	m_iPushMmtCount++;

	pMmtpPacket = new MmtpPacket;
	if (pMmtpPacket == NULL) {
		return -1;
	}

	ret = pMmtpPacket->SetPacket(pointer, size, NULL);
	if ( ret < 0 ) {
		if (m_LogLevel) {
			APPLOG("[MAN],ERR,%d,MmtCapProcessManager,PushMmtData(),SetPacket(%p,%d) Err=%d", m_id, pointer, size, ret);
		}
		delete pMmtpPacket;
		return ret;
	}
	if (m_LogLevel&&MMT_DEBUG_LOG) {
		APPLOG("[MAN],INF,%d,MmtCapProcessManager,PushMmtData(),SetPacket(%p,%d) PTYPE=%d", m_id, pointer, size, pMmtpPacket->getPayload_type());
	}

	pid = (int)pMmtpPacket->getPacket_id();

	for (i = 0; i < m_input_count; i++) {
		if (pid == m_input_pid[i]) {
			if (m_LogLevel&&MMT_DEBUG_LOG) {
				APPLOG("[MAN],INF,%d,MmtCapProcessManager,PushMmtData(),PID=%d(0x%04X)", m_id, pid, pid);
			}

			ret = CaptureMmtPacket(pMmtpPacket, i, m_input_type[i]);
			if (ret < 0) {
				if (m_LogLevel) {
					APPLOG("[MAN],ERR,%d,MmtCapProcessManager,PushMmtData(),CaptureMmtPacket() Err=%d", m_id, ret);
				}
				return ret;
			}
			if (m_LogLevel&&MMT_DEBUG_LOG) {
				APPLOG("[MAN],INF,%d,MmtCapProcessManager,PushMmtData(),CaptureMmtPacket()", m_id);
			}
			break;
		}
	}

	delete pMmtpPacket;

	return 0;
}

int MmtCapProcessManager::AppendListener(IMmtCapCaptureListener* pListener)
{
	if (pListener == NULL) {
		return -1;
	}

	m_vecListener.push_back(pListener);

	if (m_LogLevel) {
		APPLOG("[MAN],INF,%d,MmtCapProcessManager,AppendListener(),Listener Add", m_id);
	}

	return 0;
}

int MmtCapProcessManager::RemoveListener(IMmtCapCaptureListener* pListener)
{
	int i = 0;
	if (pListener == NULL) {
		return -1;
	}

	// 全要素に対するループ
	for (auto itr = m_vecListener.begin(); itr != m_vecListener.end(); ) {
		if ((*itr) == (pListener)) {
			itr = m_vecListener.erase(itr);
		}
		else {
			++itr;
		}
	}

	if (m_LogLevel) {
		APPLOG("[MAN],INF,%d,MmtCapProcessManager,RemoveListener(),Listener Remove", m_id);
	}

	return 0;
}

int MmtCapProcessManager::EventError(unsigned long ErrorCode)
{
	int id = GetId();

	LISTENER_VECTOR::iterator ite;
	for (ite = m_vecListener.begin(); ite != m_vecListener.end(); ite++)
	{
		//エラーイベント通知
		(*ite)->MmtCapManApiEventError(id, ErrorCode);
	}

	return 0;
}

int MmtCapProcessManager::EventUpdateCaptionPlane()
{
	int id = GetId();

	LISTENER_VECTOR::iterator ite;
	for (ite = m_vecListener.begin(); ite != m_vecListener.end(); ite++)
	{
		//画像出力通知
		(*ite)->MmtCapOutputApiEventUpdateCaptionPlane(id);
		if (m_LogLevel) {
			APPLOG("[MAN],INF,%d,MmtCapProcessManager,EventUpdateCaptionPlane()", m_id);
		}
	}

	return 0;
}

int MmtCapProcessManager::ThreadInitProc() {
	m_StartFlag = 1;
	m_ThreadFlag = 1;
	m_iClearTimeMsec = 0;
	m_iClearTimeCount = 0;
	return 0;
}

int MmtCapProcessManager::ThreadProc() {

	int ret = 0;
	int iCount = 0;
	int iVideoSizeNone = 0;
	m_ThreadFlag = 1;
	m_DataTriggerFlag = 0;
	//char sBuff[4096];
	int iCommandResSize = 0;
	int iCommandResDataSize = 0;
	std::string sCommandRes = "<EVENT-PAGE-INFO-RES>";
	std::string sCommandResData = "";
	std::string sCommandResPath;
	std::string sCommandResName;
	std::string sCommandResX;
	std::string sCommandResY;
	std::string sCommandResH;
	std::string sCommandResW;
	std::string sCommandResAudio;
	int iResValueIndex = 0;
	std::string sStartCmd;

#if MMT_CAPTION_DISPLAY_NEW
	int iCaptionFifoCount = 0;
	int iCaptionDisplayTimeCheck = 0;
	unsigned char* pOutputBuffer = NULL;
	int iResolutionSize = 0;
	if (m_resolution == 0) {
		iResolutionSize = MMT_RESOLUTION_2K_SIZE;
	}
	else if (m_resolution == 1) {
		iResolutionSize = MMT_RESOLUTION_4K_SIZE;
	}
	else if (m_resolution >= 2) {
		iResolutionSize = MMT_RESOLUTION_8K_SIZE;
	}
	pOutputBuffer = new unsigned char[iResolutionSize];
	if (pOutputBuffer == NULL) {
		return -1;
	}
	//init
	memset(pOutputBuffer, NULL, iResolutionSize);
#endif

	do {

		if (m_ThreadFlag==0) {
			//break;
			//delete[] pOutputBuffer;
			//pOutputBuffer = NULL;
			if (m_LogLevel) {
				APPLOG("[MAN],INF,%d,MmtCapProcessManager,ThreadProc() End", m_id);
			}
			return -1;
		}

#if MMT_INTERFACE_TYPE
		//通知
		if (m_UpadteOutputFlag) {
			if (m_type == MMTLIBMAN_TYPE_CAPTION) {
				EventUpdateCaptionPlane();
			}
			m_UpadteOutputFlag = 0;
		}
#else
		if (m_type == MMTLIBMAN_TYPE_CAPTION) {
			if (m_iClearUpdateFlag) {
				m_iClearUpdateFlag = 0;
				if (m_LogLevel) {
					APPLOG("[MAN],INF,%d,MmtCapProcessManager,ThreadProc(),m_iClearUpdateFlag=%d UpdateClearScreen()", m_id, m_iClearUpdateFlag);
				}
				ret = UpdateClearScreen();
				if (ret < 0) {
					APPLOG("[MAN],ERR,%d,MmtCapProcessManager,ThreadProc(),m_iClearUpdateFlag=%d UpdateClearScreen() rt=%d", m_id, m_iClearUpdateFlag, ret);
				}
			}
		}

#if MMT_CAPTION_DISPLAY_NEW
		if (m_type == MMTLIBMAN_TYPE_CAPTION) {

			iCaptionFifoCount = m_fifoCaption.size();

			if (iCaptionFifoCount > 0) {

				unsigned char* pOutInterfaceBuffer = NULL;
				unsigned long long i64OutTimestampSTC = 0;
				int iRomSound = 0;
				int iPrsCountOutput = 0;

				if (iCaptionDisplayTimeCheck == 0) {

					//初めの項目を取得
					map<unsigned char*, unsigned long long, int>::iterator it = m_fifoCaption.begin();
					//std::map<unsigned char*, unsigned long long> fifoCaption;
					//fifoCaption.insert(map<unsigned char*, unsigned long long>::value_type(pOutInterfaceBuffer, i64OutTimestamp));

					//値を取得
					pOutInterfaceBuffer = it->first;
					memset(pOutputBuffer, NULL, iResolutionSize);
					memcpy(pOutputBuffer, pOutInterfaceBuffer, iResolutionSize);
					i64OutTimestampSTC = (it->second) * 300;
					//i64OutTimestampSTC += TSS_TIMESTAMP_DELTA * 27000;

					iRomSound = pOutInterfaceBuffer[iResolutionSize];
					if (iRomSound == 0xFF) {
						iRomSound = -1;
					}
					iPrsCountOutput = pOutInterfaceBuffer[iResolutionSize + 1];

					//mapから削除
					int iCaptionCountBefore = m_fifoCaption.size();
					m_fifoCaption.erase(it);
					int iCaptionCountAfter = m_fifoCaption.size();
					if (m_LogLevel) {
						APPLOG("[MAN],INF,%d,TssProcessManager,ThreadProc(),[MAP] #DEL Count=%d->%d", m_id, iCaptionCountBefore, iCaptionCountAfter);
					}
					//メモリを破棄
					if (pOutInterfaceBuffer) {
						delete[] pOutInterfaceBuffer;
					}

					iCaptionDisplayTimeCheck = 1;
				}

				//ここで表示時刻のチェック
				if (iCaptionDisplayTimeCheck) {
#if	MMT_TIMESTAMP_NOCHECK
					ret = 100;
#else
					if (m_pClockTime) {
						ret = m_pClockTime->CheckTimeStamp(i64OutTimestampSTC, (TSS_TIMESTAMP_DELTA * 27000));
					}
					else {
						ret = 1001;
					}
					if (m_LogLevel) {
						APPLOG("[MAN],INF,%d,TssProcessManager,ThreadProc(),CheckTimeStamp(%016X)=%d", m_id, i64OutTimestampSTC, ret);
					}
#endif
					//字幕を破棄
					if ((ret == 1000) || (ret == 1001)) {
						//廃止
						//m_DataTriggerFlag = 0;
						if (m_LogLevel) {
							APPLOG("[MAN],INF,%d,TssProcessManager,ThreadProc(),[CAP-OT] SKEP STC=%016X", m_id, iPrsCountOutput, i64OutTimestampSTC);
						}
						iCaptionDisplayTimeCheck = 0;
					}
					//字幕を表示
					else if (ret == 100) {
						//廃止
						//m_DataTriggerFlag = 0;

						//メモリにコピー							
						memcpy(m_pOutputPointer, pOutputBuffer, iResolutionSize);

						//更新処理
						EventUpdateCaptionPlane();

						if (m_LogLevel) {
							APPLOG("[MAN],INF,%d,TssProcessManager,ThreadProc(),[CAP-OT] PES=%04d STC=%016X", m_id, iPrsCountOutput, i64OutTimestampSTC);
						}

						iCaptionDisplayTimeCheck = 0;

#if 0
						if (TSS_INTERFACE_SAVE) {
							FILE *fpt = NULL;
							std::string savePath = m_FileSavePath;
							std::string sPageZipTemp = m_sInputTempPageZipFile;
							std::string saveName = sPageZipTemp + ".argb";
							saveName = StringUtil::Format("%s\\Caption_%04d_ImageCopy", savePath.c_str(), m_iPesCount);

							if (m_LogLevel) {
								int packetId = 0;
								APPLOG("[MAN],INF,%d,TssProcessManager,ParsePesData(),[TSS] PES=%04d #SAVE path=%s", m_id, m_iPesCount, saveName.c_str());
							}

							//saveName += ".argb";
							if (m_OutputImageFileType == 0) {
								//saveName += Conv::IntToDecStr(m_output_count);
								saveName += ".argb";
							}
							else if (m_OutputImageFileType == 1) {
								//saveName += Conv::IntToDecStr(m_output_count);
								saveName += ".png";
							}
							else if (m_OutputImageFileType == 2) {
								//saveName += Conv::IntToDecStr(m_output_count);
								saveName += ".bmp";
							}

							if (m_LogLevel) {
								APPLOG("[MAN],INF,%d,TssProcessManager,ParsePesData(),[TSS] PES=%04d #SAVE name=%s", m_id, m_iPesCount, saveName.c_str());
							}

							fpt = fopen(saveName.c_str(), "w+b");
							if (fpt == NULL) {
								return -1;
							}
							fseek(fpt, 0, SEEK_SET);
							fwrite(m_pOutInteface, m_out_inteface_size, 1, fpt);
							fclose(fpt);

							if (m_LogLevel) {
								int packetId = 0;
								APPLOG("[MAN],INF,%d,TssProcessManager,ParsePesData(),[TSS] PES=%04d #SAVE=%s", m_id, m_iPesCount, saveName.c_str());
							}
						}
#endif
					}
					//字幕を待機
					else {
						//Sleep(50); //50msec
						//continue;
					}
				}
				else {
					//Sleep(50); //50msec
					//continue;
				}
			}

		}
#endif

		if (m_DataTriggerFlag) {

			if (m_LogLevel&&MMT_DEBUG_LOG) {
				APPLOG("[MAN],INF,%d,MmtCapProcessManager,ThreadProc(),m_DataTriggerFlag=%d", m_id, m_DataTriggerFlag);
			}

			//m_DataTriggerFlag = 0;

#if MMT_CAPTION_DISPLAY_OLD
			if (m_type == MMTLIBMAN_TYPE_CAPTION) {

				m_DataTriggerFlag = 0;

				ret = EventUpdateCaptionPlane();
				if (ret < 0) {
					APPLOG("[MAN],ERR,%d,MmtCapProcessManager,ThreadProc(),EventUpdateCaptionPlane() ret=%d", m_id, ret);
				}
				else {
					APPLOG("[MAN],INF,%d,MmtCapProcessManager,ThreadProc(),EventUpdateCaptionPlane()", m_id);
				}
			}
#endif
		}
#endif

		Sleep(50); //50msec

		if (m_type == MMTLIBMAN_TYPE_CAPTION) {
			if (m_iClearTimeMsec > 0) {
				if (m_iClearTimeCount == 0) {
					if (m_LogLevel) {
						APPLOG("[MAN],INF,%d,MmtCapProcessManager,ThreadProc(),m_iClearTimeMsec=%d m_iClearTimeCount=%d", m_id, m_iClearTimeMsec, m_iClearTimeCount);
					}
				}
				m_iClearTimeCount += 60;
				if (m_iClearTimeCount >= m_iClearTimeMsec) {
					if (m_LogLevel) {
						APPLOG("[MAN],INF,%d,MmtCapProcessManager,ThreadProc(),m_iClearTimeMsec=%d > m_iClearTimeCount=%d", m_id, m_iClearTimeMsec, m_iClearTimeCount);
					}
					m_iClearTimeMsec = 0;
					m_iClearTimeCount = 0;
					m_iClearUpdateFlag = 1;
					m_DataTriggerFlag = 1;
					if (m_LogLevel) {
						APPLOG("[MAN],INF,%d,MmtCapProcessManager,ThreadProc(),m_iClearTimeMsec=%d m_iClearTimeCount=%d m_iClearUpdateFlag=%d", m_id, m_iClearTimeMsec, m_iClearTimeCount, m_iClearUpdateFlag);
					}
				}
				if (m_LogLevel) {
					APPLOG("[MAN],INF,%d,MmtCapProcessManager,ThreadProc(),m_iClearTimeMsec=%d m_iClearTimeCount=%d", m_id, m_iClearTimeMsec, m_iClearTimeCount);
				}
			}
			else {
				m_iClearTimeMsec = 0;
				m_iClearTimeCount = 0;
			}
		}

	} while (1);

	m_ThreadFlag = 0;

#if MMT_CAPTION_DISPLAY_NEW
	delete[] pOutputBuffer;
	pOutputBuffer = NULL;
#endif

	if (m_LogLevel) {
		APPLOG("[MAN],INF,%d,MmtCapProcessManager,ThreadProc() End", m_id);
	}


	return 0;
}

int MmtCapProcessManager::ThreadTermProc() {

	std::string sCmd;

	m_ThreadFlag = 0;

	return 0;
}

int MmtCapProcessManager::CaptureMmtPacket(MmtpPacket *packet, int i, unsigned long type)
{
	int ret = 0;

	unsigned short packetId = packet->getPacket_id();
	unsigned long payloadLen = packet->getMMTP_payload_length();
	unsigned char* payloadPointer = packet->getMMTP_payload();
	unsigned char payloadType = packet->getPayload_type();

	{
		int __index = 0;
		int __payloadPayloadLength = 0;
		int __payloadFragmentType = 0;
		int __payloadTimedFlag = 0;
		int __payloadFragmentationIndicator = 0;
		int __payloadAggregationFlag = 0;
		__payloadPayloadLength = payloadPointer[__index] * 256;
		__index++;
		__payloadPayloadLength += payloadPointer[__index];
		__index++;
		__payloadFragmentType = (payloadPointer[__index] >> 4) & 0x0F;
		__payloadTimedFlag = (payloadPointer[__index] >> 3) & 0x01;
		__payloadFragmentationIndicator = (payloadPointer[__index] >> 1) & 0x03;
		__payloadAggregationFlag = (payloadPointer[__index] >> 0) & 0x01;
		__index++;
		__payloadAggregationFlag = payloadPointer[__index];
		__index++;
		if (MMT_DEBUG_LOG) {
			APPLOG("[MAN],DBG,%d,MmtCapProcessManager,CaptureMmtPacket(),pid=%d(0x%04X) type=0x%08X len=%d <plen=%d ptype=%d pflag=%d pind=%d pagf=%d> ", m_id
				, packetId, packetId, type, payloadLen
				, __payloadPayloadLength, __payloadFragmentType, __payloadTimedFlag, __payloadFragmentationIndicator, __payloadAggregationFlag);
		}
		if (MMT_DEBUG_LOG) {
			APPLOG("[MAN],DBG,%d,MmtCapProcessManager,CaptureMmtPacket(),[ %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X ]",
				m_id,
				payloadPointer[0], payloadPointer[1], payloadPointer[2], payloadPointer[3],
				payloadPointer[4], payloadPointer[5], payloadPointer[6], payloadPointer[7],
				payloadPointer[8], payloadPointer[9], payloadPointer[10], payloadPointer[11],
				payloadPointer[12], payloadPointer[13], payloadPointer[14], payloadPointer[15]
			);
		}
	}
	if (type == MMT_TYPEPID_MPU_TTMLCAPTION) {
		//従来方式
	}

	if (payloadType != 0x00) {
		if (m_LogLevel) {
			APPLOG("[MAN],WAR,%d,MmtCapProcessManager,CaptureMmtPacket(),[MMT] payloadType=0x%02X(%d)", m_id, payloadType, payloadType);
		}
		return -1;
	}

	int mfuSubtitleTag = 0;
	int mfuSbtitleSequenceNumber = 0;
	int mfuSubsampleNumber = 0;
	int mfuLastSubsampleNumber = 0;
	int mfuDataType = 0;
	int mfuLengthExtensionFlag = 0;
	int mfuSubsampleInfoListFlag = 0;
	int mfuReserved1 = 0;
	int mftDataSize = 0;
	int mftFileComplete = 0;

	//m_Time.Now(); COUT("%s [MMT] PID=0x%04X(%d) #PROC START =========================", m_Time.GetHHMMSSFFF().c_str(), packetId, packetId);
	if (m_LogLevel) {
		APPLOG("[MAN],INF,%d,MmtCapProcessManager,CaptureMmtPacket(),[MMT] PID=0x%04X(%d) #PROC START ============", m_id, packetId, packetId);
	}

	int _index = 0;
	int payloadPayloadLength = 0;
	int payloadFragmentType = 0;
	int payloadTimedFlag = 0;
	int payloadFragmentationIndicator = 0;
	int payloadAggregationFlag = 0;
	int payloadFragmentCounter = 0;
	unsigned long payloadMpuSequenceNumber = 0;
	unsigned long payloadTtemId = 0;
	unsigned char* payloadMfuData = NULL;

	payloadPayloadLength = payloadPointer[_index] * 256;
	_index++;
	payloadPayloadLength += payloadPointer[_index];
	_index++;
	payloadFragmentType = (payloadPointer[_index] >> 4) & 0x0F;
	payloadTimedFlag = (payloadPointer[_index] >> 3) & 0x01;
	payloadFragmentationIndicator = (payloadPointer[_index] >> 1) & 0x03;
	payloadAggregationFlag = (payloadPointer[_index] >> 0) & 0x01;
	_index++;
	payloadFragmentCounter = payloadPointer[_index];
	_index++;

	payloadMpuSequenceNumber = payloadPointer[_index] * 256 * 256 * 256;
	_index++;
	payloadMpuSequenceNumber += payloadPointer[_index] * 256 * 256;
	_index++;
	payloadMpuSequenceNumber += payloadPointer[_index] * 256;
	_index++;
	payloadMpuSequenceNumber += payloadPointer[_index];
	_index++;

	m_payloadMpuSequenceNumber = payloadMpuSequenceNumber;

	unsigned long mfuDataLen = 0;
	unsigned char* mfuDataPointer = NULL;

	m_FileComplete = 0;

	if (m_LogLevel) {
		APPLOG("[MAN],INF,%d,MmtCapProcessManager,CaptureMmtPacket(),[MMT] PID=0x%04X(%d) #FLAG payloadFragmentationIndicator=%d", m_id, packetId, packetId, payloadFragmentationIndicator);
	}

	//先頭＆最終
	if (payloadFragmentationIndicator == 0) {

		mfuSubtitleTag = 0;
		mfuSbtitleSequenceNumber = 0;
		mfuSubsampleNumber = 0;
		mfuLastSubsampleNumber = 0;
		mfuDataType = 0;
		mfuLengthExtensionFlag = 0;
		mfuSubsampleInfoListFlag = 0;
		mfuReserved1 = 0;
		mftDataSize = 0;

		m_iGetStatus = 1;

		if (payloadTimedFlag == 1) {
			_index += 4;//movie_fragment_sequence_number 
			_index += 4;//sample_number 
			_index += 4;//offset
			_index++;	//priority 
			_index++;	//dependency_counter
		}

		mfuSubtitleTag = payloadPointer[_index];
		_index++;
		mfuSbtitleSequenceNumber = payloadPointer[_index];
		_index++;
		mfuSubsampleNumber = payloadPointer[_index];
		_index++;
		mfuLastSubsampleNumber = payloadPointer[_index];
		_index++;
		m_mfuSubsampleNumber = mfuSubsampleNumber;
		m_mfuLastSubsampleNumber = mfuLastSubsampleNumber;

		mfuDataType = (payloadPointer[_index] >> 4) & 0x0F;
		mfuLengthExtensionFlag = (payloadPointer[_index] >> 3) & 0x01;
		mfuSubsampleInfoListFlag = (payloadPointer[_index] >> 2) & 0x01;
		mfuReserved1 = (payloadPointer[_index] >> 0) & 0x03;
		_index++;

		mftDataSize = payloadPointer[_index] * 256;
		_index++;
		mftDataSize += payloadPointer[_index];
		_index++;
		m_mfuSubsampleDataSize = mftDataSize;

		mfuDataLen = payloadLen - _index;
		mfuDataPointer = payloadPointer + _index;

		memset(m_pDataSubSumpleTemp, NULL, 0x10000);
		m_pDataSubSumpleTempLength = 0;

		//ここでメモリにためる
		{
			//メモリオーバーチェック
			memcpy(m_pDataSubSumpleTemp + m_pDataSubSumpleTempLength, mfuDataPointer, mfuDataLen);
			m_pDataSubSumpleTempLength += mfuDataLen;
		}
		m_Time.Now(); COUT("%s [MMT] PID=0x%04X(%d) #SE %d/%d %06d/%06d %06d N=%08d", m_Time.GetHHMMSSFFF().c_str(), packetId, packetId, m_mfuSubsampleNumber, m_mfuLastSubsampleNumber, mfuDataLen, m_mfuSubsampleDataSize, m_pDataSubSumpleTempLength, payloadMpuSequenceNumber);
		if (m_LogLevel) {
			APPLOG("[MAN],INF,%d,MmtCapProcessManager,CaptureMmtPacket(),[MMT] PID=0x%04X(%d) #SE %d/%d %06d/%06d %06d N=%08d", m_id, packetId, packetId, m_mfuSubsampleNumber, m_mfuLastSubsampleNumber, mfuDataLen, m_mfuSubsampleDataSize, m_pDataSubSumpleTempLength, payloadMpuSequenceNumber);
		}

		m_iGetStatus = 0;

		m_FileComplete = 1;
	}
	//先頭
	else if (payloadFragmentationIndicator == 1) {

		mfuSubtitleTag = 0;
		mfuSbtitleSequenceNumber = 0;
		mfuSubsampleNumber = 0;
		mfuLastSubsampleNumber = 0;
		mfuDataType = 0;
		mfuLengthExtensionFlag = 0;
		mfuSubsampleInfoListFlag = 0;
		mfuReserved1 = 0;
		mftDataSize = 0;

		m_iGetStatus = 1;

		if (payloadTimedFlag == 1) {
			_index += 4;//movie_fragment_sequence_number 
			_index += 4;//sample_number 
			_index += 4;//offset
			_index++;	//priority 
			_index++;	//dependency_counter
		}

		mfuSubtitleTag = payloadPointer[_index];
		_index++;
		mfuSbtitleSequenceNumber = payloadPointer[_index];
		_index++;
		mfuSubsampleNumber = payloadPointer[_index];
		_index++;
		mfuLastSubsampleNumber = payloadPointer[_index];
		_index++;
		m_mfuSubsampleNumber = mfuSubsampleNumber;
		m_mfuLastSubsampleNumber = mfuLastSubsampleNumber;

		if (m_mfuSubsampleNumber == 1) {
			int aaa = 0;
		}

		mfuDataType = (payloadPointer[_index] >> 4) & 0x0F;
		mfuLengthExtensionFlag = (payloadPointer[_index] >> 3) & 0x01;
		mfuSubsampleInfoListFlag = (payloadPointer[_index] >> 2) & 0x01;
		mfuReserved1 = (payloadPointer[_index] >> 0) & 0x03;
		_index++;
		m_mfuSubsampleDataType = mfuDataType;

		mftDataSize = payloadPointer[_index] * 256;
		_index++;
		mftDataSize += payloadPointer[_index];
		_index++;
		m_mfuSubsampleDataSize = mftDataSize;

		mfuDataLen = payloadLen - _index;
		mfuDataPointer = payloadPointer + _index;

		memset(m_pDataSubSumpleTemp, NULL, 0x10000);
		m_pDataSubSumpleTempLength = 0;

		//ここでメモリにためる
		{
			//メモリオーバーチェック
			memcpy(m_pDataSubSumpleTemp + m_pDataSubSumpleTempLength, mfuDataPointer, mfuDataLen);
			m_pDataSubSumpleTempLength += mfuDataLen;
		}
		m_Time.Now(); COUT("%s [MMT] PID=0x%04X(%d) #S- %d/%d %06d/%06d %06d N=%08d", m_Time.GetHHMMSSFFF().c_str(), packetId, packetId, m_mfuSubsampleNumber, m_mfuLastSubsampleNumber, mfuDataLen, m_mfuSubsampleDataSize, m_pDataSubSumpleTempLength, payloadMpuSequenceNumber);
		if (m_LogLevel) {
			APPLOG("[MAN],INF,%d,MmtCapProcessManager,CaptureMmtPacket(),[MMT] PID=0x%04X(%d) #S- %d/%d %06d/%06d %06d N=%08d", m_id, packetId, packetId, m_mfuSubsampleNumber, m_mfuLastSubsampleNumber, mfuDataLen, m_mfuSubsampleDataSize, m_pDataSubSumpleTempLength, payloadMpuSequenceNumber);
		}

		m_FileComplete = 0;
	}
	//途中
	else if (payloadFragmentationIndicator == 2) {

		if (payloadTimedFlag == 1) {
			_index += 4;//movie_fragment_sequence_number 
			_index += 4;//sample_number 
			_index += 4;//offset
			_index++;	//priority 
			_index++;	//dependency_counter
		}

		if (m_iGetStatus == 1) {

			mfuDataLen = payloadLen - _index;
			mfuDataPointer = payloadPointer + _index;

			//ここでメモリにためる
			{
				//メモリオーバーチェック
				memcpy(m_pDataSubSumpleTemp + m_pDataSubSumpleTempLength, mfuDataPointer, mfuDataLen);
				m_pDataSubSumpleTempLength += mfuDataLen;
			}
			m_Time.Now(); COUT("%s [MMT] PID=0x%04X(%d) #-- %d/%d %06d/%06d %06d", m_Time.GetHHMMSSFFF().c_str(), packetId, packetId, m_mfuSubsampleNumber, m_mfuLastSubsampleNumber, mfuDataLen, m_mfuSubsampleDataSize, m_pDataSubSumpleTempLength);
			if (m_LogLevel) {
				APPLOG("[MAN],INF,%d,MmtCapProcessManager,CaptureMmtPacket(),[MMT] PID=0x%04X(%d) #-- %d/%d %06d/%06d %06d", m_id, packetId, packetId, m_mfuSubsampleNumber, m_mfuLastSubsampleNumber, mfuDataLen, m_mfuSubsampleDataSize, m_pDataSubSumpleTempLength);
			}

			m_iGetStatus = 1;

			m_FileComplete = 0;
		}

	}
	//最終
	else if (payloadFragmentationIndicator == 3) {

		if (payloadTimedFlag == 1) {
			_index += 4;//movie_fragment_sequence_number 
			_index += 4;//sample_number 
			_index += 4;//offset
			_index++;	//priority 
			_index++;	//dependency_counter
		}

		if (m_iGetStatus == 1) {

			mfuDataLen = payloadLen - _index;
			mfuDataPointer = payloadPointer + _index;

			//ここでメモリにためる
			{
				//メモリオーバーチェック
				memcpy(m_pDataSubSumpleTemp + m_pDataSubSumpleTempLength, mfuDataPointer, mfuDataLen);
				m_pDataSubSumpleTempLength += mfuDataLen;
			}
			m_Time.Now(); COUT("%s [MMT] PID=0x%04X(%d) #-E %d/%d %06d/%06d %06d", m_Time.GetHHMMSSFFF().c_str(), packetId, packetId, m_mfuSubsampleNumber, m_mfuLastSubsampleNumber, mfuDataLen, m_mfuSubsampleDataSize, m_pDataSubSumpleTempLength);
			if (m_LogLevel) {
				APPLOG("[MAN],INF,%d,MmtCapProcessManager,CaptureMmtPacket(),[MMT] PID=0x%04X(%d) #-E %d/%d %06d/%06d %06d", m_id, packetId, packetId, m_mfuSubsampleNumber, m_mfuLastSubsampleNumber, mfuDataLen, m_mfuSubsampleDataSize, m_pDataSubSumpleTempLength);
			}

			m_iGetStatus = 0;

			m_FileComplete = 1;
		}

	}
	else {
		//error
	}

	//ファイルに保存
	if (m_FileComplete) {

		//m_FileComplete = 0;

		if (m_mfuSubsampleDataSize != m_pDataSubSumpleTempLength) {
			//警告
		}

#if MMT_MEMORY_INTERFACE
		if (m_mfuSubsampleNumber == 0) {
			if (m_pDataSubSumpleData0) {
				delete[] m_pDataSubSumpleData0;
				m_pDataSubSumpleData0 = NULL;
				m_pDataSubSumpleLength0 = 0;
			}
			if (m_pDataSubSumpleData0 == NULL) {
				m_pDataSubSumpleData0 = new unsigned char[m_pDataSubSumpleTempLength];
				if (m_pDataSubSumpleData0 == NULL) { return -1; }
				memset(m_pDataSubSumpleData0, NULL, m_pDataSubSumpleTempLength);
				memcpy(m_pDataSubSumpleData0, m_pDataSubSumpleTemp, m_pDataSubSumpleTempLength);
				m_pDataSubSumpleLength0 = m_pDataSubSumpleTempLength;
			}
		}
		else if (m_mfuSubsampleNumber == 1) {
			if (m_pDataSubSumpleData1) {
				delete[] m_pDataSubSumpleData1;
				m_pDataSubSumpleData1 = NULL;
				m_pDataSubSumpleLength1 = 0;
			}
			if (m_pDataSubSumpleData1 == NULL) {
				m_pDataSubSumpleData1 = new unsigned char[m_pDataSubSumpleTempLength];
				if (m_pDataSubSumpleData1 == NULL) { return -1; }
				memset(m_pDataSubSumpleData1, NULL, m_pDataSubSumpleTempLength);
				memcpy(m_pDataSubSumpleData1, m_pDataSubSumpleTemp, m_pDataSubSumpleTempLength);
				m_pDataSubSumpleLength1 = m_pDataSubSumpleTempLength;
			}
		}
		else if (m_mfuSubsampleNumber == 2) {
			if (m_pDataSubSumpleData2) {
				delete[] m_pDataSubSumpleData2;
				m_pDataSubSumpleData2 = NULL;
				m_pDataSubSumpleLength2 = 0;
			}
			if (m_pDataSubSumpleData2 == NULL) {
				m_pDataSubSumpleData2 = new unsigned char[m_pDataSubSumpleTempLength];
				if (m_pDataSubSumpleData2 == NULL) { return -1; }
				memset(m_pDataSubSumpleData2, NULL, m_pDataSubSumpleTempLength);
				memcpy(m_pDataSubSumpleData2, m_pDataSubSumpleTemp, m_pDataSubSumpleTempLength);
				m_pDataSubSumpleLength2 = m_pDataSubSumpleTempLength;
			}
		}
		else if (m_mfuSubsampleNumber == 3) {
			if (m_pDataSubSumpleData3) {
				delete[] m_pDataSubSumpleData3;
				m_pDataSubSumpleData3 = NULL;
				m_pDataSubSumpleLength3 = 0;
			}
			if (m_pDataSubSumpleData3 == NULL) {
				m_pDataSubSumpleData3 = new unsigned char[m_pDataSubSumpleTempLength];
				if (m_pDataSubSumpleData3 == NULL) { return -1; }
				memset(m_pDataSubSumpleData3, NULL, m_pDataSubSumpleTempLength);
				memcpy(m_pDataSubSumpleData3, m_pDataSubSumpleTemp, m_pDataSubSumpleTempLength);
				m_pDataSubSumpleLength3 = m_pDataSubSumpleTempLength;
			}
		}
		else if (m_mfuSubsampleNumber == 4) {
			if (m_pDataSubSumpleData4) {
				delete[] m_pDataSubSumpleData4;
				m_pDataSubSumpleData4 = NULL;
				m_pDataSubSumpleLength4 = 0;
			}
			if (m_pDataSubSumpleData4 == NULL) {
				m_pDataSubSumpleData4 = new unsigned char[m_pDataSubSumpleTempLength];
				if (m_pDataSubSumpleData4 == NULL) { return -1; }
				memset(m_pDataSubSumpleData4, NULL, m_pDataSubSumpleTempLength);
				memcpy(m_pDataSubSumpleData4, m_pDataSubSumpleTemp, m_pDataSubSumpleTempLength);
				m_pDataSubSumpleLength4 = m_pDataSubSumpleTempLength;
			}
		}
		else {
			return -1;
		}

		//全てのサブサンプルが終了
		if (m_mfuSubsampleNumber == m_mfuLastSubsampleNumber)
		{
			if (m_LogLevel) {
				APPLOG("[MAN],INF,%d,MmtCapProcessManager,CaptureMmtPacket(),[MMT] PID=0x%04X(%d) #LN LastNumber=%d", m_id, packetId, packetId, m_mfuLastSubsampleNumber);
			}

			m_mfuSubsampleNumber = 0;

			//ZIP作成
			std::string sCaptionCapFileame = "page.ttml";
			std::string sCaptionDatFileame1 = "1";
			std::string sCaptionDatFileame2 = "2";
			std::string sCaptionDatFileame3 = "3";
			std::string sCaptionDatFileame4 = "4";
			int iAddFileCount = 0;
			int iZipRt = 0;
			int iZipAddSize = 512;
			///////////////////////////////////
			// 初期化
			///////////////////////////////////
			fileCompInit();
			///////////////////////////////////
			// ZIPするファイルを追加
			///////////////////////////////////
			unsigned char* pZipDst = NULL;
			int iZipSize = 0;
			if (m_pDataSubSumpleLength0 <= 0) {
				if (m_LogLevel) {
					APPLOG("[MAN],WAR,%d,MmtCapProcessManager,CaptureMmtPacket(),[MMT] PID=0x%04X(%d) #LN SubSamoleNumber=%d Length=%d", m_id, packetId, packetId, m_mfuLastSubsampleNumber, m_pDataSubSumpleLength0);
				}
				return 0;
			}

			//if (m_pDataSubSumpleLength0 > 0) {

			//TTMLファイル
			if (m_pDataSubSumpleLength0 > 0) {
				fileCompAdd(sCaptionCapFileame.c_str(), (int)sCaptionCapFileame.size(), m_pDataSubSumpleData0, m_pDataSubSumpleLength0);
				iZipAddSize += m_pDataSubSumpleLength0;
				iZipAddSize += 256;
				iAddFileCount++;
			}
			//1ファイル
			if (m_pDataSubSumpleLength1 > 0) {
				fileCompAdd(sCaptionDatFileame1.c_str(), (int)sCaptionDatFileame1.size(), m_pDataSubSumpleData1, m_pDataSubSumpleLength1);
				iZipAddSize += m_pDataSubSumpleLength1;
				iZipAddSize += 256;
				iAddFileCount++;
			}
			//2ファイル
			if (m_pDataSubSumpleLength2 > 0) {
				fileCompAdd(sCaptionDatFileame2.c_str(), (int)sCaptionDatFileame2.size(), m_pDataSubSumpleData2, m_pDataSubSumpleLength2);
				iZipAddSize += m_pDataSubSumpleLength2;
				iZipAddSize += 256;
				iAddFileCount++;
			}
			//3ファイル
			if (m_pDataSubSumpleLength3 > 0) {
				fileCompAdd(sCaptionDatFileame3.c_str(), (int)sCaptionDatFileame3.size(), m_pDataSubSumpleData3, m_pDataSubSumpleLength3);
				iZipAddSize += m_pDataSubSumpleLength3;
				iZipAddSize += 256;
				iAddFileCount++;
			}
			//4ファイル
			if (m_pDataSubSumpleLength4 > 0) {
				fileCompAdd(sCaptionDatFileame4.c_str(), (int)sCaptionDatFileame4.size(), m_pDataSubSumpleData4, m_pDataSubSumpleLength4);
				iZipAddSize += m_pDataSubSumpleLength4;
				iZipAddSize += 256;
				iAddFileCount++;
			}

			///////////////////////////////////
			// ZIPのアーカイブ化
			///////////////////////////////////
			iZipSize = iZipAddSize;
			pZipDst = new unsigned char[iZipSize];
			if (pZipDst) {
				memset(pZipDst, NULL, iZipSize);
				iZipRt = fileCompExec(pZipDst, iZipSize, 0);
				if (iZipRt < 0) {
					//ERROR
					return -1;
				}
			}

			//字幕用内部インタフェース（入力＆出力）
			int ret = 0;

			//実行中ではない場合
			if (m_busy == 0) {

				printf("[INF] ApiCaptionUpdate(%d) IN File = %d, OUT File = %d\n", m_number, iZipSize, m_out_inteface_size);

				int capType = 0;
				int capInterval = 0;
				int capDuration = 0;
				int capTtmlBegin = 0;
				int capTtmlEnd = 0;
				int animationOpacity = -1;

				//ClearCsreenを解除
				m_iClearTimeMsec = 0;
				m_iClearTimeCount = 0;

				//実行中
				m_busy = 1;

				ret = ApiCaptionUpdate(m_number, pZipDst, iZipSize, m_pOutInteface, m_out_inteface_size, m_OutputImageFileType, &capType, &capInterval, &capDuration, &capTtmlBegin, &capTtmlEnd, animationOpacity);

				//実行終了
				m_busy = 0;
				if (m_LogLevel&&MMT_DEBUG_LOG) {
					APPLOG("[MAN],DBG,%d,MmtCapProcessManager,CaptureMmtPacket(),[MMT] PID=0x%04X(%d) #ApiCaptionUpdate() duration=%d[msec]", m_id, packetId, packetId, capDuration);
				}

				if (ret < 0) {
					printf("[ERR] ApiCaptionUpdate(%d) = %d\n", m_number, ret);
					if (m_LogLevel) {
						APPLOG("[MAN],ERR,%d,MmtCapProcessManager,CaptureMmtPacket(),[MMT] PID=0x%04X(%d) #ApiCaptionUpdate(%d) = %d", m_id, packetId, packetId, m_number, ret);
					}
					delete[] pZipDst;
					return -1;
				}

				//サイズ
				m_out_inteface_size = ret;

				//消去条件
				if (capDuration > 0) {
					//ret = SetClearControl(capInterval);
					m_iClearTimeMsec = capDuration;
					if (m_LogLevel) {
						APPLOG("[MAN],INF,%d,MmtCapProcessManager,CaptureMmtPacket(),[MMT] PID=0x%04X(%d) #ApiCaptionUpdate() duration=%d[msec]", m_id, packetId, packetId, capDuration);
					}
				}
				else {
					//CancelClearControl();
					m_iClearTimeMsec = 0;
				}

				//ZIPクリア
				if (pZipDst) {
					delete[] pZipDst;
					pZipDst = NULL;
					iZipSize = 0;
				}

				if (m_LogLevel) {
					APPLOG("[MAN],INF,%d,MmtCapProcessManager,CaptureMmtPacket(),[MMT] PID=0x%04X(%d) #OUT ApiCaptionUpdate(NUMBER=%d,IN-SIZE=%d,IMG-TYPE=%d)", m_id, packetId, packetId, m_number, iZipSize, m_OutputImageFileType);
				}

				//出力チェック
				if (m_pOutputPointer == NULL) {
					printf("[ERR] m_pOutputPointer = NULL\n");
					return -1;
				}
				//出力コピー
				memcpy(m_pOutputPointer, m_pOutInteface, m_out_inteface_size);


				if (MMT_INTERFACE_SAVE) {
					std::string sPageZipTemp = m_sInputTempPageZipFile;
					//if (0) {
					FILE *fpt = NULL;
					std::string saveName = sPageZipTemp + ".argb";
					//saveName += Conv::IntToDecStr(m_output_count);
					//saveName += ".argb";
					if (m_OutputImageFileType == 0) {
						saveName += Conv::IntToDecStr(m_output_count);
						saveName += ".argb";
					}
					else if (m_OutputImageFileType == 1) {
						saveName += Conv::IntToDecStr(m_output_count);
						saveName += ".png";
					}
					else if (m_OutputImageFileType == 2) {
						saveName += Conv::IntToDecStr(m_output_count);
						saveName += ".bmp";
					}
					fpt = fopen(saveName.c_str(), "w+b");
					if (fpt == NULL) {
						return -1;
					}
					fseek(fpt, 0, SEEK_SET);
					fwrite(m_pOutInteface, m_out_inteface_size, 1, fpt);
					fclose(fpt);
					m_Time.Now(); COUT("%s [MMT] ARGB = %s", m_Time.GetHHMMSSFFF().c_str(), saveName.c_str());

					if (m_LogLevel) {
						APPLOG("[MAN],INF,%d,MmtCapProcessManager,CaptureMmtPacket(),[MMT] PID=0x%04X(%d) #SAVE=%s", m_id, packetId, packetId, saveName.c_str());
					}
				}

				//通知を指示
				m_DataTriggerFlag = 1;
				if (m_LogLevel) {
					APPLOG("[MAN],INF,%d,MmtCapProcessManager,CaptureMmtPacket(),[MMT] PID=0x%04X(%d) DataTriggerFlag=%d", m_id, packetId, packetId, m_DataTriggerFlag);
				}

			}
			//実行中の場合飛ばす
			else {
				if (m_LogLevel) {
					APPLOG("[MAN],WAR,%d,MmtCapProcessManager,CaptureMmtPacket(),[MMT] PID=0x%04X(%d) #ApiCaptionUpdate(%d) can not Execute", m_id, packetId, packetId, m_number);
				}
				if (pZipDst) {
					delete[] pZipDst;
					pZipDst = NULL;
					iZipSize = 0;
				}
				return 0;
			}

			//ページ番号を更新
			m_output_count++;

		}
#endif

	}

	m_Time.Now(); COUT("%s [MMT] PID=0x%04X(%d) #PROC END ===========================", m_Time.GetHHMMSSFFF().c_str(), packetId, packetId);
	if (m_LogLevel) {
		APPLOG("[MAN],INF,%d,MmtCapProcessManager,CaptureMmtPacket(),[MMT] PID=0x%04X(%d) #PROC END ================", m_id, packetId, packetId);
	}

	return 0;
}

int MmtCapProcessManager::UpdateClearScreen() {

	if (m_ClearTtmlFlag == 0) {
		int ret = 0;
		FILE* fp = NULL;
		struct stat stbuf;
		//int fd = 0;
		int file_size = 0;
		int read_size = 0;
		unsigned char* pFileBuffer = NULL;

		fp = fopen(m_ClearTtml.c_str(), "rb");
		if (fp == NULL) {
			//printf("[ERR] Zip File None File=%s\n", m_ClearTtml.c_str());
			if (m_LogLevel) {
				APPLOG("[MAN],ERR,%d,MmtCapProcessManager,UpdateClearScreen(),[MMT] Clear Ttml File None File=%s", m_id, m_ClearTtml.c_str());
			}
			return -1;
		}
		fstat(_fileno(fp), &stbuf);
		file_size = stbuf.st_size;
		if (file_size <= 0) {
			fclose(fp);
			//printf("[ERR] Zip File Size = %d\n", file_size);
			if (m_LogLevel) {
				APPLOG("[MAN],ERR,%d,MmtCapProcessManager,UpdateClearScreen(),[MMT] Clear Ttml File Size=%d", m_id, file_size);
			}
			return -1;
		}
		//printf("[INF] Zip File Size = %d\n", file_size);
		if (m_LogLevel) {
			APPLOG("[MAN],INF,%d,MmtCapProcessManager,UpdateClearScreen(),[MMT] Clear Ttml File Size=%d Name=%s", m_id, file_size, m_ClearTtml.c_str());
		}

		pFileBuffer = new unsigned char[file_size];
		if (pFileBuffer == NULL) {
			fclose(fp);
			//printf("[ERR] Zip File Buffer New\n");
			if (m_LogLevel) {
				APPLOG("[MAN],ERR,%d,MmtCapProcessManager,UpdateClearScreen(),[MMT] Clear Ttml File New Error", m_id);
			}
			return -1;
		}
		memset(pFileBuffer, NULL, file_size);

		fseek(fp, 0, SEEK_SET);

		read_size = (int)fread(pFileBuffer, 1, file_size, fp);
		if (read_size != file_size) {
			//printf("[ERR] Zip File Read Size = %d, File Size = %d\n", read_size, file_size);
			if (m_LogLevel) {
				APPLOG("[MAN],ERR,%d,MmtCapProcessManager,UpdateClearScreen(),[MMT] Clear Ttml File Read Size=%d", m_id, read_size);
			}
			delete[] pFileBuffer;
			fclose(fp);
			return -1;
		}

		fclose(fp);

		//CancelClearControl();
		//m_iClearTimeMsec = 0;

		m_busy = 1; //実行中
		int capType = 0;
		int capInterval = 0;
		int capDuration = 0;
		int capTtmlBegin = 0;
		int capTtmlEnd = 0;
		int animationOpacity = -1;

		ret = ApiCaptionUpdate(m_number, pFileBuffer, file_size, m_pOutInteface, m_out_inteface_size, m_OutputImageFileType, &capType, &capInterval, &capDuration, &capTtmlBegin, &capTtmlEnd, animationOpacity);
		if (ret < 0) {
			if (1) {
				APPLOG("[MAN],DBG,%d,MmtCapProcessManager,UpdateClearScreen(),[MMT] #ApiCaptionUpdate() duration=%d[msec] return=%d", m_id, capDuration, ret);
			}
		}
		m_busy = 0; //実行終了
		if (m_LogLevel) {
			APPLOG("[MAN],INF,%d,MmtCapProcessManager,UpdateClearScreen(),[MMT] #ApiCaptionUpdate() duration=%d[msec]", m_id, capDuration);
		}
		if (ret < 0) {
			//printf("[ERR] ApiCaptionUpdate(%d) = %d\n", m_number, ret);
			if (m_LogLevel) {
				APPLOG("[MAN],ERR,%d,MmtCapProcessManager,CaptureMmtPacket(),[MMT] #ApiCaptionUpdate(%d) = %d", m_id, m_number, ret);
			}
			delete[] pFileBuffer;
			return -1;
		}
		m_out_inteface_size = ret;

		//cs copy
		memcpy(m_pOutputClearPointer, m_pOutInteface, m_out_inteface_size);
		if (m_LogLevel) {
			APPLOG("[MAN],INF,%d,MmtCapProcessManager,UpdateClearScreen(),[MMT] #COPY CS m_pOutputClearPointer size=%d flag=%d", m_id, m_out_inteface_size, m_ClearTtmlFlag);
		}
		m_output_clear_size = m_out_inteface_size;

		//output
		memcpy(m_pOutputPointer, m_pOutInteface, m_out_inteface_size);
		m_ClearTtmlFlag = 1;
	}
	else {
		//cs -> output
		memcpy(m_pOutputPointer, m_pOutputClearPointer, m_output_clear_size);
		if (m_LogLevel) {
			APPLOG("[MAN],INF,%d,MmtCapProcessManager,UpdateClearScreen(),[MMT] #COPY CS m_pOutputClearPointer size=%d flag=%d", m_id, m_out_inteface_size, m_ClearTtmlFlag);
		}
	}

	//m_DataTriggerFlag = 1;

	return 0;
}
