/*
//#######################################################################
//## MmtCapProcessManagerApi.cpp
//#######################################################################
*/

//**************************************************
// include
//**************************************************
#include <stdio.h>
#include <Util/Path.h>
#include <Util/Conv.h>
#include <App/AppLogger.h>
#include "MmtCapProcessManagerApi.h"
#include "MmtCapProcessManager.h"

//**************************************************
// define
//**************************************************
//VERSION
#define MMTLIB_TYPE_CAPTION_BS_VERSION		1000
#define MMTLIB_TYPE_CAPTION_BS_MAXCOUNT		12
#define MPM_OUTPUT_PUTIMAGE 1
#define MMT_PROCESS_INTERFACE_DEBUG		1
#define MMTLIB_INITIAL_NAME	"MmtCapProcessManager.ini"
#define MMTLIB_INITIAL_FILE	"C:\\WORK\\app\\MmtCapProcessManager.ini"
#define MMTLIB_LOG_PATH		"C:\\WORK\\app\\Log\\MmtCap"
#define MMT_API_DEBUG_LOG 0


//**************************************************
// global parameter
//**************************************************
MmtCapProcessManager *gObjectMmtCaption[MMTLIB_TYPE_CAPTION_BS_MAXCOUNT];
int gObjectMmtCount = 0;
int gObjectMmtCaptionCount = 0;
std::string gIniMmtFilename = MMTLIB_INITIAL_NAME;
int gObjectMmtLogLevel = 1;
std::string gObjectMmtLogPath = MMTLIB_LOG_PATH;
PROCESS_INFORMATION gNodejsPi;

int gObjectMmtCapCreateFlag = 0;
int gObjectMmtCapInitialSettingFlag = 0;


//**************************************************
// function
//**************************************************
MmtCapProcessManager *ManFuncGetObject(int type, int number)
{
	MmtCapProcessManager *pObject = NULL;

	if (type == MMTLIB_TYPE_CAPTION_BS) {
		if (number < 1) {
			return NULL;
		}
		if (number > MMTLIB_TYPE_CAPTION_BS_MAXCOUNT) {
			return NULL;
		}
		pObject = gObjectMmtCaption[number-1];
	}
	else { pObject = NULL; }

#if MMT_PROCESS_INTERFACE_DEBUG
	printf(" ManFuncGetObject()                 :type=%d munber=%d pObject=%p \n", type, number, pObject);
#endif

	return pObject;
}

MmtCapProcessManager *ManFuncGetObjectFromId(int id)
{
	MmtCapProcessManager *pObject = NULL;

	int type = (id >> 12) - 1;
	int number = id & 0x000F;

	if (type == MMTLIB_TYPE_CAPTION_BS) {
		if (number < 1) {
			return NULL;
		}
		if (number > MMTLIB_TYPE_CAPTION_BS_MAXCOUNT) {
			return NULL;
		}
		pObject = gObjectMmtCaption[number - 1];
	}
	else { pObject = NULL; }

#if MMT_PROCESS_INTERFACE_DEBUG
	printf(" ManFuncGetObjectFromId()           :id=%d pObject=%p \n", id, pObject);
#endif

	return pObject;
}

int ManFuncGetInstanceId(int type, int number)
{
	int iInstanceId = 0;

	iInstanceId = (type+1) * 256 * 16 + number;

#if MMT_PROCESS_INTERFACE_DEBUG
	printf(" ManFuncGetInstanceId()             :type=%d munber=%d id=%d(0x%08X) \n", type, number, iInstanceId, (unsigned int)iInstanceId);
#endif

	return iInstanceId;
}

