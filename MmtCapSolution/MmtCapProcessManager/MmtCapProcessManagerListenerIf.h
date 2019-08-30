#pragma once

#ifdef MMT_CAP_PROCESS_LISTERNER_EXPORTS

#define MMT_CAP_PROCESS_LISTERNER_API __declspec(dllexport)
#else
#define MMT_CAP_PROCESS_LISTERNER_API __declspec(dllimport)
#endif

__interface MMT_CAP_PROCESS_LISTERNER_API IMmtCapCaptureListener
{
  	public:
//		IMmtCapCaptureListener(){};
//		virtual ~IMmtCapCaptureListener(){};
  	
	//画像出力通知
	virtual int MmtCapOutputApiEventUpdateCaptionPlane(int id) = 0;
	
  	//エラー通知イベント
    virtual int MmtCapManApiEventError(int id, unsigned long ErrorCode) = 0;
    
    //他にもいろいろ通知イベントのメソッドを定義します…
    
};


