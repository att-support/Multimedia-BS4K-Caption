#if !defined _MMT_CAP_PROC_API_
#define _MMT_CAP_PROC_API_
//#######################################################################
//## MmtCapProcessManagerApi.h
//#######################################################################

//**************************************************
// include
//**************************************************
#include "MmtCapProcessManagerListenerIf.h"

//**************************************************
// define
//**************************************************
//IMPORT/EXPORT
#ifdef MMT_CAP_PROCESS_MANAGER_EXPORTS
#define MMTPROC_API __declspec(dllexport)
#else
#define MMTPROC_API __declspec(dllimport)
#endif
//TYPE
#define MMTLIB_TYPE_CAPTION_BS			   0
#define MMTLIB_TYPE_MTMEDIA_BS			   1
//RESOLUTION
#define MMTLIB_RESOKUTUION_2K			   0
#define MMTLIB_RESOKUTUION_4K			   1
#define MMTLIB_RESOKUTUION_8K			   2
//asset_type
#define TYPEPID_MPU_TTMLCAPTION			0x73747070 //stpp
#define TYPEPID_SEC_MPT_ESSAGE_MPT		0x6D6D7470 //mmtp


//**************************************************
// error code
//**************************************************
#define MMTLIB_OK						   0
#define MMTLIB_ERROR					  -1
#define MMTLIB_ERROR_INSTANCEID			 -10
#define MMTLIB_ERROR_INPUT				-101

//**************************************************
// function
//**************************************************

//--------------------------------------------------
// int MmtCapManApiGetVersion()
// 説明：本DLLのバージョン値(10進数4桁=NXYZ)
// 　　：N=1～9(メインバージョン　 ：開発版は9,リリース版は1から)
// 　　：X=0～9(マイナーバージョン ：開発版は9,リリース版は0から)
// 　　：Y=0～9(ﾊﾞｸﾞﾌｨｯｸｽバージョン：開発版は9,リリース版は0から)
// 　　：Z=0～9(ビルドバージョン　 ：0～9 9を超える場合はZ=0,Y++)
// 引数：なし
// 戻値：int＜バージョン番号＞上記数値
//--------------------------------------------------
MMTPROC_API int MmtCapManApiGetVersion();

//**************************************************
// 生成・破棄
//**************************************************
//--------------------------------------------------
// int MmtCapManApiCreateInstance(int resolution)
// 説明：インスタンスを生成します(種別指定)
// 引数：int resolution
//     ：＜解像度＞(0=2K,1=4K,2=8K)
// 戻値：int＜インスタンスID,正否＞(0=インスタンスID,マイナス値=エラーコード)
//--------------------------------------------------
MMTPROC_API int MmtCapManApiCreateInstance(int resolution);

//--------------------------------------------------
// int MmtCapManApiDeleteInstance(int id)
// 説明：インスタンスを破棄します(インスタンスID指定)
// 引数：int id
//     ：＜インスタンスID＞
// 戻値：int＜正否＞(0=OK,マイナス値=エラーコード)
//--------------------------------------------------
MMTPROC_API int MmtCapManApiDeleteInstance(int id);

//**************************************************
// 設定
//**************************************************
//--------------------------------------------------
// int MmtCapManApiSetParameterInit(int id, int resolution, unsigned long streamid, unsigned long serviceid)
// 説明：初期設定
// 引数：int id
//     ：＜インスタンスID＞
// 引数：unsigned long streamid
//     ：＜ストリームID＞(32bit値)
//     ：同じサービスのデータ放送を複数並べる場合も考慮して、サービスより上位の識別値として設定する。
//	　 ：この値は、上位側で管理する任意のIDを設定する。これにより同じTSの異なる時間帯を複数並べて表示が出来る想定。
// 引数：unsigned long serviceid
//     ：＜サービスID＞(16bit値)
//     ：TSのSID(16bit値)、MMTのMMT_package_id_byteの下位16bit
// 戻値：int＜正否＞(0=OK,マイナス値=エラーコード)
//--------------------------------------------------
MMTPROC_API int MmtCapManApiSetParameterInit(int id, unsigned long streamid, unsigned long serviceid);