int ManFuncGetTypeFromId(int id)
{
	int iType = 0;
	if (     id == 0x1001) { iType = MMTLIB_TYPE_CAPTION_BS; }
	else if (id == 0x1002) { iType = MMTLIB_TYPE_CAPTION_BS; }
	else if (id == 0x1003) { iType = MMTLIB_TYPE_CAPTION_BS; }
	else if (id == 0x1004) { iType = MMTLIB_TYPE_CAPTION_BS; }
	else if (id == 0x2001) { iType = MMTLIB_TYPE_MTMEDIA_BS; }
	else if (id == 0x2002) { iType = MMTLIB_TYPE_MTMEDIA_BS; }
	else if (id == 0x2003) { iType = MMTLIB_TYPE_MTMEDIA_BS; }
	else if (id == 0x2004) { iType = MMTLIB_TYPE_MTMEDIA_BS; }
	else { iType = -1; }

#if MMT_PROCESS_INTERFACE_DEBUG
	printf(" ManFuncGetTypeFromId()             :id=%d type=%d \n", id, iType);
#endif

	return iType;
}

int ManFuncInitialSetting(int iInstanceId)
{
	std::string m_INI_FILENAME;
	std::string sGroupName;
	std::string sParamName;
	std::string sTemp;
	char sBuff[64];
	int iTemp;

	//INIファイル
	m_INI_FILENAME = gIniMmtFilename;

	if (Path::IsExist(m_INI_FILENAME) == 0) {
		printf("[API],ERR,%d,MmtCapProcessManagerApi,ManFuncInitialSetting(),Ini File Error Path=%s", iInstanceId, m_INI_FILENAME.c_str());
		return MMTLIB_ERROR;
	}

	//[System]
	sGroupName = "System";

	//LogLevel
	sParamName = "Log";
	iTemp = GetPrivateProfileIntA((LPCTSTR)sGroupName.c_str(), (LPCTSTR)sParamName.c_str(), -1, (LPCTSTR)m_INI_FILENAME.c_str());
	if (iTemp == 1) {
		gObjectMmtLogLevel = 1;
	}
	else {
		gObjectMmtLogLevel = 0;
	}

	//m_LogPath
	memset(sBuff, NULL, 64);
	sParamName = "LogPath";
	iTemp = ::GetPrivateProfileStringA((LPCTSTR)sGroupName.c_str(), (LPCTSTR)sParamName.c_str(), "", (LPSTR)sBuff, 64, (LPCTSTR)m_INI_FILENAME.c_str());
	sTemp = sBuff;
	if (Path::IsDir(sTemp) == 1) {
		gObjectMmtLogPath = sTemp;
	}
	else {
		Path::MakeDir(sTemp);

		if (Path::IsDir(sTemp) == 1) {
			gObjectMmtLogPath = sTemp;
		}
		else {
			printf("[API],ERR,%d,MmtCapProcessManagerApi,ManFuncInitialSetting(),Log Path Error Path=%s", iInstanceId, sTemp.c_str());
			if (gObjectMmtLogLevel == 1) {
				gObjectMmtLogPath = MMTLIB_LOG_PATH;
				if (Path::IsDir(gObjectMmtLogPath) == 0) {
					gObjectMmtLogLevel = 0;
					//return TSSLIB_ERROR;
				}
			}
		}
	}

	//LogSetting
	AppLogger::Init(gObjectMmtLogPath, "DllMmtCap");

	if (gObjectMmtLogLevel) {
		APPLOG("[API],INF,%d,MmtCapProcessManagerApi,ManFuncInitialSetting(),Log Level=%d", iInstanceId, gObjectMmtLogLevel);
	}

	if (gObjectMmtLogLevel) {
		APPLOG("[API],INF,%d,MmtCapProcessManagerApi,ManFuncInitialSetting(),Log Path =%s", iInstanceId, gObjectMmtLogPath.c_str());
	}

	return 0;
}

//--------------------------------------------------
// MmtCapManApiGetVersion()
//--------------------------------------------------
int MmtCapManApiGetVersion()
{
	int version=0;
	int type = MMTLIB_TYPE_CAPTION_BS;
	version = MMTLIB_TYPE_CAPTION_BS_VERSION;

	if (gObjectMmtLogLevel) {
		APPLOG("[API],INF,%d,TssProcessManagerApi,MmtCapManApiGetVersion(),Type=%d Version=%d", 0, type, version);
	}

#if MMT_PROCESS_INTERFACE_DEBUG
	printf(" ManApiGetVersion()                 :type=%d vresion=%d\n", type, version);
#endif
	return version;
}

