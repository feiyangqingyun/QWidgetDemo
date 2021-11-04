///////////////////////////////////////////////////////////////////////////
// DS-40xxHC/HF BOARD SYSTEM SDK                     //    
/////////////////////////////////////////////////////////////////////////// 

#ifndef DECODECARD_SDK_H
#define DECODECARD_SDK_H

#include "datatype.h"

#define DLLEXPORT_API  extern "C"__declspec(dllexport) 
#define ERR_WAIT_TIMEOUT                0xc0000001
#define ERR_INVALID_HANDLE                0xc0000002
#define ERR_INVALID_ARGUMENT                0xc0000003
#define ERR_DDRAW_CREATE_FAILED                0xc0000004
#define ERR_DDRAW_CAPS_FAULT                0xc0000005
#define ERR_SET_COOPERATIVELEVEL_FAILED            0xc0000006
#define ERR_PRIMARY_SURFACE_CREATE_FAILED        0xc0000007
#define ERR_GET_OVERLAY_ADDRESS_FAILED            0xc0000008
#define ERR_OVERLAY_SURFACE_CREATE_FAILED        0xc0000009
#define ERR_OVERLAY_UPDATE_FAILED            0xc000000a
#define ERR_TMMAN_FAILURE                0xc000000b
#define ERR_CHANNELMAGIC_MISMATCH            0xc000000c
#define ERR_CALLBACK_REGISTERED                0xc000000d
#define ERR_QUEUE_OVERFLOW                0xc000000e
#define ERR_STREAM_THREAD_FAILURE            0xc000000f
#define ERR_THREAD_STOP_ERROR                0xc0000010
#define ERR_NOT_SUPPORT                    0xc0000011
#define ERR_OUTOF_MEMORY                0xc0000012
#define ERR_DSP_BUSY                    0xc0000013
#define ERR_DATA_ERROR                    0xc0000014
#define ERR_KERNEL                    0xc0000016
#define ERR_OFFSCREEN_CREATE_FAILED            0xc0000017
#define ERR_MULTICLOCK_FAILURE                0xc0000018
#define ERR_INVALID_DEVICE                0xc0000019
#define ERR_INVALID_DRIVER                0xc000001a
//error code for MD card
#define HWERR_SUCCESS                        0
#define HWERR_ALLOCATE_MEMORY                0xc1000001
#define HWERR_INVALID_HANDLE                0xc1000002
#define HWERR_DDRAW_CREATE_FAILED            0xc1000003
#define HWERR_DDRAW_CAPS_FAULT                0xc1000004
#define HWERR_SET_COOPERATIVELEVEL_FAILED        0xc1000005
#define HWERR_PRIMARY_SURFACE_CREATE_FAILED        0xc1000006
#define HWERR_OVERLAY_CREATE_FAILED            0xc1000007
#define HWERR_GET_OVERLAY_ADDRESS_FAILED        0xc1000008
#define HWERR_OVERLAY_UPDATE_FAILED            0xc1000009
#define HWERR_SURFACE_NULL                0xc100000a
#define HWERR_FILEHEADER_UNKNOWN            0xc100000b
#define HWERR_CREATE_FILE_FAILED            0xc100000c
#define HWERR_FILE_SIZE_ZERO                0xc100000d
#define HWERR_FILE_SIZE_INVALID                0xc100000d
#define HWERR_CREATE_OBJ_FAILED                0xc100000e
#define HWERR_CHANNELMAGIC_MISMATCH            0xc100000f
#define HWERR_PARA_OVER                    0xc1000010
#define HWERR_ORDER                    0xc1000011
#define HWERR_COMMAND                    0xc1000012
#define HWERR_UNSUPPORTED                0xc1000013
#define HWERR_DSPOPEN                    0xc1000014
#define HWERR_DSPLOAD                    0xc1000015
#define HWERR_ALLOCATE_DSPMEMORY            0xc1000016
#define HWERR_DSPCHECHER                0xc1000017
#define HWERR_IMGFILE_UNKNOWN                0xc1000018
#define HWERR_INVALID_FILE                0xc1000019
//standart
#define HW_PAL                            2
#define HW_NTSC                            1
//jump direction
#define HW_JUMP_FORWARD                    309 
#define HW_JUMP_BACKWARD                310


