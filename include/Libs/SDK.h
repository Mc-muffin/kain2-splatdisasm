#ifndef _SDK_H_
#define _SDK_H_

/*TODO: Remove LIBS/ when adding PSYQ headers*/

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef ABS
#define ABS(a) ((a) < 0 ? -(a) : (a))
#endif

#define getScratchAddr(offset)  ((unsigned long *)(0x1f800000+(offset)*4))

#define getTPage(tp, abr, x, y) 					\
	 ((((tp)&0x3)<<7)|(((abr)&0x3)<<5)|(((y)&0x100)>>4)|(((x)&0x3ff)>>6)| \
	 (((y)&0x200)<<2))

#define getClut(x, y) \
	(((y)<<6)|(((x)>>4)&0x3f))

// GTE macros

#define gte_SetRotMatrix(r0) __asm__ ( \
        "lw    $12, 0(%0);" \
        "lw    $13, 4(%0);" \
        "ctc2    $12, $0;" \
        "ctc2    $13, $1;" \
        "lw    $12, 8(%0);" \
        "lw    $13, 12(%0);" \
        "lw    $14, 16(%0);" \
        "ctc2    $12, $2;" \
        "ctc2    $13, $3;" \
        "ctc2    $14, $4" \
        : : "r"(r0) \
        : "$12", "$13", "$14" \
    )

#define gte_ldv0(r0) __asm__ ( \
        "lwc2    $0, 0(%0);" \
        "lwc2    $1, 4(%0)" \
        : : "r"(r0) \
    )

#define gte_nmvmva(sf, mx, v, cv, lm) __asm__ ( \
        "nop;" \
        "nop;" \
        ".word %0" \
        : : "g"(0x4A400012 | ((sf) & 0x1) << 19 | ((mx) & 0x3) << 17 | ((v) & 0x3) << 15 | ((cv) & 0x3) << 13 | ((lm) & 0x1) << 10) \
    )

#define gte_mvmva(sf, mx, v, cv, lm) __asm__ ( \
        ".word %0" \
        : : "g"(0x4A400012 | ((sf) & 0x1) << 19 | ((mx) & 0x3) << 17 | ((v) & 0x3) << 15 | ((cv) & 0x3) << 13 | ((lm) & 0x1) << 10) \
    )

#define gte_stlvnl(r0) __asm__ ( \
        "swc2    $25, 0(%0);" \
        "swc2    $26, 4(%0);" \
        "swc2    $27, 8(%0)" \
        : : "r"(r0) \
        : "memory" \
    )

#define gte_nrtv0() gte_nmvmva(1, 0, 0, 3, 0)

#define gte_ldlvl(r0) __asm__ ( \
        "lwc2    $9, 0(%0);" \
        "lwc2    $10, 4(%0);" \
        "lwc2    $11, 8(%0)" \
        : : "r"(r0) \
    )

#define gte_nsqr(sf) __asm__ ( \
        "nop;" \
        "nop;" \
        ".word %0" \
        : : "g"(0x4AA00428 | ((sf) & 0x1) << 19) \
    )

#define gte_nsqr0() gte_nsqr(0)

#define gte_sqr(sf) __asm__ ( \
        ".word %0" \
        : : "g"(0x4AA00428 | ((sf) & 0x1) << 19) \
    )

#define gte_sqr0() gte_sqr(0)

#define gte_nrtps() __asm__ ( \
        "nop;" \
        "nop;" \
        ".word 0x4A180001" \
    )

#define gte_stsxy(r0) __asm__ ( \
        "swc2    $14, 0(%0)" \
        : : "r"(r0) \
        : "memory" \
    )

// size: 0x8
typedef struct RECT {
    // offset: 0000
    short x;
    // offset: 0002
    short y;
    // offset: 0004
    short w;
    // offset: 0006
    short h;
} RECT;

// size: 0x14
typedef struct DISPENV {
    // offset: 0000 (8 bytes)
    struct RECT disp;
    // offset: 0008 (8 bytes)
    struct RECT screen;
    // offset: 0010
    unsigned char isinter;
    // offset: 0011
    unsigned char isrgb24;
    // offset: 0012
    unsigned char pad0;
    // offset: 0013
    unsigned char pad1;
} DISPENV;

// size: 0x40
typedef struct DR_ENV {
    // offset: 0000
    unsigned long tag;
    // offset: 0004 (60 bytes)
    unsigned long code[15];
} DR_ENV;

// size: 0x5C
typedef struct DRAWENV {
    // offset: 0000 (8 bytes)
    struct RECT clip;
    // offset: 0008 (4 bytes)
    short ofs[2];
    // offset: 000C (8 bytes)
    struct RECT tw;
    // offset: 0014
    unsigned short tpage;
    // offset: 0016
    unsigned char dtd;
    // offset: 0017
    unsigned char dfe;
    // offset: 0018
    unsigned char isbg;
    // offset: 0019
    unsigned char r0;
    // offset: 001A
    unsigned char g0;
    // offset: 001B
    unsigned char b0;
    // offset: 001C (64 bytes)
    struct DR_ENV dr_env;
} DRAWENV;

// size: 0xC
typedef struct DR_STP {
    // offset: 0000
    unsigned long tag;
    // offset: 0004 (8 bytes)
    unsigned long code[2];
} DR_STP;

typedef unsigned short PadData;

typedef struct
{
    unsigned short buttons;
    signed char xOffset;
    signed char yOffset;
}
MouseData;