//--------------------------------------------------
// MmtCapManApiCreateInstance()
//--------------------------------------------------
int MmtCapManApiCreateInstance(
	int resolution
){
	int iObjectCount=-1;
	MmtCapProcessManager *pObject = NULL;
	int iCount = 0;
	int iInstanceId = 0;
	int ret = 0;
	int iWaiteCount = 0;
	int type = MMTLIB_TYPE_CAPTION_BS;

	if (gObjectMmtCapCreateFlag) {
		do {
			if (gObjectMmtCapCreateFlag == 0) {
				break;
			}
			iWaiteCount++;
			Sleep(100);
			if (iWaiteCount > 100) {
				gObjectMmtCapCreateFlag = 0;
				break;
			}
		} while (1);
	}
	gObjectMmtCapCreateFlag = 1;

	if (type == MMTLIB_TYPE_CAPTION_BS) {
		iCount = gObjectMmtCaptionCount;
	}
	else {
		gObjectMmtCapCreateFlag = 0;
		return MMTLIB_ERROR;
	}

	//init
	if (iCount == 0) {
		if (type == MMTLIB_TYPE_CAPTION_BS) {
			for (int i = 0; i < MMTLIB_TYPE_CAPTION_BS_MAXCOUNT; i++) {
				gObjectMmtCaption[i] = NULL;
			}
		}
	}

	if (type == MMTLIB_TYPE_CAPTION_BS) {
		if (iCount < MMTLIB_TYPE_CAPTION_BS_MAXCOUNT) {
		}
		else {
			gObjectMmtCapCreateFlag = 0;
			return MMTLIB_ERROR;
		}
	}
	else {
		gObjectMmtCapCreateFlag = 0;
		return MMTLIB_ERROR;
	}

	if (type == MMTLIB_TYPE_CAPTION_BS) {

		if (gObjectMmtCapInitialSettingFlag == 0) {

			//IniFile
			gIniMmtFilename = MMTLIB_INITIAL_NAME;

			if (Path::IsExist(gIniMmtFilename) == 0) {
				printf("[API],WAR,%d,MmtCapProcessManagerApi,TssCapProcessManager(),Ini File Error Path=%s", iInstanceId, gIniMmtFilename.c_str());

				gIniMmtFilename = MMTLIB_INITIAL_FILE;
				if (Path::IsExist(gIniMmtFilename) == 0) {
					printf("[API],ERR,%d,MmtCapProcessManagerApi,TssCapProcessManager(),Ini File Error Path=%s", iInstanceId, gIniMmtFilename.c_str());
					return MMTLIB_ERROR;
				}
			}

			//LogPath/LogLevel
			iInstanceId = ManFuncGetInstanceId(type, (gObjectMmtCaptionCount + 1));

			ret = ManFuncInitialSetting(iInstanceId);
			if (ret < 0) {
				printf("[API],ERR,%d,MmtCapProcessManagerApi,TssCapProcessManager(),ManFuncInitialSetting()=%d", iInstanceId, ret);
				return MMTLIB_ERROR;
			}
			if (gObjectMmtLogLevel) {
				APPLOG("[API/DLL],Level(INF/WAR/DBG),id,Param1,Param2,FreeText");
			}
			if (gObjectMmtLogLevel) {
				APPLOG("[API],INF,%d,MmtCapProcessManagerApi,TssCapManApiCreateInstance(),Start Instance #################################################", iInstanceId);
				APPLOG("[API],INF,%d,MmtCapProcessManagerApi,TssCapManApiCreateInstance(%d, %d) Log=%d", iInstanceId, type, resolution, gObjectMmtLogLevel);
			}
		}

		pObject = new MmtCapProcessManager(gIniMmtFilename, gObjectMmtLogPath, gObjectMmtLogLevel);
		if (pObject == NULL) {
			gObjectMmtCapCreateFlag = 0;
			return MMTLIB_ERROR;
		}
		if (gObjectMmtLogLevel) {
			APPLOG("[API],INF,%d,MmtCapProcessManagerApi,new MmtCapProcessManager(%s, %s, %d)", iInstanceId, gIniMmtFilename.c_str(), gObjectMmtLogPath.c_str(), gObjectMmtLogLevel);
		}

		gObjectMmtCount++;
	}
	else {
		gObjectMmtCapCreateFlag = 0;
		return MMTLIB_ERROR;
	}

	if (type == MMTLIB_TYPE_CAPTION_BS) {
		gObjectMmtCaptionCount++;
		iCount = gObjectMmtCaptionCount;
	}
	else {
		gObjectMmtCapCreateFlag = 0;
		return MMTLIB_ERROR;
	}

	//SetPreviewLog
	if (gObjectMmtCapInitialSettingFlag == 0) {
#if MMT_PROCESS_MMCTRL
		//PreviewLogSetting
		pObject->InitPreview(gObjectMmtLogPath);
#endif
	}

	//Version
	MmtCapManApiGetVersion();

#if MMT_PROCESS_INTERFACE_DEBUG
	printf(" MmtCapManApiCreateInstance()             :Create(%d,%d) START\n", iCount, type);
	if (gObjectMmtLogLevel) { 
		APPLOG("[API],INF,%d,MmtCapProcessManagerApi,MmtCapManApiCreateInstance(),Create(%d,%d)", iInstanceId, iCount, type);
	}
#endif

#if MMT_PROCESS_MMCTRL
	if (type == MMTLIB_TYPE_CAPTION_BS) {
		ret = pObject->Create(iCount, type, resolution);
	}
	else {
		gObjectMmtCapCreateFlag = 0;
		return MMTLIB_ERROR;
	}
#endif

#if MMT_PROCESS_INTERFACE_DEBUG
	printf(" MmtCapManApiCreateInstance()             :Create(%d,%d) END\n", iCount, type);
	if (gObjectMmtLogLevel) { 
		APPLOG("[API],INF,%d,MmtCapProcessManagerApi,MmtCapManApiCreateInstance(),Create(%d,%d) END", iInstanceId, iCount, type);
	}
#endif

	if (type == MMTLIB_TYPE_CAPTION_BS) {

		iInstanceId = ManFuncGetInstanceId(type, iCount);

		pObject->SetId(iInstanceId);
		if (gObjectMmtLogLevel) {
			APPLOG("[API],INF,%d,MmtCapProcessManagerApi,MmtCapManApiCreateInstance(),SetId(%d)", iInstanceId, iInstanceId);
		}

		pObject->SetType(type);
		if (gObjectMmtLogLevel) {
			APPLOG("[API],INF,%d,MmtCapProcessManagerApi,MmtCapManApiCreateInstance(),SetType(%d)", iInstanceId, type);
		}

		pObject->SetNumber(iCount);
		if (gObjectMmtLogLevel) {
			APPLOG("[API],INF,%d,MmtCapProcessManagerApi,MmtCapManApiCreateInstance(),SetNumber(%d)", iInstanceId, iCount);
		}
	}
	else {
		gObjectMmtCapCreateFlag = 0;
		return MMTLIB_ERROR;
	}

#if MMT_PROCESS_INTERFACE_DEBUG
	printf(" MmtCapManApiCreateInstance()             :type=%d munber=%d id=%d(0x%08X) \n", type, iCount, iInstanceId, (unsigned int)iInstanceId);
	if (gObjectMmtLogLevel) { 
		APPLOG("[API],INF,%d,MmtCapProcessManagerApi,MmtCapManApiCreateInstance(),type=%d munber=%d id=%d(0x%08X)", iInstanceId, type, iCount, iInstanceId, (unsigned int)iInstanceId);
	}
#endif

#if MMT_PROCESS_INTERFACE_DEBUG
	if (gObjectMmtLogLevel) {
		APPLOG("[API],INF,%d,MmtCapProcessManagerApi,MmtCapManApiCreateInstance(),index=%d", iInstanceId, (iCount - 1));
		if (type == MMTLIB_TYPE_CAPTION_BS) {
			APPLOG("[API],INF,%d,MmtCapProcessManagerApi,MmtCapManApiCreateInstance(),gObjectTssCaption(%d)=%p", iInstanceId, (iCount - 1), gObjectMmtCaption[iCount - 1]);
		}
	}
#endif

	if (type == MMTLIB_TYPE_CAPTION_BS) {
		gObjectMmtCaption[iCount - 1] = pObject;
	}
	else {
		gObjectMmtCapCreateFlag = 0;
		return MMTLIB_ERROR;
	}

#if MMT_PROCESS_INTERFACE_DEBUG
	if (type == MMTLIB_TYPE_CAPTION_BS) {
		printf(" ManApiCreateInstance()             :pObject=%p \n", pObject);
		if (gObjectMmtLogLevel) {
			APPLOG("[API],INF,%d,MmtCapProcessManagerApi,MmtCapManApiCreateInstance(),pObject=%p", iInstanceId, pObject);
		}
	}
	else {
		gObjectMmtCapCreateFlag = 0;
		return MMTLIB_ERROR;
	}
#endif

	if (gObjectMmtCapInitialSettingFlag == 0) {
		gObjectMmtCapInitialSettingFlag++;
	}

	gObjectMmtCapCreateFlag = 0;

	return iInstanceId;
}