//--------------------------------------------------
// int MmtCapManApiSetParameterInputAdd(int id, int pid, unsigned long type, int tag)
// 説明：PID設定(追加方式)
// 引数：int id
//     ：＜インスタンスID＞
//     ：int pid
//     ：＜PID＞入力PID設定（追加方式）必要な数だけ設定する
//     ：unsigned long type
//     ：TYPEPID_MPU_TTMLCAPTION
//     ：TYPEPID_SEC_MPT_ESSAGE_MPT
//     ：＜データタイプ＞(32bit値)
//     ：MPTに書かれているasset_type(32bit)
//     ：int tag
//     ：＜タグID＞(8bit値)
//     ：MPTに含まれるMH-ストリーム識別記述子内のcomponent_tag値
// 戻値：int＜正否＞(0=OK,マイナス値=エラーコード)
//--------------------------------------------------
MMTPROC_API int MmtCapManApiSetParameterInputAdd(int id, int pid, unsigned long type, int tag);

//--------------------------------------------------
// int MmtCapManApiSetParameterInputClear(int id)
// 説明：PID設定(設定のクリア)
// 引数：int id
//     ：＜インスタンスID＞
// 戻値：int＜正否＞(0=OK,マイナス値=エラーコード)
//--------------------------------------------------
MMTPROC_API int MmtCapManApiSetParameterInputClear(int id);

//--------------------------------------------------
// int MmtCapManApiSetParameterOutput(int id, unsigned char *pointer, int size)
// 説明：出力設定(書込用メモリ設定)
// 引数：int id
//     ：＜インスタンスID＞
// 引数：unsigned char *pointer
//     ：＜書込ポインタ＞
//     ：書き込むデータ領域の先頭ポインタ(解像度に以上のバイト数とする)
// 引数：int size
//     ：＜書込サイズ＞
//     ：書き込むデータ領域のサイズ（書き込める最大サイズ）
// 戻値：int＜正否＞(0=OK,マイナス値=エラーコード)
//--------------------------------------------------
MMTPROC_API int MmtCapManApiSetParameterOutput(int id, unsigned char *pointer, int size);

//**************************************************
// 動作開始停止
//**************************************************
//--------------------------------------------------
// int MmtCapManApiStart(int id)
// 説明：動作開始
// 引数：int id
//     ：＜インスタンスID＞
// 戻値：int＜正否＞(0=OK,マイナス値=エラーコード)
//--------------------------------------------------
MMTPROC_API int MmtCapManApiStart(int id);

//--------------------------------------------------
// int MmtCapManApiPause(int id)
// 説明：一時停止
// 引数：int id
//     ：＜インスタンスID＞
// 引数：int enable
//     ：＜一時停止制御＞(0=一時停止解除,1=一時停止)
//     ：enable＝1で一時停止 enable=0で一時停止解除
// 戻値：int＜正否＞(0=OK,マイナス値=エラーコード)
//--------------------------------------------------
//MMTPROC_API int MmtCapManApiPause(int id, int enable);

//--------------------------------------------------
// int MmtCapManApiStop(int id)
// 説明：動作停止
// 引数：int id
//     ：＜インスタンスID＞
// 戻値：int＜正否＞(0=OK,マイナス値=エラーコード)
//--------------------------------------------------
MMTPROC_API int MmtCapManApiStop(int id);

//**************************************************
// MMT s
//**************************************************
MMTPROC_API int MmtCapInputApiPushMmtData(int id, unsigned char *pointer, int size);

//**************************************************
// NTP
//**************************************************
MMTPROC_API int MmtCapManApiPushNtp(int id, unsigned long long Ntp);	//※NTP情報を上位から通知(Ntp=LongNTP)

//**************************************************
// 通知登録
//**************************************************
MMTPROC_API int MmtCapManApiAppendListener(int id, IMmtCapCaptureListener* pListener);	//リスナー登録追加  
MMTPROC_API int MmtCapManApiRemoveListener(int id, IMmtCapCaptureListener* pListener);	//リスナー登録削除


#endif //_MMT_CAP_PROC_API_
