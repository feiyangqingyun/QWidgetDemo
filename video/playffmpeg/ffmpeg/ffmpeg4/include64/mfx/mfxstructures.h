/******************************************************************************* *\

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2007-2012 Intel Corporation. All Rights Reserved.

File Name: mfxstructures.h

*******************************************************************************/
#ifndef __MFXSTRUCTURES_H__
#define __MFXSTRUCTURES_H__
#include "mfxdefs.h"

#pragma warning(disable: 4201)

#ifdef __cplusplus
extern "C" {
#endif

#define MFX_MAKEFOURCC(A,B,C,D)    ((((int)A))+(((int)B)<<8)+(((int)C)<<16)+(((int)D)<<24))

/* Extended Configuration Header Structure */
typedef struct {
    mfxU32  BufferId;
    mfxU32  BufferSz;
} mfxExtBuffer;

/* Frame ID for SVC and MVC */
typedef struct {
    mfxU16      TemporalId;
    mfxU16      PriorityId;
    union {
        struct {
            mfxU16  DependencyId;
            mfxU16  QualityId;
        };
        struct {
            mfxU16  ViewId;
        };
    };
} mfxFrameId;

/* Frame Info */
typedef struct {
    mfxU32  reserved[6];
    mfxFrameId FrameId;

    mfxU32  FourCC;
    mfxU16  Width;
    mfxU16  Height;

    mfxU16  CropX;
    mfxU16  CropY;
    mfxU16  CropW;
    mfxU16  CropH;

    mfxU32  FrameRateExtN;
    mfxU32  FrameRateExtD;
    mfxU16  reserved3;

    mfxU16  AspectRatioW;    
    mfxU16  AspectRatioH;

    mfxU16  PicStruct;
    mfxU16  ChromaFormat;
    mfxU16  reserved2;
} mfxFrameInfo;

/* FourCC */
enum {
    MFX_FOURCC_NV12         = MFX_MAKEFOURCC('N','V','1','2'),   /* Native Format */
    MFX_FOURCC_YV12         = MFX_MAKEFOURCC('Y','V','1','2'),
    MFX_FOURCC_YUY2         = MFX_MAKEFOURCC('Y','U','Y','2'),
    MFX_FOURCC_RGB3         = MFX_MAKEFOURCC('R','G','B','3'),   /* RGB24 */
    MFX_FOURCC_RGB4         = MFX_MAKEFOURCC('R','G','B','4'),    /* RGB32 */
    MFX_FOURCC_P8           = 41         /*  D3DFMT_P8   */
};

/* PicStruct */
enum {
    MFX_PICSTRUCT_UNKNOWN       =0x00,
    MFX_PICSTRUCT_PROGRESSIVE   =0x01,
    MFX_PICSTRUCT_FIELD_TFF     =0x02,
    MFX_PICSTRUCT_FIELD_BFF     =0x04,

    MFX_PICSTRUCT_FIELD_REPEATED=0x10,  /* first field repeated, pic_struct=5 or 6 in H.264 */
    MFX_PICSTRUCT_FRAME_DOUBLING=0x20,  /* pic_struct=7 in H.264 */
    MFX_PICSTRUCT_FRAME_TRIPLING=0x40   /* pic_struct=8 in H.264 */
};

/* ColorFormat */
enum {
    MFX_CHROMAFORMAT_MONOCHROME =0,
    MFX_CHROMAFORMAT_YUV420     =1,
    MFX_CHROMAFORMAT_YUV422     =2,
    MFX_CHROMAFORMAT_YUV444     =3,
    MFX_CHROMAFORMAT_YUV400     = MFX_CHROMAFORMAT_MONOCHROME,
    MFX_CHROMAFORMAT_YUV411     = 4,
    MFX_CHROMAFORMAT_YUV422H    = MFX_CHROMAFORMAT_YUV422,
    MFX_CHROMAFORMAT_YUV422V    = 5
    
};

enum {
    MFX_TIMESTAMP_UNKNOWN = -1
};

enum {
    MFX_FRAMEORDER_UNKNOWN = -1
};

/* DataFlag in mfxFrameData */
enum {
    MFX_FRAMEDATA_ORIGINAL_TIMESTAMP = 0x0001
};

/* Corrupted in mfxFrameData */
enum {
    MFX_CORRUPTION_MINOR           = 0x0001,
    MFX_CORRUPTION_MAJOR           = 0x0002,
    MFX_CORRUPTION_REFERENCE_FRAME = 0x0010,
    MFX_CORRUPTION_REFERENCE_LIST  = 0x0020
};

/* Frame Data Info */
typedef struct {
    mfxU32      reserved[8];

    mfxU64      TimeStamp;
    mfxU32      FrameOrder;
    mfxU16      Locked;
    mfxU16      Pitch;

    /* color planes */
    union {
        mfxU8   *Y;
        mfxU8   *R;
    };
    union {
        mfxU8   *UV;            /* for UV merged formats */
        mfxU8   *VU;            /* for VU merged formats */
        mfxU8   *CbCr;          /* for CbCr merged formats */
        mfxU8   *CrCb;          /* for CrCb merged formats */
        mfxU8   *Cb;
        mfxU8   *U;
        mfxU8   *G;
    };
    union {
        mfxU8   *Cr;
        mfxU8   *V;
        mfxU8   *B;
    };
    mfxU8       *A;
    mfxMemId    MemId;

    /* Additional Flags */
    mfxU16  Corrupted;
    mfxU16  DataFlag;
} mfxFrameData;

/* Frame Surface */
typedef struct {
    mfxU32  reserved[4];
    mfxFrameInfo    Info;
    mfxFrameData    Data;
} mfxFrameSurface1;

enum {
    MFX_TIMESTAMPCALC_UNKNOWN = 0,
    MFX_TIMESTAMPCALC_TELECINE = 1,
};

/* Transcoding Info */
typedef struct {
    mfxU32  reserved[7];

    mfxU16  reserved4; 
    mfxU16  BRCParamMultiplier; 

    mfxFrameInfo    FrameInfo;
    mfxU32  CodecId;
    mfxU16  CodecProfile;
    mfxU16  CodecLevel;
    mfxU16  NumThread;

    union {
        struct {   /* MPEG-2/H.264 Encoding Options */
            mfxU16  TargetUsage;

            mfxU16  GopPicSize;
            mfxU16  GopRefDist;
            mfxU16  GopOptFlag;
            mfxU16  IdrInterval;

            mfxU16  RateControlMethod;
            union {
                mfxU16  InitialDelayInKB;
                mfxU16  QPI;
                mfxU16  Accuracy;
            };
            mfxU16  BufferSizeInKB;
            union {
                mfxU16  TargetKbps;
                mfxU16  QPP;
            };
            union {
                mfxU16  MaxKbps;
                mfxU16  QPB;
                mfxU16  Convergence;
            };  

            mfxU16  NumSlice;
            mfxU16  NumRefFrame;
            mfxU16  EncodedOrder;
        };
        struct {   /* H.264, MPEG-2 and VC-1 Decoding Options */
            mfxU16  DecodedOrder;
            mfxU16  ExtendedPicStruct;
            mfxU16 TimeStampCalc;
            mfxU16  reserved2[10];
        };
        struct {   /* JPEG Decoding Options */
            mfxU16  JPEGChromaFormat;
            mfxU16  Rotation;
            mfxU16  reserved3[11];
        };
    };
} mfxInfoMFX;

typedef struct {
    mfxU32  reserved[8];
    mfxFrameInfo    In;
    mfxFrameInfo    Out;
} mfxInfoVPP;

typedef struct {
    mfxU32  reserved[3];
    mfxU16  reserved3;
    mfxU16  AsyncDepth;

    union {
        mfxInfoMFX  mfx;
        mfxInfoVPP  vpp;
    };
    mfxU16  Protected;
    mfxU16  IOPattern;
    mfxExtBuffer** ExtParam;
    mfxU16  NumExtParam;
    mfxU16  reserved2;
} mfxVideoParam;

/* IOPattern */
enum {
    MFX_IOPATTERN_IN_VIDEO_MEMORY   = 0x01,
    MFX_IOPATTERN_IN_SYSTEM_MEMORY  = 0x02,
    MFX_IOPATTERN_IN_OPAQUE_MEMORY  = 0x04,
    MFX_IOPATTERN_OUT_VIDEO_MEMORY  = 0x10,
    MFX_IOPATTERN_OUT_SYSTEM_MEMORY = 0x20,
    MFX_IOPATTERN_OUT_OPAQUE_MEMORY = 0x40
};

/* CodecId */
enum {
    MFX_CODEC_AVC          =MFX_MAKEFOURCC('A','V','C',' '),
    MFX_CODEC_MPEG2       =MFX_MAKEFOURCC('M','P','G','2'),
    MFX_CODEC_VC1         =MFX_MAKEFOURCC('V','C','1',' ')
};

/* CodecProfile, CodecLevel */
enum {
    MFX_PROFILE_UNKNOWN                     =0,
    MFX_LEVEL_UNKNOWN                       =0,

    /* AVC Profiles & Levels */
    MFX_PROFILE_AVC_CONSTRAINT_SET0     = (0x100 << 0),
    MFX_PROFILE_AVC_CONSTRAINT_SET1     = (0x100 << 1),
    MFX_PROFILE_AVC_CONSTRAINT_SET2     = (0x100 << 2),
    MFX_PROFILE_AVC_CONSTRAINT_SET3     = (0x100 << 3),
    MFX_PROFILE_AVC_CONSTRAINT_SET4     = (0x100 << 4),
    MFX_PROFILE_AVC_CONSTRAINT_SET5     = (0x100 << 5),

    MFX_PROFILE_AVC_BASELINE                =66,
    MFX_PROFILE_AVC_MAIN                    =77,
    MFX_PROFILE_AVC_EXTENDED                =88,
    MFX_PROFILE_AVC_HIGH                    =100,
    MFX_PROFILE_AVC_CONSTRAINED_BASELINE    =MFX_PROFILE_AVC_BASELINE + MFX_PROFILE_AVC_CONSTRAINT_SET1,
    MFX_PROFILE_AVC_CONSTRAINED_HIGH        =MFX_PROFILE_AVC_HIGH     + MFX_PROFILE_AVC_CONSTRAINT_SET4
                                                                      + MFX_PROFILE_AVC_CONSTRAINT_SET5,
    MFX_PROFILE_AVC_PROGRESSIVE_HIGH        =MFX_PROFILE_AVC_HIGH     + MFX_PROFILE_AVC_CONSTRAINT_SET4,

    MFX_LEVEL_AVC_1                         =10,
    MFX_LEVEL_AVC_1b                        =9,
    MFX_LEVEL_AVC_11                        =11,
    MFX_LEVEL_AVC_12                        =12,
    MFX_LEVEL_AVC_13                        =13,
    MFX_LEVEL_AVC_2                         =20,
    MFX_LEVEL_AVC_21                        =21,
    MFX_LEVEL_AVC_22                        =22,
    MFX_LEVEL_AVC_3                         =30,
    MFX_LEVEL_AVC_31                        =31,
    MFX_LEVEL_AVC_32                        =32,
    MFX_LEVEL_AVC_4                         =40,
    MFX_LEVEL_AVC_41                        =41,
    MFX_LEVEL_AVC_42                        =42,
    MFX_LEVEL_AVC_5                         =50,
    MFX_LEVEL_AVC_51                        =51,

    /* MPEG-2 Profiles & Levels */
    MFX_PROFILE_MPEG2_SIMPLE                =0x50,
    MFX_PROFILE_MPEG2_MAIN                  =0x40,
    MFX_PROFILE_MPEG2_HIGH                  =0x10,

    MFX_LEVEL_MPEG2_LOW                     =0xA,
    MFX_LEVEL_MPEG2_MAIN                    =0x8,
    MFX_LEVEL_MPEG2_HIGH                    =0x4,
    MFX_LEVEL_MPEG2_HIGH1440                =0x6,

    /* VC1 Profiles & Levels */
    MFX_PROFILE_VC1_SIMPLE                  =(0+1),
    MFX_PROFILE_VC1_MAIN                    =(4+1),
    MFX_PROFILE_VC1_ADVANCED                =(12+1),

    /* VC1 levels for simple & main profiles */
    MFX_LEVEL_VC1_LOW                       =(0+1),
    MFX_LEVEL_VC1_MEDIAN                    =(2+1),
    MFX_LEVEL_VC1_HIGH                      =(4+1),

    /* VC1 levels for the advanced profile */
    MFX_LEVEL_VC1_0                         =(0x00+1),
    MFX_LEVEL_VC1_1                         =(0x01+1),
    MFX_LEVEL_VC1_2                         =(0x02+1),
    MFX_LEVEL_VC1_3                         =(0x03+1),
    MFX_LEVEL_VC1_4                         =(0x04+1)
};

/* GopOptFlag */
enum {
    MFX_GOP_CLOSED          =1,
    MFX_GOP_STRICT          =2
};

/* TargetUsages: from 1 to 7 inclusive */
enum {
    MFX_TARGETUSAGE_UNKNOWN         =0,
    MFX_TARGETUSAGE_BEST_QUALITY    =1,
    MFX_TARGETUSAGE_BALANCED        =4,
    MFX_TARGETUSAGE_BEST_SPEED      =7
};

/* RateControlMethod */
enum {
    MFX_RATECONTROL_CBR     =1,
    MFX_RATECONTROL_VBR     =2,
    MFX_RATECONTROL_CQP     =3,
    MFX_RATECONTROL_AVBR    =4
};

typedef struct {
    mfxExtBuffer Header;

    mfxU16      reserved1;
    mfxU16      RateDistortionOpt;      /* tri-state option */
    mfxU16      MECostType;
    mfxU16      MESearchType;
    mfxI16Pair  MVSearchWindow;
    mfxU16      EndOfSequence;          /* tri-state option */
    mfxU16      FramePicture;           /* tri-state option */

    union {
        struct {    /* AVC */
            mfxU16      CAVLC;                  /* tri-state option */
            mfxU16      reserved2[3];
            mfxU16      ViewOutput;             /* tri-state option */
            mfxU16      NalHrdConformance;      /* tri-state option */  
            mfxU16      SingleSeiNalUnit;       /* tri-state option */
            mfxU16      VuiVclHrdParameters;    /* tri-state option */
            
            mfxU16      RefPicListReordering;   /* tri-state option */
            mfxU16      ResetRefList;           /* tri-state option */
            mfxU16      RefPicMarkRep;          /* tri-state option */
            mfxU16      FieldOutput;            /* tri-state option */

            mfxU16      IntraPredBlockSize;
            mfxU16      InterPredBlockSize;
            mfxU16      MVPrecision;
            mfxU16      MaxDecFrameBuffering;   

            mfxU16      AUDelimiter;            /* tri-state option */
            mfxU16      EndOfStream;            /* tri-state option */
            mfxU16      PicTimingSEI;           /* tri-state option */
            mfxU16      VuiNalHrdParameters;    /* tri-state option */
        };
    };
} mfxExtCodingOption;

/* IntraPredBlockSize/InterPredBlockSize */
enum {
    MFX_BLOCKSIZE_UNKNOWN   = 0,
    MFX_BLOCKSIZE_MIN_16X16 = 1, /* 16x16              */
    MFX_BLOCKSIZE_MIN_8X8   = 2, /* 16x16, 8x8         */
    MFX_BLOCKSIZE_MIN_4X4   = 3  /* 16x16, 8x8, 4x4    */
};

/* MVPrecision */
enum {
    MFX_MVPRECISION_UNKNOWN    = 0,
    MFX_MVPRECISION_INTEGER    = (1 << 0),
    MFX_MVPRECISION_HALFPEL    = (1 << 1),
    MFX_MVPRECISION_QUARTERPEL = (1 << 2)
};

enum {
    MFX_CODINGOPTION_UNKNOWN    =0,
    MFX_CODINGOPTION_ON         =0x10,
    MFX_CODINGOPTION_OFF        =0x20
};

typedef struct _mfxEncryptedData mfxEncryptedData;

typedef struct {
    union {
        mfxEncryptedData* EncryptedData;
        mfxU32  reserved[8];
    };
    mfxU64  TimeStamp;
    mfxU8*  Data;
    mfxU32  DataOffset;
    mfxU32  DataLength;
    mfxU32  MaxLength;

    mfxU16  PicStruct;
    mfxU16  FrameType;
    mfxU16  DataFlag;
    mfxU16  reserved2;
} mfxBitstream;

/* Data Flag */
enum {
    MFX_BITSTREAM_COMPLETE_FRAME    = 0x0001        /* the bitstream contains a complete frame or field pair of data */
};

/* Extended Buffer Ids */
enum {
    MFX_EXTBUFF_CODING_OPTION       =   MFX_MAKEFOURCC('C','D','O','P'),
    MFX_EXTBUFF_CODING_OPTION_SPSPPS=   MFX_MAKEFOURCC('C','O','S','P'),
    MFX_EXTBUFF_VPP_DONOTUSE        =   MFX_MAKEFOURCC('N','U','S','E'),
    MFX_EXTBUFF_VPP_AUXDATA         =   MFX_MAKEFOURCC('A','U','X','D'),
    MFX_EXTBUFF_VPP_DENOISE         =   MFX_MAKEFOURCC('D','N','I','S'),
    MFX_EXTBUFF_VPP_SCENE_ANALYSIS  =   MFX_MAKEFOURCC('S','C','L','Y'),
    MFX_EXTBUFF_VPP_SCENE_CHANGE    =   MFX_EXTBUFF_VPP_SCENE_ANALYSIS,
    MFX_EXTBUFF_VPP_PROCAMP         =   MFX_MAKEFOURCC('P','A','M','P'),
    MFX_EXTBUFF_VPP_DETAIL          =   MFX_MAKEFOURCC('D','E','T',' '),
    MFX_EXTBUFF_VIDEO_SIGNAL_INFO   =   MFX_MAKEFOURCC('V','S','I','N'),
    MFX_EXTBUFF_VPP_DOUSE           =   MFX_MAKEFOURCC('D','U','S','E'),
    MFX_EXTBUFF_OPAQUE_SURFACE_ALLOCATION  = MFX_MAKEFOURCC('O','P','Q','S'),
    MFX_EXTBUFF_AVC_REFLIST_CTRL       =   MFX_MAKEFOURCC('R','L','S','T'),
    MFX_EXTBUFF_VPP_FRAME_RATE_CONVERSION  = MFX_MAKEFOURCC('F','R','C',' '),
    MFX_EXTBUFF_PICTURE_TIMING_SEI         = MFX_MAKEFOURCC('P','T','S','E'),
    MFX_EXTBUFF_AVC_TEMPORAL_LAYERS        = MFX_MAKEFOURCC('A','T','M','L')
};

/* VPP Conf: Do not use certain algorithms  */
typedef struct {
    mfxExtBuffer    Header;
    mfxU32          NumAlg;
    mfxU32*         AlgList;
} mfxExtVPPDoNotUse;

typedef struct {
    mfxExtBuffer    Header;
    mfxU16  DenoiseFactor;
} mfxExtVPPDenoise;

typedef struct {
    mfxExtBuffer    Header;
    mfxU16  DetailFactor;
} mfxExtVPPDetail;

typedef struct {
    mfxExtBuffer    Header;
    mfxF64   Brightness;
    mfxF64   Contrast;
    mfxF64   Hue;
    mfxF64   Saturation;
} mfxExtVPPProcAmp;

/* statistics collected for decode, encode and vpp */
typedef struct {
    mfxU32  reserved[16];
    mfxU32  NumFrame;
    mfxU64  NumBit;
    mfxU32  NumCachedFrame;
} mfxEncodeStat;

typedef struct {
    mfxU32  reserved[16];
    mfxU32  NumFrame;
    mfxU32  NumSkippedFrame;
    mfxU32  NumError;
    mfxU32  NumCachedFrame;
} mfxDecodeStat;

typedef struct {
    mfxU32  reserved[16];
    mfxU32  NumFrame;
    mfxU32  NumCachedFrame;
} mfxVPPStat;

typedef struct {
    mfxExtBuffer    Header;
    mfxU32          SpatialComplexity;
    mfxU32          TemporalComplexity;
    mfxU16          SceneChangeRate;
    mfxU16          RepeatedFrame;
} mfxExtVppAuxData;

typedef struct {
    mfxU32      reserved[4];
    mfxU8       *Data;      /* buffer pointer */
    mfxU32      NumBit;     /* number of bits */
    mfxU16      Type;       /* SEI message type in H.264 or user data start_code in MPEG-2 */
    mfxU16      BufSize;    /* payload buffer size in bytes */
} mfxPayload;

typedef struct {
    mfxExtBuffer    Header;
    mfxU32  reserved[5];
    mfxU16  reserved1;

    mfxU16  QP; /* per frame QP */

    mfxU16  FrameType;
    mfxU16  NumExtParam;
    mfxU16  NumPayload;     /* MPEG-2 user data or H.264 SEI message(s) */
    mfxU16  reserved2;

    mfxExtBuffer    **ExtParam;
    mfxPayload      **Payload;      /* for field pair, first field uses even payloads and second field uses odd payloads */
} mfxEncodeCtrl;

/* Buffer Memory Types */
enum {
    /* Buffer types */
    MFX_MEMTYPE_PERSISTENT_MEMORY   =0x0002
};

/* Frame Memory Types */
#define MFX_MEMTYPE_BASE(x) (0xf0ff & (x))

enum {
    MFX_MEMTYPE_DXVA2_DECODER_TARGET       =0x0010,
    MFX_MEMTYPE_DXVA2_PROCESSOR_TARGET     =0x0020,
    MFX_MEMTYPE_VIDEO_MEMORY_DECODER_TARGET   = MFX_MEMTYPE_DXVA2_DECODER_TARGET,
    MFX_MEMTYPE_VIDEO_MEMORY_PROCESSOR_TARGET = MFX_MEMTYPE_DXVA2_PROCESSOR_TARGET,
    MFX_MEMTYPE_SYSTEM_MEMORY              =0x0040,

    MFX_MEMTYPE_FROM_ENCODE     = 0x0100,
    MFX_MEMTYPE_FROM_DECODE     = 0x0200,
    MFX_MEMTYPE_FROM_VPPIN      = 0x0400,
    MFX_MEMTYPE_FROM_VPPOUT     = 0x0800,

    MFX_MEMTYPE_INTERNAL_FRAME  = 0x0001,
    MFX_MEMTYPE_EXTERNAL_FRAME  = 0x0002,
    MFX_MEMTYPE_OPAQUE_FRAME    = 0x0004
};

typedef struct {
    mfxU32  reserved[4];
    mfxFrameInfo    Info;
    mfxU16  Type;   /* decoder or processor render targets */
    mfxU16  NumFrameMin;
    mfxU16  NumFrameSuggested;
    mfxU16  reserved2;
} mfxFrameAllocRequest;

typedef struct {
    mfxU32      reserved[4];
    mfxMemId    *mids;      /* the array allocated by application */
    mfxU16      NumFrameActual;
    mfxU16      reserved2;
} mfxFrameAllocResponse;

/* FrameType */
enum {
    MFX_FRAMETYPE_UNKNOWN       =0x0000,

    MFX_FRAMETYPE_I             =0x0001,
    MFX_FRAMETYPE_P             =0x0002,
    MFX_FRAMETYPE_B             =0x0004,
    MFX_FRAMETYPE_S             =0x0008,

    MFX_FRAMETYPE_REF           =0x0040,
    MFX_FRAMETYPE_IDR           =0x0080,

    MFX_FRAMETYPE_xI            =0x0100,
    MFX_FRAMETYPE_xP            =0x0200,
    MFX_FRAMETYPE_xB            =0x0400,
    MFX_FRAMETYPE_xS            =0x0800,

    MFX_FRAMETYPE_xREF          =0x4000,
    MFX_FRAMETYPE_xIDR          =0x8000
};

typedef enum {
    MFX_HANDLE_DIRECT3D_DEVICE_MANAGER9         =1,      /* IDirect3DDeviceManager9      */
    MFX_HANDLE_D3D9_DEVICE_MANAGER              = MFX_HANDLE_DIRECT3D_DEVICE_MANAGER9,
    MFX_HANDLE_D3D11_DEVICE_AND_CONTEXT         = 2,
    MFX_HANDLE_D3D11_DEVICE                     = 3,
    MFX_HANDLE_VA_DISPLAY                       = 4
} mfxHandleType;

typedef enum {
    MFX_SKIPMODE_NOSKIP=0,
    MFX_SKIPMODE_MORE=1,
    MFX_SKIPMODE_LESS=2
} mfxSkipMode;

/* Library initialization and deinitialization */
typedef mfxI32 mfxIMPL;
#define MFX_IMPL_BASETYPE(x) (0x00ff & (x)) 

enum  {
    MFX_IMPL_AUTO         = 0x0000,  /* Auto Selection/In or Not Supported/Out */
    MFX_IMPL_SOFTWARE     = 0x0001,  /* Pure Software Implementation */
    MFX_IMPL_HARDWARE     = 0x0002,  /* Hardware Accelerated Implementation (default device) */
    MFX_IMPL_AUTO_ANY     = 0x0003,  /* Auto selection of any hardware/software implementation */
    MFX_IMPL_HARDWARE_ANY = 0x0004,  /* Auto selection of any hardware implementation */
    MFX_IMPL_HARDWARE2    = 0x0005,  /* Hardware accelerated implementation (2nd device) */
    MFX_IMPL_HARDWARE3    = 0x0006,  /* Hardware accelerated implementation (3rd device) */
    MFX_IMPL_HARDWARE4    = 0x0007,  /* Hardware accelerated implementation (4th device) */

    MFX_IMPL_VIA_ANY      = 0x0100,
    MFX_IMPL_VIA_D3D9     = 0x0200, 
    MFX_IMPL_VIA_D3D11    = 0x0300, 

    MFX_IMPL_UNSUPPORTED  = 0x0000  /* One of the MFXQueryIMPL returns */
};

/* Version Info */
typedef union {
    struct {
        mfxU16  Minor;
        mfxU16  Major;
    };
    mfxU32  Version;
} mfxVersion;

typedef struct {
    mfxExtBuffer    Header;
    mfxU8           *SPSBuffer;
    mfxU8           *PPSBuffer;
    mfxU16          SPSBufSize;
    mfxU16          PPSBufSize;
    mfxU16          SPSId;
    mfxU16          PPSId;
} mfxExtCodingOptionSPSPPS;

/* session priority */
typedef enum
{
    MFX_PRIORITY_LOW = 0,
    MFX_PRIORITY_NORMAL = 1,
    MFX_PRIORITY_HIGH = 2

} mfxPriority;

typedef struct {
    mfxExtBuffer    Header;
    mfxU16          VideoFormat;
    mfxU16          VideoFullRange;
    mfxU16          ColourDescriptionPresent;
    mfxU16          ColourPrimaries;
    mfxU16          TransferCharacteristics;
    mfxU16          MatrixCoefficients;
} mfxExtVideoSignalInfo;

typedef struct {
    mfxExtBuffer    Header;
    mfxU32          NumAlg;
    mfxU32          *AlgList;
} mfxExtVPPDoUse;

typedef struct {
    mfxExtBuffer    Header;
    mfxU32      reserved1[2];
    struct {
        mfxFrameSurface1 **Surfaces;
        mfxU32  reserved2[5];
        mfxU16  Type;
        mfxU16  NumSurface;
    } In, Out;  
} mfxExtOpaqueSurfaceAlloc;

typedef struct {
    mfxExtBuffer    Header;
    mfxU16          NumRefIdxL0Active;
    mfxU16          NumRefIdxL1Active;

    struct {
        mfxU32      FrameOrder;
        mfxU16      PicStruct;
        mfxU16      ViewId;
        mfxU32      reserved[2];
    } PreferredRefList[32], RejectedRefList[16], LongTermRefList[16];

    mfxU32      reserved[8];
} mfxExtAVCRefListCtrl;

enum {
    MFX_FRCALGM_PRESERVE_TIMESTAMP = 1,
    MFX_FRCALGM_DISTRIBUTED_TIMESTAMP = 2
};

typedef struct {
    mfxExtBuffer    Header;
    mfxU16      Algorithm;
    mfxU16      reserved;
    mfxU32      reserved2[15];
} mfxExtVPPFrameRateConversion;

typedef struct {
  mfxExtBuffer    Header;
  mfxU32      reserved[14];

  struct {
      mfxU16    ClockTimestampFlag;
      mfxU16    CtType;
      mfxU16    NuitFieldBasedFlag;
      mfxU16    CountingType;
      mfxU16    FullTimestampFlag;
      mfxU16    DiscontinuityFlag;
      mfxU16    CntDroppedFlag;
      mfxU16    NFrames;
      mfxU16    SecondsFlag;
      mfxU16    MinutesFlag;
      mfxU16    HoursFlag;
      mfxU16    SecondsValue;
      mfxU16    MinutesValue;
      mfxU16    HoursValue;
      mfxU32    TimeOffset;
  } TimeStamp[3];
} mfxExtPictureTimingSEI;

typedef struct {
    mfxExtBuffer    Header;
    mfxU32          reserved1[4];
    mfxU16          reserved2;
    mfxU16          BaseLayerPID;

    struct {
        mfxU16 Scale;
        mfxU16 reserved[3];
    }Layer[8];
} mfxExtAvcTemporalLayers;

#ifdef __cplusplus
} // extern "C" 
#endif

#endif