//--------------------------------------------------
// MmtCapManApiDeleteInstance()
//--------------------------------------------------
int MmtCapManApiDeleteInstance(
	int id
){
	MmtCapProcessManager *pObject = NULL;
	int iType = 0;
	int iCount = 0;
	int iNumber = 0;
	int ret = 0;

	pObject = ManFuncGetObjectFromId(id);
	if (pObject == NULL) {
		return MMTLIB_ERROR_INSTANCEID;
	}
	iNumber = pObject->GetNumber();
	iType = pObject->GetType();

#if MMT_PROCESS_INTERFACE_DEBUG
	printf(" MmtCapManApiDeleteInstance()             :Release(%d,%d) START\n", iNumber, iType);
	if (gObjectMmtLogLevel) {
		APPLOG("[API],INF,%d,MmtCapProcessManagerApi,MmtCapManApiDeleteInstance(),Release(%d,%d) START", id, iNumber, iType);
	}
#endif

#if MMT_PROCESS_MMCTRL
	ret = pObject->Release(iNumber, iType);
	if ( ret < 0) {
		return ret;
	}
	if (gObjectMmtCount == 0) {
		pObject->ExitPreview();
	}
	if (pObject) {
		delete pObject;
		pObject = NULL;
	}
#endif

#if MMT_PROCESS_INTERFACE_DEBUG
	printf(" MmtCapManApiDeleteInstance()             :Release(%d,%d) END\n", iNumber, iType);
	if (gObjectMmtLogLevel) {
		APPLOG("[API],INF,%d,MmtCapProcessManagerApi,MmtCapManApiDeleteInstance(),Release(%d,%d) END", id, iNumber, iType);
	}
#endif

	iType = ManFuncGetTypeFromId(id);

	if (iType == MMTLIB_TYPE_CAPTION_BS) {
		gObjectMmtCaptionCount--;
		if (gObjectMmtCaptionCount < 0) { gObjectMmtCaptionCount = 0; }
		iCount = gObjectMmtCaptionCount;
	}

#if MMT_PROCESS_INTERFACE_DEBUG
	printf(" MmtCapManApiDeleteInstance()             :type=%d munber=%d id=%d(0x%08X) \n", iType, iNumber, id, (unsigned int)id);
	if (gObjectMmtLogLevel) {
		APPLOG("[API],INF,%d,MmtCapProcessManagerApi,MmtCapManApiDeleteInstance(),type=%d munber=%d id=%d(0x%08X)", id, iType, iNumber, id, (unsigned int)id);
	}
#endif

	return 0;
}