typedef enum tagTypeVideoFormat
{
   vdfRGB8A_233              = 0x00000001,
   vdfRGB8R_332              = 0x00000002,
   vdfRGB15Alpha             = 0x00000004,
   vdfRGB16                  = 0x00000008,
   vdfRGB24                  = 0x00000010,
   vdfRGB24Alpha             = 0x00000020,
   
   vdfYUV420Planar           = 0x00000040,
   vdfYUV422Planar           = 0x00000080,
   vdfYUV411Planar           = 0x00000100,
   vdfYUV420Interspersed     = 0x00000200,
   vdfYUV422Interspersed     = 0x00000400,
   vdfYUV411Interspersed     = 0x00000800,
   vdfYUV422Sequence         = 0x00001000,   /* U0, Y0, V0, Y1:  For VO overlay */
   vdfYUV422SequenceAlpha    = 0x00002000,   
   /* U0, Y0, V0, Y1:  For VO overlay, with low bit for alpha blending */
   vdfMono                   = 0x00004000,  /* 8 bit monochrome */

   vdfYUV444Planar           = 0x00008000,
}TypeVideoFormat;


typedef enum _BitrateControlType_t
{
    brCBR = 0,
    brVBR = 1,
}BitrateControlType_t;

typedef enum _BOARD_TYPE_DS
{
    DS400XM             =0,
    DS400XH                =1,
    DS4004HC            =2,
    DS4008HC            =3,
    DS4016HC            =4,
    DS4001HF            =5,
    DS4004HF            =6,
    DS4002MD            =7,
    DS4004MD            =8,        //4004MD
    DS4016HCS            =9,        //4016HCS
    DS4002HT            =10,    //4002HT
    DS4004HT            =11,    //4004HT
    DS4008HT            =12,    //4008HT
    DS4004HC_PLUS        =13,    //4004HC+
    DS4008HC_PLUS        =14,    //4008HC+
    DS4016HC_PLUS        =15,    //4016HC+
    DS4008HF            =16,    //4008HF
    DS4008MD            =17,    //4008MD
    DS4008HS            =18,    //4008HS
    DS4016HS            =19,    //4016HS
    INVALID_BOARD_TYPE    =0xffffffff,
}BOARD_TYPE_DS;

#define STREAM_TYPE_VIDEO    1
#define STREAM_TYPE_AUDIO    2
#define STREAM_TYPE_AVSYNC    3
#define DRAWFUN(x)                void  (CALLBACK* x)(long nPort,HDC hDc,LONG nUser)


typedef void (*LOGRECORD_CALLBACK)(char *str, void *context);
typedef int (*STREAM_READ_CALLBACK)(ULONG channelNumber, void *context);
typedef int (*STREAM_DIRECT_READ_CALLBACK)(ULONG channelNumber,void *DataBuf,DWORD Length,int FrameType,void *context);

typedef struct tagChannelCapability{
    UCHAR bAudioPreview;
    UCHAR bAlarmIO;
    UCHAR bWatchDog;
}CHANNEL_CAPABILITY, *PCHANNEL_CAPABILITY;

typedef struct tagFramsStatistics{
    ULONG VideoFrames;
    ULONG AudioFrames;
    ULONG FramesLost;
    ULONG QueueOverflow;
    ULONG CurBps;
}FRAMES_STATISTICS, *PFRAMES_STATISTICS;