typedef struct
{
    unsigned short digitalButtons;               /* Bit mask of plain keys. */
    char centralTwist;                           /* Analogue twisting thing.*/
    char buttonI;                                /* The I analogue button.  */
    char buttonII;                               /* The II analogue button. */
    char topLeft;                                /* The analogue tl button. */
}
NegconData;

typedef struct
{
    unsigned char transStatus;  /* 0xff = no pad, bad pad, bad transmission */
    unsigned char dataFormat;            /* Top 4 bits = type of controller */

    union                                         /* Controller data union. */                   /* Controller data union. */
    {
        PadData    pad;                                 /* Plain pad.       */
        NegconData negcon;                              /* Namco controller.*/
    }
    data;
}
TapCtrllerData;

typedef struct
{
    TapCtrllerData ctrllers[4];               /* Just 4 controller packets. */
}
MultiTapData;

typedef struct
{
    unsigned char transStatus;  /* 0xff = no pad, bad pad, bad transmission */
    unsigned char dataFormat;            /* Top 4 bits = type of controller */
    /* Bottom 4 == shorts of data written */
    union                                         /* Controller data union. */
    {
        PadData      pad;                               /* Plain pad.       */
        MouseData    mouse;                             /* Mouse.           */
        NegconData   negcon;                            /* Namco controller.*/
        MultiTapData tap;                               /* 4-way multi-tap. */
    }
    data;
}
ControllerPacket;

typedef void (*CdlCB)();

// size: 0x4
typedef struct CdlLOC {
    // offset: 0000
    unsigned char minute;
    // offset: 0001
    unsigned char second;
    // offset: 0002
    unsigned char sector;
    // offset: 0003
    unsigned char track;
} CdlLOC;

// size: 0x18
typedef struct CdlFILE {
    // offset: 0000 (4 bytes)
    struct CdlLOC pos;
    // offset: 0004
    unsigned long size;
    // offset: 0008 (16 bytes)
    char name[16];
} CdlFILE;

// size: 0x4
typedef struct CdlFILTER {
    // offset: 0000
    unsigned char file;
    // offset: 0001
    unsigned char chan;
    // offset: 0002
    unsigned short pad;
} CdlFILTER;

// size: 0x4
typedef struct SpuVolume {
    // offset: 0000
    short left;
    // offset: 0002
    short right;
} SpuVolume;

// size: 0xC
typedef struct SpuExtAttr {
    // offset: 0000 (4 bytes)
    struct SpuVolume volume;
    // offset: 0004
    long reverb;
    // offset: 0008
    long mix;
} SpuExtAttr;

// size: 0x28
typedef struct SpuCommonAttr {
    // offset: 0000
    unsigned long mask;
    // offset: 0004 (4 bytes)
    struct SpuVolume mvol;
    // offset: 0008 (4 bytes)
    struct SpuVolume mvolmode;
    // offset: 000C (4 bytes)
    struct SpuVolume mvolx;
    // offset: 0010 (12 bytes)
    struct SpuExtAttr cd;
    // offset: 001C (12 bytes)
    struct SpuExtAttr ext;
} SpuCommonAttr;

int rand();
void ApplyMatrix(MATRIX *, SVECTOR *, VECTOR *);
void ApplyMatrixSV(MATRIX *, SVECTOR *, SVECTOR *);
void ApplyMatrixLV(MATRIX *, VECTOR *, VECTOR *);
void TransposeMatrix(MATRIX *, MATRIX *);
void EnterCriticalSection();
void ExitCriticalSection();
int GetRCnt(int);
void ClearOTagR(unsigned long *, int);
void RotMatrix(SVECTOR *, MATRIX *);
long ratan2(long y, long x);
MATRIX *RotMatrixX(long r, MATRIX *m);
MATRIX *RotMatrixY(long r, MATRIX *m);
MATRIX *RotMatrixZ(long r, MATRIX *m);
MATRIX *RotMatrixZYX(SVECTOR *r, MATRIX *m);
MATRIX *MulMatrix0(MATRIX *m0, MATRIX *m1, MATRIX *m2);
MATRIX *MulMatrix2(MATRIX *m0, MATRIX *m1);
void SetBackColor(long rbk, long gbk, long bbk);
void SetGeomScreen(long h);
void SetFogNearFar(long a, long b, long h);
int checkagain();
void PopMatrix();
void SetTransMatrix(MATRIX *m);
void SetRotMatrix(MATRIX *m);
void PushMatrix();
int rcos(int);
int rsin(int);
void ScaleMatrix(MATRIX *, VECTOR *);
DISPENV *SetDefDispEnv(DISPENV *env, int x, int y, int w, int h);
u_long *BreakDraw(void);
int DrawSync(int mode);
int LoadImage(RECT *rect, u_long *p);
void DrawOTag(u_long *p);
int MoveImage(RECT *rect, int x, int y);
void SetDrawStp(DR_STP *p, int pbw);
void DrawPrim(void *p);
DRAWENV *PutDrawEnv(DRAWENV *env);
int VSyncCallback(void (*f)(void));
CdlCB CdSyncCallback(CdlCB func);
int CdPosToInt(CdlLOC *p);
CdlFILE *CdSearchFile(CdlFILE *fp, char *name);
int CdControl(u_char com, u_char *param, u_char *result);
CdlCB CdSyncCallback(CdlCB func);
void SpuSetCommonAttr(SpuCommonAttr *attr);

#endif