//--------------------------------------------------
// MmtCapManApiSetParameterInit()
//--------------------------------------------------
int MmtCapManApiSetParameterInit(
	int id,
//	int resolution,
	unsigned long streamid,
	unsigned long serviceid
){
	MmtCapProcessManager *pObject = NULL;
	int iType = 0;

	pObject = ManFuncGetObjectFromId(id);
	if (pObject == NULL) {
		return MMTLIB_ERROR_INSTANCEID;
	}

	//pObject->SetResolution(resolution);
	pObject->SetStreamId(streamid);
	pObject->SetServiceId(serviceid);

#if MMT_PROCESS_INTERFACE_DEBUG
	printf(" MmtCapManApiSetParameterInit()           :id=%d(0x%08X) \n", id,(unsigned int)id);
	printf(" MmtCapManApiSetParameterInit()           :streamid=%d(0x%08X) serviceid=%d(0x%08X) \n", streamid, streamid, (unsigned int)serviceid, (unsigned int)serviceid);
	if (gObjectMmtLogLevel) {
		APPLOG("[API],INF,%d,MmtCapProcessManagerApi,MmtCapManApiSetParameterInit(),id=%d(0x%08X) streamid=%d(0x%08X) serviceid=%d(0x%08X)", id, id, (unsigned int)id, streamid, streamid, (unsigned int)serviceid, (unsigned int)serviceid);
	}
#endif

	return 0;
}