DLLEXPORT_API int __stdcall InitDSPs();
DLLEXPORT_API int __stdcall DeInitDSPs();
DLLEXPORT_API HANDLE __stdcall ChannelOpen(int ChannelNum);
DLLEXPORT_API int __stdcall ChannelClose(HANDLE hChannelHandle);
DLLEXPORT_API int __stdcall GetTotalChannels();
DLLEXPORT_API int __stdcall GetTotalDSPs();
DLLEXPORT_API int __stdcall StartVideoPreview(HANDLE hChannelHandle,HWND WndHandle, RECT *rect, BOOLEAN bOverlay, int VideoFormat, int FrameRate);
DLLEXPORT_API int __stdcall StopVideoPreview(HANDLE hChannelHandle);
DLLEXPORT_API int __stdcall SetVideoPara(HANDLE hChannelHandle, int Brightness, int Contrast, int Saturation, int Hue);
DLLEXPORT_API int __stdcall GetVideoPara(HANDLE hChannelHandle, VideoStandard_t *VideoStandard, int *Brightness, int *Contrast, int *Saturation, int *Hue);
DLLEXPORT_API int __stdcall GetVideoSignal(HANDLE  hChannelHandle);
DLLEXPORT_API int __stdcall GetSDKVersion(PVERSION_INFO VersionInfo);
DLLEXPORT_API int __stdcall GetCapability(HANDLE hChannelHandle, CHANNEL_CAPABILITY *Capability);
DLLEXPORT_API int __stdcall GetLastErrorNum(HANDLE hChannelHandle, ULONG *DspError, ULONG *SdkError);
DLLEXPORT_API int __stdcall SetStreamType(HANDLE hChannelHandle, USHORT Type);
DLLEXPORT_API int __stdcall GetStreamType(HANDLE hChannelHandle, USHORT *StreamType);
DLLEXPORT_API int __stdcall GetFramesStatistics(HANDLE hChannelHandle,     PFRAMES_STATISTICS framesStatistics);
DLLEXPORT_API int __stdcall StartMotionDetection(HANDLE hChannelHandle);
DLLEXPORT_API int __stdcall GetBoardInfo(HANDLE hChannelHandle, ULONG *BoardType, UCHAR *SerialNo);
DLLEXPORT_API int __stdcall StopMotionDetection(HANDLE hChannelHandle);
DLLEXPORT_API int __stdcall GetOriginalImage(HANDLE hChannelHandle, UCHAR *ImageBuf, ULONG *Size);
DLLEXPORT_API int __stdcall RegisterLogRecordCallback(LOGRECORD_CALLBACK LogRecordFunc, void *Context);
DLLEXPORT_API int __stdcall SetAudioPreview(HANDLE hChannelHandle, BOOL bEnable);
DLLEXPORT_API int __stdcall ReadStreamData(HANDLE hChannelHandle, void *DataBuf, DWORD *Length, int *FrameType);
DLLEXPORT_API int __stdcall RegisterMessageNotifyHandle(HWND hWnd, UINT MessageId);
DLLEXPORT_API int __stdcall StartVideoCapture(HANDLE hChannelHandle);
DLLEXPORT_API int __stdcall StopVideoCapture(HANDLE hChannelHandle);
DLLEXPORT_API int __stdcall SetIBPMode(HANDLE hChannelHandle, int KeyFrameIntervals, int BFrames, int PFrames, int FrameRate);
DLLEXPORT_API int __stdcall SetDefaultQuant(HANDLE hChannelHandle, int IQuantVal, int PQuantVal, int BQuantVal);
DLLEXPORT_API int __stdcall SetOsd(HANDLE hChannelHandle, BOOL Enable);

DLLEXPORT_API int __stdcall SetLogo(HANDLE hChannelHandle, int x, int y, int w, int h, unsigned char *yuv);
DLLEXPORT_API int __stdcall StopLogo(HANDLE hChannelHandle);
DLLEXPORT_API int __stdcall SetupMotionDetection(HANDLE hChannelHandle, RECT *RectList, int iAreas);
DLLEXPORT_API int __stdcall MotionAnalyzer(HANDLE hChannelHandle, char *MotionData, int iThreshold, int *iResult);
DLLEXPORT_API int __stdcall LoadYUVFromBmpFile(char *FileName, unsigned char *yuv, int BufLen, int *Width, int *Height);
DLLEXPORT_API int __stdcall SaveYUVToBmpFile(char *FileName, unsigned char *yuv, int Width, int Height);
DLLEXPORT_API int __stdcall CaptureIFrame(HANDLE hChannelHandle);
DLLEXPORT_API int __stdcall RegisterStreamReadCallback(STREAM_READ_CALLBACK StreamReadCallback, void *Context);
DLLEXPORT_API int __stdcall AdjustMotionDetectPrecision(HANDLE hChannelHandle, 
                                           int iGrade, int iFastMotionDetectFps, 
                                           int iSlowMotionDetectFps);
