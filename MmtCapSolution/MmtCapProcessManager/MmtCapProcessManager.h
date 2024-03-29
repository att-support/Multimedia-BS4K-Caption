#pragma once

//============================================================
#define MMT_PROCESS_MMCTRL 1
//============================================================

#include <map>
#include <vector>
#include <Util/Thread.h>
#include <Util/SysTime.h>
#include <PacketStructure/MmtpPacket.h>
#include "ClockTime.h"
#include "MmtCapProcessManagerListenerIf.h"

#if MMT_PROCESS_MMCTRL
#include "MmtMultimediaControl.h"
#pragma comment(lib, "Modules/MmtMultimediaControl.lib")
#endif

//#ifdef MMT_CAP_PROCESS_MANAGER_EXPORTS
//
//#define MMT_CAP_PROCESS_MANAGER_API __declspec(dllexport)
//#else
//#define MMT_CAP_PROCESS_MANAGER_API __declspec(dllimport)
//#endif

//TYPE
#define MMTLIBMAN_TYPE_CAPTION	0
#define MMTLIBMAN_TYPE_MTMEDIA	1

//input pid smax count
#define MMT_PROCESS_INPUT_COUNT 8

//class MMT_CAP_PROCESS_MANAGER_API MmtCapProcessManager : public Thread
class MmtCapProcessManager : public Thread
{
private:
	int m_id;
	int m_type;
	int m_number;
	int m_resolution;
	int m_streamid;
	int m_serviceid;
	int m_busy;
	int m_release_flag;

public:
	MmtCapProcessManager(std::string InitFile, std::string LogPath, int iLogLevel=0);
	~MmtCapProcessManager();

public:
	void InitPreview(std::string sLogPath);
	void ExitPreview();
	long Create(long number, long type, int resolution);
	long Release(long number, long type);

public:
	void SetId(int param) {			m_id = param; };
	void SetType(int param) {		m_type = param; };
	void SetNumber(int param) {		m_number = param; };
	void SetResolution(int param);
	void SetStreamId(int param) {	m_streamid = param; };
	void SetServiceId(int param) {	m_serviceid = param; };

	int GetId() { return m_id; };
	int GetType() { return m_type; };
	int GetNumber() { return m_number; };

private:
	int m_CreateFlag;
	int m_ThreadFlag;
	int m_StartFlag;
	int ThreadProc();
	int ThreadInitProc();
	int ThreadTermProc();
	unsigned long long m_ntp;
	ClockTime* m_pClockTime;

public:
	void ThreadFlagStop() { m_ThreadFlag = 0; };

public:
	int SetParameterInputAdd(int pid, unsigned long type, int tag);
	int SetParameterInputClear();
	int SetParameterOutput(unsigned char *pointer, int size);
	int GetParameterInputCount() {	return m_input_count; };

private:
	int m_UpadteOutputFlag;
	int m_DataTriggerFlag;
	int m_iClearTimeMsec;
	int m_iClearTimeCount;
	int m_iClearUpdateFlag;
	int m_iPushMmtCount;
	int m_FileComplete;

public:
	int UpdateClearScreen();
	int PushMmtData(unsigned char *pointer, int size);
	int SetNtp(unsigned long long uNtp);
	int CaptureMmtPacket(MmtpPacket *packet, int i, unsigned long type);

private:
	unsigned char *m_pDataSubSumpleTemp;
	int m_pDataSubSumpleTempLength;

	unsigned char *m_pDataSubSumpleData0;
	int m_pDataSubSumpleLength0;
	unsigned char *m_pDataSubSumpleData1;
	int m_pDataSubSumpleLength1;
	unsigned char *m_pDataSubSumpleData2;
	int m_pDataSubSumpleLength2;
	unsigned char *m_pDataSubSumpleData3;
	int m_pDataSubSumpleLength3;
	unsigned char *m_pDataSubSumpleData4;
	int m_pDataSubSumpleLength4;

	int m_iGetStatus;
	int m_payloadMpuSequenceNumber;
	int m_mfuSubsampleNumber;
	int m_mfuLastSubsampleNumber;
	int m_mfuSubsampleDataType;
	int m_mfuSubsampleDataSize;
	SysTime m_Time;

public:
	int AppendListener(IMmtCapCaptureListener* pListener);	//リスナー登録追加  
	int RemoveListener(IMmtCapCaptureListener* pListener);	//リスナー登録削除
	int GetListenerCount() { return(int) m_vecListener.size(); };

	int EventError(unsigned long ErrorCode);
	int EventUpdateCaptionPlane();

	std::string GetVbsPath() { return m_sVbsPath; };

private:
	std::string m_sNumber;
	//std::string m_sInputDir;
	//std::string m_sInputTriggerFile;
	//std::string m_sInputPageZipFile;
	//std::string m_sInputTempDir;
	//std::string m_sInputTempPageDir;
	//std::string m_sInputTempTriggerFile;
	std::string m_sInputTempPageZipFile;
	//std::string m_sInputTempCommandFile;
	//std::string m_sOutputDir;
	//std::string m_sOutputTriggerFile;
	std::string m_sOutputImageFile;
	int m_OutputImageFileType;
	std::string m_sVbsPath;

public:
	int m_input_pid[MMT_PROCESS_INPUT_COUNT];
	unsigned long m_input_type[MMT_PROCESS_INPUT_COUNT];
	int m_input_tag[MMT_PROCESS_INPUT_COUNT];
	int m_input_count;

private:
	unsigned char *m_pOutputPointer;
	int m_output_size;
	unsigned char *m_pOutInteface;
	int m_out_inteface_size;
	int m_status;
	int m_output_count;
	int m_DemoMode;
	std::map<unsigned char*, unsigned long long> m_fifoCaption;

private:
	typedef std::vector<IMmtCapCaptureListener*> LISTENER_VECTOR;
	LISTENER_VECTOR	m_vecListener;
	std::string m_INI_FILENAME;
	int m_LogLevel;
	std::string m_LogPath;

	std::string m_ClearTtml;
	int m_ClearTtmlFlag;
	unsigned char *m_pOutputClearPointer;
	int m_output_clear_size;
};