//--------------------------------------------------
// MmtCapManApiSetParameterInputAdd()
//--------------------------------------------------
int MmtCapManApiSetParameterInputAdd(
	int id,
	int pid,
	unsigned long type,
	int tag
){
	int ret = 0;
	MmtCapProcessManager *pObject = NULL;
	int iType = 0;
	int iCount = 0;

	pObject = ManFuncGetObjectFromId(id);
	if (pObject == NULL) {
		return MMTLIB_ERROR_INSTANCEID;
	}

	ret = pObject->SetParameterInputAdd(pid, type, tag);
	if (ret < 0) {
		return ret;
	}

	iCount = pObject->GetParameterInputCount();

#if MMT_PROCESS_INTERFACE_DEBUG
	printf(" MmtCapManApiSetParameterInputAdd()       :id=%d(0x%08X) \n", id,(unsigned int)id);
	printf(" MmtCapManApiSetParameterInputAdd()       :pid=%d type=%d tag=%d \n", pid, type, tag );
	printf(" MmtCapManApiSetParameterInputAdd()       :count=%d \n", iCount);
	if (gObjectMmtLogLevel) {
		APPLOG("[API],INF,%d,MmtCapProcessManagerApi,MmtCapManApiSetParameterInputAdd(),id=%d(0x%08X) pid=%d type=%d(0x%08X) tag=%d count=%d", id, id, (unsigned int)id, pid, type, (unsigned int)type, tag, iCount);
	}
#endif

	return 0;
}

//--------------------------------------------------
// MmtCapManApiSetParameterInputClear()
//--------------------------------------------------
int MmtCapManApiSetParameterInputClear(
	int id
){
	int ret = 0;
	MmtCapProcessManager *pObject = NULL;
	int iType = 0;
	int iCount = 0;

	pObject = ManFuncGetObjectFromId(id);
	if (pObject == NULL) {
		return MMTLIB_ERROR_INSTANCEID;
	}

	ret = pObject->SetParameterInputClear();
	if (ret < 0) {
		return ret;
	}

	iCount = pObject->GetParameterInputCount();

#if MMT_PROCESS_INTERFACE_DEBUG
	printf(" MmtCapManApiSetParameterInputClear()     :id=%d(0x%08X) \n", id,(unsigned int)id);
	printf(" MmtCapManApiSetParameterInputClear()     :count=%d \n", iCount);
	if (gObjectMmtLogLevel) {
		APPLOG("[API],INF,%d,MmtCapProcessManagerApi,MmtCapManApiSetParameterInputClear(),id=%d(0x%08X) count=%d", id, id, (unsigned int)id, iCount);
	}
#endif

	return 0;
}