DLLEXPORT_API int __stdcall SetupBitrateControl(HANDLE hChannelHandle, ULONG MaxBps);
DLLEXPORT_API int __stdcall SetOverlayColorKey(COLORREF DestColorKey);
DLLEXPORT_API int __stdcall SetOsdDisplayMode(HANDLE hChannelHandle, int Brightness, BOOL Translucent, int parameter, USHORT *Format1, USHORT *Format2);
DLLEXPORT_API int __stdcall SetLogoDisplayMode(HANDLE hChannelHandle, COLORREF ColorKey, BOOL Translucent, int TwinkleInterval);
DLLEXPORT_API int __stdcall SetEncoderPictureFormat(HANDLE hChannelHandle, PictureFormat_t PictureFormat);
DLLEXPORT_API int __stdcall SetVideoStandard(HANDLE hChannelHandle, VideoStandard_t VideoStandard);
DLLEXPORT_API int __stdcall RestoreOverlay();
DLLEXPORT_API int __stdcall ResetDSP(int DspNumber);
DLLEXPORT_API int __stdcall GetSoundLevel(HANDLE hChannelHandle);


DLLEXPORT_API int __stdcall SetBitrateControlMode(HANDLE hChannelHandle, BitrateControlType_t brc);
DLLEXPORT_API int __stdcall SetupNotifyThreshold(HANDLE hChannelHandle, int iFramesThreshold);

DLLEXPORT_API int __stdcall SetupSubChannel(HANDLE hChannelHandle, int iSubChannel);
DLLEXPORT_API int __stdcall GetSubChannelStreamType(void  *DataBuf, int FrameType);
//add for HC/HF 
DLLEXPORT_API int __stdcall RegisterStreamDirectReadCallback(STREAM_DIRECT_READ_CALLBACK StreamDirectReadCallback,void *Context);
DLLEXPORT_API int __stdcall RegisterDrawFun(DWORD nport, DRAWFUN(DrawFun),LONG nUser);
DLLEXPORT_API int __stdcall SetupMask(HANDLE hChannelHandle, RECT *rectList, int iAreas);
DLLEXPORT_API int __stdcall StopMask(HANDLE hChannelHandle);
DLLEXPORT_API int __stdcall SetSubEncoderPictureFormat(HANDLE hChannelHandle, PictureFormat_t PictureFormat);
DLLEXPORT_API int __stdcall StartSubVideoCapture(HANDLE hChannelHandle);
DLLEXPORT_API int __stdcall StopSubVideoCapture(HANDLE hChannelHandle);
DLLEXPORT_API int __stdcall SetupDateTime(HANDLE hChannelHandle, SYSTEMTIME *now);
/*
    以下为1.7版本新增的功能。
    以后可能会根据用户的反馈进行修改和完善。
*/
//原始图像流设置
typedef void (*IMAGE_STREAM_CALLBACK)(UINT channelNumber,void *context );
DLLEXPORT_API int __stdcall SetImageStream(HANDLE hChannel,BOOL bStart,UINT fps,UINT width,UINT height,unsigned char *imageBuffer);
DLLEXPORT_API int __stdcall RegisterImageStreamCallback(IMAGE_STREAM_CALLBACK,void *context);
/*
    设置视频输入位置：
        (x,y)为系统所处理图像的左上角在摄像机输入的原始图像中的坐标。
        x必须为2的整数倍。
        (x,y)坐标的参数范围和摄像机的型号有关，如果指定的值和摄像机的输入不匹配，
    可能会导致图像静止或水平、垂直方向滚动。
*/
DLLEXPORT_API int __stdcall SetInputVideoPosition(HANDLE hChannel,UINT x,UINT y);
DLLEXPORT_API int __stdcall StopRegisterDrawFun(DWORD nport);

