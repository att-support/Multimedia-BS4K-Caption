// MmtMultimediaControl.h

#pragma once

#ifdef __cplusplus
#define PluginAPI extern "C" __declspec(dllexport)
#else
#define PluginAPI __declspec(dllexport)
#endif // __cplusplus

#ifndef ___MMT_MULTIMEDIA_CONTROL

PluginAPI long ApiCreate(long number, long type, long resolution);

PluginAPI long ApiRelease(long number, long type);

PluginAPI void ApiInit(long logLevel, char *logPath);

PluginAPI void ApiDestory();

PluginAPI long ApiSendCommand(long number, char *cmd, int size);

//ctrl_type=0 通常
//ctrl_type=1 フラッシング	->image_intervalに点滅[msec]
//ctrl_type=2 スクロール　	->image_intervalは速さ
//duration =0 通常は0。end-beginがある場合、表示時間[msec]を示す
//PluginAPI long ApiCaptionUpdate(long number, void *input_pointer, int input_size, void *output_pointer, int output_size, int image_type, int *ctrl_type, long *image_interval, long *duration, long *begin, long *end); //字幕用 入力(ZIP)＋出力(画像) [C++->C#]
PluginAPI long ApiCaptionUpdate(long number, void *input_pointer, int input_size, void *output_pointer, int output_size, int image_type, int *ctrl_type, int *image_interval, int *duration, int *begin, int *end, int animationOpacity); //字幕用 入力(ZIP)＋出力(画像) [C++->C#]

PluginAPI long ApiMmDataUpdate(long number, void *output_pointer, int output_size, int image_type); //データ放送用 出力(画像) [C++->C#]

PluginAPI long ApiMmDataRecvCommand(long number, char *cmd, int size); //CollBack for UDP Received[C#->C++]

PluginAPI long ApiSetParam(long number, long type, char *param, int size);

PluginAPI long ApiStart(long number, long type);

PluginAPI long ApiMmDataReload(long number);
#endif // ___MMT_MULTIMEDIA_CONTROL