//--------------------------------------------------
// MmtCapManApiSetParameterOutput()
//--------------------------------------------------
int MmtCapManApiSetParameterOutput(
	int id,
	unsigned char *pointer,
	int size
){
	int ret = 0;
	MmtCapProcessManager *pObject = NULL;
	int iType = 0;

	pObject = ManFuncGetObjectFromId(id);
	if (pObject == NULL) {
		return MMTLIB_ERROR_INSTANCEID;
	}

	ret = pObject->SetParameterOutput(pointer, size);
	if (ret < 0) {
		return ret;
	}

#if MMT_PROCESS_INTERFACE_DEBUG
	printf(" MmtCapManApiSetParameterOutput()         :id=%d(0x%08X) \n", id,(unsigned int)id);
	printf(" MmtCapManApiSetParameterOutput()         :pointer=%p size=%d \n", pointer, size);
	if (gObjectMmtLogLevel) {
		APPLOG("[API],INF,%d,MmtCapProcessManagerApi,MmtCapManApiSetParameterOutput(),id=%d(0x%08X) pointer=%p size=%d", id, id, (unsigned int)id, pointer, size);
	}
#endif

	return 0;
}

//--------------------------------------------------
// MmtCapManApiStart()
//--------------------------------------------------
int MmtCapManApiStart(
	int id
) { 
	int rv = 0;
	MmtCapProcessManager *pObject = NULL;
	int iType = 0;

	pObject = ManFuncGetObjectFromId(id);
	if (pObject == NULL) {
		return MMTLIB_ERROR_INSTANCEID;
	}

#if MMT_PROCESS_INTERFACE_DEBUG
	printf(" MmtCapManApiStart()                      :id=%d(0x%08X) \n", id,(unsigned int)id);
	if (gObjectMmtLogLevel) {
		APPLOG("[API],INF,%d,MmtCapProcessManagerApi,MmtCapManApiStart(),id=%d(0x%08X)", id, id, (unsigned int)id);
	}
#endif

#if MPM_OUTPUT_PUTIMAGE
	pObject->StartThread();
#endif

	//std::string msg = "<EVENT-KEY>40";
	//pObject->SendUdpCommand(msg);
	//printf(" SendUdpCommand()                   :Command=%s \n", msg.c_str());

	return 0;
}

//--------------------------------------------------
// MmtCapManApiPause()
//--------------------------------------------------
#if 0
int MmtCapManApiPause(
	int id,
	int enable
) { 
	int rv = 0;
	MmtCapProcessManager *pObject = NULL;
	int iType = 0;

	pObject = ManFuncGetObjectFromId(id);
	if (pObject == NULL) {
		return MMTLIB_ERROR_INSTANCEID;
	}

	//一時停止解除
	if (enable == 0) {
		//if (pObject->RealtimeStart() < 0) {
		//	return MMTLIB_ERROR;
		//}
		pObject->StartThread();
	}
	//一時停止
	else {
		//if (pObject->RealtimeStop() < 0) {
		//	return MMTLIB_ERROR;
		//}
		pObject->ThreadFlagStop();
		pObject->StopThread();
	}

#if MMT_PROCESS_INTERFACE_DEBUG
	printf(" MmtCapManApiPause()                      :id=%d(0x%08X) enable=%d\n", id, (unsigned int)id, enable);
	if (gObjectMmtLogLevel) {
		APPLOG("[API],INF,%d,MmtCapProcessManagerApi,MmtCapManApiPause(),id=%d(0x%08X) enable=%d", id, id, (unsigned int)id, enable);
	}
#endif

	return 0;
}
#endif

//--------------------------------------------------
// MmtCapManApiStop()
//--------------------------------------------------
int MmtCapManApiStop(
	int id
) {
	int rv = 0;
	MmtCapProcessManager *pObject = NULL;
	int iType = 0;

	pObject = ManFuncGetObjectFromId(id);
	if (pObject == NULL) {
		return MMTLIB_ERROR_INSTANCEID;
	}

#if MMT_PROCESS_INTERFACE_DEBUG
	printf(" MmtCapManApiStop()                       :id=%d(0x%08X)\n", id, (unsigned int)id);
	if (gObjectMmtLogLevel) {
		APPLOG("[API],INF,%d,MmtCapProcessManagerApi,MmtCapManApiStop(),id=%d(0x%08X)", id, id, (unsigned int)id);
	}
#endif

#if MPM_OUTPUT_PUTIMAGE
	pObject->ThreadFlagStop();
	pObject->StopThread();
#endif

	return 0;
}