/*
    3.0
*/
#define SERIAL_NUMBER_LENGTH    12    //板卡序列号长度
typedef struct tagDS_BOARD_DETAIL
{
    BOARD_TYPE_DS type;                    //板卡类型
    BYTE sn[16];                        //序列号
    UINT dspCount;                        //板卡包含的DSP个数
    UINT firstDspIndex;                    //板卡上第一个DSP的索引
    UINT encodeChannelCount;            //板卡包含的编码通道个数
    UINT firstEncodeChannelIndex;        //板卡上第一个编码通道的索引
    UINT decodeChannelCount;            //板卡包含的解码通道个数
    UINT firstDecodeChannelIndex;        //板卡上第一个解码通道的索引
    UINT displayChannelCount;            //板卡包含的视频输出通道个数
    UINT firstDisplayChannelIndex;        //板卡上第一个视频输出通道的索引
    UINT reserved1;
    UINT reserved2;
    UINT reserved3;
    UINT reserved4;
}DS_BOARD_DETAIL;
typedef struct tagDSP_DETAIL
{
    UINT encodeChannelCount;            //板卡包含的编码通道个数
    UINT firstEncodeChannelIndex;        //板卡上第一个编码通道的索引
    UINT decodeChannelCount;            //板卡包含的解码通道个数
    UINT firstDecodeChannelIndex;        //板卡上第一个解码通道的索引
    UINT displayChannelCount;            //板卡包含的视频输出通道个数
    UINT firstDisplayChannelIndex;        //板卡上第一个视频输出通道的索引
    UINT reserved1;
    UINT reserved2;
    UINT reserved3;
    UINT reserved4;
}DSP_DETAIL;
DLLEXPORT_API unsigned int __stdcall GetBoardCount();
DLLEXPORT_API int __stdcall GetBoardDetail(UINT boardNum,DS_BOARD_DETAIL *pBoardDetail);
DLLEXPORT_API unsigned int __stdcall GetDspCount();
DLLEXPORT_API int __stdcall GetDspDetail(UINT dspNum,DSP_DETAIL *pDspDetail);
DLLEXPORT_API unsigned int __stdcall GetEncodeChannelCount();
DLLEXPORT_API unsigned int __stdcall GetDecodeChannelCount();
DLLEXPORT_API unsigned int __stdcall GetDisplayChannelCount();
DLLEXPORT_API int __stdcall SetDefaultVideoStandard(VideoStandard_t VideoStandard);
DLLEXPORT_API int __stdcall SetVideoDetectPrecision(HANDLE hChannel,unsigned int value);
DLLEXPORT_API int __stdcall SetSubStreamType(HANDLE hChannelHandle, USHORT Type);
DLLEXPORT_API int __stdcall GetSubStreamType(HANDLE hChannelHandle, USHORT *StreamType);

#define MAX_DISPLAY_REGION        16
typedef struct tagREGION_PARAM
{
    UINT left;
    UINT top;
    UINT width;
    UINT height;
    COLORREF color;
    UINT param;
}REGION_PARAM;
DLLEXPORT_API int __stdcall SetDisplayStandard(UINT nDisplayChannel,VideoStandard_t VideoStandard);
DLLEXPORT_API int __stdcall SetDisplayRegion(UINT nDisplayChannel,UINT nRegionCount,REGION_PARAM *pParam,UINT nReserved);
DLLEXPORT_API int __stdcall ClearDisplayRegion(UINT nDisplayChannel,UINT nRegionFlag);
DLLEXPORT_API int __stdcall SetDisplayRegionPosition(UINT nDisplayChannel,UINT nRegion,UINT nLeft,UINT nTop);
DLLEXPORT_API int __stdcall FillDisplayRegion(UINT nDisplayChannel,UINT nRegion,unsigned char *pImage);
DLLEXPORT_API int __stdcall SetEncoderVideoExtOutput(UINT nEncodeChannel,UINT nPort,BOOL bOpen,UINT nDisplayChannel,UINT nDisplayRegion,UINT nReserved);
DLLEXPORT_API int __stdcall SetDecoderVideoExtOutput(UINT nDecodeChannel,UINT nPort,BOOL bOpen,UINT nDisplayChannel,UINT nDisplayRegion,UINT nReserved);
DLLEXPORT_API int __stdcall SetDecoderVideoOutput(UINT nDecodeChannel,UINT nPort,BOOL bOpen,UINT nDisplayChannel,UINT nDisplayRegion,UINT nReserved);
DLLEXPORT_API int __stdcall SetDecoderAudioOutput(UINT nDecodeChannel,BOOL bOpen,UINT nOutputChannel);
//3.1
DLLEXPORT_API int __stdcall SetDeInterlace(HANDLE hChannelHandle,UINT mode,UINT level);
DLLEXPORT_API int __stdcall SetPreviewOverlayMode(BOOL bTrue);

//DECODE functions for DS4002MD
#if defined( _WINDLL)
    #define PLAYER_API  extern "C"__declspec(dllexport) 
#else
    #define PLAYER_API  extern "C" __declspec(dllimport) 
#endif
typedef struct tagDISPLAY_PARA
{
    long bToScreen;
    long bToVideoOut;
    long nLeft;
    long nTop;
    long nWidth;
    long nHeight;
    long nReserved;
}DISPLAY_PARA,*PDISPLAY_PARA;
//Version info
typedef struct tagVERSION{
    ULONG DspVersion, DspBuildNum;
    ULONG DriverVersion, DriverBuildNum;
    ULONG SDKVersion, SDKBuildNum;
}HW_VERSION, *PHW_VERSION;

//init part
PLAYER_API int __stdcall HW_InitDirectDraw(HWND hParent,COLORREF colorKey);
PLAYER_API int __stdcall HW_ReleaseDirectDraw();
PLAYER_API int __stdcall HW_InitDecDevice(long *pDeviceTotal);
PLAYER_API int __stdcall HW_ReleaseDecDevice();
PLAYER_API int __stdcall HW_ChannelOpen(long nChannelNum,HANDLE* phChannel);
PLAYER_API int __stdcall HW_ChannelClose(HANDLE hChannel);
//open part

PLAYER_API int __stdcall HW_OpenStream(HANDLE hChannel,PBYTE pFileHeadBuf,DWORD nSize);
DLLEXPORT_API int __stdcall HW_ResetStream(HANDLE hChannel);
PLAYER_API int __stdcall HW_CloseStream(HANDLE hChannel);
PLAYER_API int __stdcall HW_InputData(HANDLE hChannel,PBYTE pBuf,DWORD nSize);
PLAYER_API int __stdcall HW_OpenFile(HANDLE hChannel,LPTSTR sFileName);
PLAYER_API int __stdcall HW_CloseFile(HANDLE hChannel);

//play part
PLAYER_API int __stdcall HW_SetDisplayPara(HANDLE hChannel,DISPLAY_PARA *pPara);
PLAYER_API int __stdcall HW_Play(HANDLE hChannel);
PLAYER_API int __stdcall HW_Stop(HANDLE hChannel);
PLAYER_API int __stdcall HW_Pause(HANDLE hChannel,ULONG bPause);