//--------------------------------------------------
// MmtCapManApiPushNtp()
//--------------------------------------------------
int MmtCapManApiPushNtp(
	int id,
	unsigned long long Ntp
) {
	int rv = 0;
	MmtCapProcessManager *pObject = NULL;
	int iType = 0;

	pObject = ManFuncGetObjectFromId(id);
	if (pObject == NULL) {
		APPLOG("[API],ERR,%d,MmtCapProcessManagerApi,MmtCapManApiPushNtp(),id=%d(0x%08X) #ST NTP=%016X", id, id, (unsigned int)id, Ntp);
		return MMTLIB_ERROR_INSTANCEID;
	}

	if (pObject->SetNtp(Ntp) < 0) {
		return MMTLIB_ERROR;
	}

	if (gObjectMmtLogLevel&&MMT_API_DEBUG_LOG) {
		APPLOG("[API],ERR,%d,MmtCapProcessManagerApi,MmtCapManApiPushNtp(),id=%d(0x%08X) #ED NTP=%016X", id, id, (unsigned int)id, Ntp);
	}

	return 0;
}

//--------------------------------------------------
// MmtCapInputApiPushMmtData()
//--------------------------------------------------
int MmtCapInputApiPushMmtData(
	int id,
	unsigned char *pointer,
	int size
) {
	int rv = 0;
	MmtCapProcessManager *pObject = NULL;
	int iType = 0;

	pObject = ManFuncGetObjectFromId(id);
	if (pObject == NULL) {
		APPLOG("[API],ERR,%d,MmtCapProcessManagerApi,MmtCapInputApiPushMmtData(),ManFuncGetObjectFromId() id=%d(0x%08X)", id, id, (unsigned int)id);
		return MMTLIB_ERROR_INSTANCEID;
	}

	if (pObject->PushMmtData(pointer, size) < 0) {
		return MMTLIB_ERROR;
	}

	if (gObjectMmtLogLevel&&MMT_API_DEBUG_LOG) {
		if (size >= 12) {
			APPLOG("[API],INF,%d,MmtCapProcessManagerApi,MmtCapInputApiPushMmtData(),size=%04d data=%02X%02X %02X%02X %02X%02X%02X%02X %02X%02X%02X%02X", id, size,
				pointer[0], pointer[1], pointer[2], pointer[3], pointer[4], pointer[5], pointer[6], pointer[7], pointer[8], pointer[9], pointer[10], pointer[11]);
		}
	}

	return 0;
}

//--------------------------------------------------
// MmtCapManApiAppendListener()
//--------------------------------------------------
int MmtCapManApiAppendListener(
	int id,
	IMmtCapCaptureListener* pListener
) {
	int rv = 0;
	MmtCapProcessManager *pObject = NULL;
	int iType = 0;
	int iCount = 0;

	pObject = ManFuncGetObjectFromId(id);
	if (pObject == NULL) {
		return MMTLIB_ERROR_INSTANCEID;
	}

	if (pObject->AppendListener(pListener) < 0) {
		return MMTLIB_ERROR;
	}

	iCount = pObject->GetListenerCount();

#if MMT_PROCESS_INTERFACE_DEBUG
	printf(" MmtCapManApiAppendListener()             :id=%d(0x%08X) count=%d \n", id, (unsigned int)id, iCount);
#endif
	
	return 0;
}

//--------------------------------------------------
// MmtCapManApiRemoveListener()
//--------------------------------------------------
int MmtCapManApiRemoveListener(
	int id,
	IMmtCapCaptureListener* pListener
) {
	int rv = 0;
	MmtCapProcessManager *pObject = NULL;
	int iType = 0;
	int iCount = 0;

	pObject = ManFuncGetObjectFromId(id);
	if (pObject == NULL) {
		return MMTLIB_ERROR_INSTANCEID;
	}

	if (pObject->RemoveListener(pListener) < 0) {
		return MMTLIB_ERROR;
	}

	iCount = pObject->GetListenerCount();

#if MMT_PROCESS_INTERFACE_DEBUG
	printf(" MmtCapManApiRemoveListener()             :id=%d(0x%08X) count=%d \n", id, (unsigned int)id, iCount);
#endif

	return 0;
}