//sound part
PLAYER_API int __stdcall HW_PlaySound(HANDLE hChannel);
PLAYER_API int __stdcall HW_StopSound(HANDLE hChannel);
PLAYER_API int __stdcall HW_SetVolume(HANDLE hChannel,ULONG nVolume);
//overlay part
PLAYER_API int __stdcall HW_RefreshSurface();
PLAYER_API int __stdcall HW_RestoreSurface();
PLAYER_API int __stdcall HW_ClearSurface();
PLAYER_API int __stdcall HW_ZoomOverlay(RECT* pSrcClientRect, RECT* pDecScreenRect);
//cut file
PLAYER_API int __stdcall HW_StartCapFile(HANDLE hChannel,LPTSTR sFileName);
PLAYER_API int __stdcall HW_StopCapFile(HANDLE hChannel);
//capture picture
PLAYER_API int __stdcall HW_GetYV12Image(HANDLE hChannel, PBYTE pBuffer, ULONG nSize);
PLAYER_API int __stdcall HW_GetPictureSize(HANDLE hChannel,ULONG* pWidth, ULONG* pHeight);
PLAYER_API int __stdcall HW_ConvertToBmpFile(BYTE * pBuf,ULONG nSize,ULONG nWidth,ULONG nHeight,char *sFileName,ULONG nReserved);
//setting and getting  part
PLAYER_API int __stdcall HW_Jump(HANDLE hChannel,ULONG nDirection);
PLAYER_API int __stdcall HW_SetJumpInterval(HANDLE hChannel,ULONG nSecond);
PLAYER_API int __stdcall HW_GetSpeed(HANDLE hChannel,long *pSpeed);
PLAYER_API int __stdcall HW_SetSpeed(HANDLE hChannel,long nSpeed);
PLAYER_API int __stdcall HW_SetPlayPos(HANDLE hChannel,ULONG nPos);
PLAYER_API int __stdcall HW_GetPlayPos(HANDLE hChannel,ULONG* pPos);
PLAYER_API int __stdcall HW_GetVersion(PHW_VERSION pVersion);
PLAYER_API int __stdcall HW_GetCurrentFrameRate(HANDLE hChannel,ULONG* pFrameRate);
PLAYER_API int __stdcall HW_GetCurrentFrameNum(HANDLE hChannel,ULONG* pFrameNum);
PLAYER_API int __stdcall HW_GetFileTotalFrames(HANDLE hChannel,ULONG* pTotalFrames);
PLAYER_API int __stdcall HW_GetFileTime(HANDLE hChannel, ULONG* pFileTime);
PLAYER_API int __stdcall HW_GetCurrentFrameTime(HANDLE hChannel,ULONG* pFrameTime);
PLAYER_API int __stdcall HW_GetPlayedFrames(HANDLE hChannel,ULONG *pDecVFrames);
PLAYER_API int __stdcall HW_GetDeviceSerialNo(HANDLE hChannel,ULONG *pDeviceSerialNo);
PLAYER_API int __stdcall HW_SetFileEndMsg(HANDLE hChannel,HWND hWnd,UINT nMsg);
PLAYER_API int __stdcall HW_SetStreamOpenMode(HANDLE hChannel,ULONG nMode);
PLAYER_API int __stdcall HW_GetStreamOpenMode(HANDLE hChannel,ULONG *pMode);
PLAYER_API int __stdcall HW_SetVideoOutStandard(HANDLE hChannel,ULONG nStandard);
PLAYER_API int __stdcall HW_SetDspDeadlockMsg(HWND hWnd,UINT nMsg);
PLAYER_API int __stdcall HW_GetChannelNum(long nDspNum,long *pChannelNum,ULONG nNumsToGet,ULONG * pNumsGotten);
PLAYER_API int __stdcall HW_ResetDsp(long nDspNum);
PLAYER_API int __stdcall HW_SetAudioPreview(HANDLE hChannel, BOOL bEnable);
//////////////////////////////////////////////

PLAYER_API int __stdcall HW_OpenStreamEx(HANDLE hChannel,PBYTE pFileHeadBuf,DWORD nSize);
PLAYER_API int __stdcall HW_CloseStreamEx(HANDLE hChannel);
PLAYER_API int __stdcall HW_InputVideoData(HANDLE hChannel,PBYTE pBuf,DWORD nSize);
PLAYER_API int __stdcall HW_InputAudioData(HANDLE hChannel,PBYTE pBuf,DWORD nSize);

//4.0
PLAYER_API int __stdcall SetOsdDisplayModeEx(HANDLE hChannelHandle,int color,BOOL Translucent,int param,int nLineCount,USHORT **Format);
typedef void (*MOTION_DETECTION_CALLBACK)(ULONG channelNumber,BOOL bMotionDetected,void *context);
PLAYER_API int __stdcall SetupMotionDetectionEx(HANDLE hChannelHandle,int iGrade,int iFastMotionDetectFps,
                                                   int iSlowMotionDetectFps,UINT delay,RECT *RectList, int iAreas,
                                                   MOTION_DETECTION_CALLBACK MotionDetectionCallback,int reserved);
PLAYER_API int __stdcall GetJpegImage(HANDLE hChannelHandle,UCHAR *ImageBuf,ULONG *Size,UINT nQuality);
//WatchDog
PLAYER_API int __stdcall SetWatchDog(UINT boardNumber,BOOL bEnable);
//4.1
typedef void (*FILE_REF_DONE_CALLBACK)(UINT nChannel,UINT nSize);
PLAYER_API int __stdcall HW_SetFileRef(HANDLE hChannel,BOOL bEnable,FILE_REF_DONE_CALLBACK FileRefDoneCallback);
PLAYER_API int __stdcall HW_LocateByAbsoluteTime(HANDLE hChannel,SYSTEMTIME time);
PLAYER_API int __stdcall HW_LocateByFrameNumber(HANDLE hChannel,UINT frmNum);
PLAYER_API int __stdcall HW_GetCurrentAbsoluteTime(HANDLE hChannel,SYSTEMTIME *pTime);
PLAYER_API int __stdcall HW_GetFileAbsoluteTime(HANDLE hChannel,SYSTEMTIME *pStartTime,SYSTEMTIME *pEndTime);
//4.2
DLLEXPORT_API int __stdcall HW_ImportFileRef(HANDLE hChannel,char *pBuffer,UINT nSize);
DLLEXPORT_API int __stdcall HW_ExportFileRef(HANDLE hChannel,char *pBuffer,UINT nSize);
DLLEXPORT_API int __stdcall SetDisplayVideoCapture(UINT nDisplayChannel,BOOL bStart,UINT fps,UINT width,UINT height,unsigned char *imageBuffer);
DLLEXPORT_API int __stdcall RegisterDisplayVideoCaptureCallback(IMAGE_STREAM_CALLBACK DisplayVideoCaptureCallback,void *context);
DLLEXPORT_API int __stdcall SetDisplayVideoBrightness(UINT chan,int Brightness);
DLLEXPORT_API int __stdcall SetChannelStreamCRC(HANDLE hChannel,BOOL bEnable);
DLLEXPORT_API int __stdcall SetSubChannelStreamCRC(HANDLE hChannel,BOOL bEnable);
DLLEXPORT_API int __stdcall HW_SetDecoderPostProcess(HANDLE hChannel,UINT param);
//
typedef void (*DECODER_VIDEO_CAPTURE_CALLBACK)(UINT nChannelNumber,void *DataBuf,UINT width,UINT height,UINT nFrameNum,UINT nFrameTime,SYSTEMTIME *pFrameAbsoluteTime,void *context);
DLLEXPORT_API int __stdcall RegisterDecoderVideoCaptureCallback(DECODER_VIDEO_CAPTURE_CALLBACK DecoderVideoCaptureCallback,void *context);
DLLEXPORT_API int __stdcall HW_SetDecoderVideoCapture(HANDLE hChannel,BOOL bStart,UINT param);
DLLEXPORT_API int __stdcall HW_InputDataByFrame(HANDLE hChannel,PBYTE pBuf,DWORD nSize);
/*
人脸检测接口:仅供测试、评估，以后可能会做修改。
*/
typedef struct
{
    short x,y,width,height;
}FACE_AREA_DEMO;
typedef struct
{
    FACE_AREA_DEMO faceArea;
    FACE_AREA_DEMO leftEyeArea;
    FACE_AREA_DEMO rightEyeArea;
    FACE_AREA_DEMO leftPupilArea;
    FACE_AREA_DEMO rightPupilArea;
    FACE_AREA_DEMO noseArea;
    FACE_AREA_DEMO mouthArea;
}FACE_INFO_DEMO;
typedef void (*FACE_DETECTION_DEMO_CALLBACK)(UINT nChannel,UINT nFaceCount,FACE_INFO_DEMO *pFaceInfo,
                                        char *pData,UINT nDataSize,UINT nImageWidth,UINT nImageHeight);

DLLEXPORT_API int __stdcall SetFaceDetectionDemo(HANDLE hChannelHandle,BOOL bEnable,
                                                 UINT nFrameInterval,FACE_DETECTION_DEMO_CALLBACK pFunc,
                                                 BOOL bCompress,UINT nCompressQuality,BOOL bLocateEyePos);

#endif

