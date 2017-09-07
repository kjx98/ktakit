/********************************************************
 *														*
 *	Technical Analysis Trade System 					*
 *														*
 *		(C) Copyright									*
 *			Jesse Kuang									*
 *														*
 *														*
 *	$Revision$											*
 *	$Log$												*
 *														*
 *														*
 *														*
 ********************************************************/
/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997-2015 KTA */

#ifndef __KTAKIT_H__
#define __KTAKIT_H__


#include "ktadefs.h"

#ifdef  KFLOAT_DOUBLE
typedef double  KFloat;
#else
typedef float   KFloat;
#endif // KFLOAT_DOUBLE


#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------- */
/* tareturn                                                             */
/* -------------------------------------------------------------------- */

#define taSuccess                0
#define taErrSuccess             0
#define taErrInvalidParm        -1
#define taErrFileOpen           -2
#define taErrMalloc             -3
#define taErrFileRead           -4
#define taErrFileWrite          -5
#define taErrFileSeek           -6
#define taErrFileExists         -7
#define taErrFileNotFound       -8
#define taErrRecordNotFound     -9
#define taChartErrNoGraphics   -10
#define taChartErrNoFonts      -11
#define taChartErrUndefined    -12
#define taChartErrMaxlines     -13
#define taChartErrMaxgrids     -14

/* -------------------------------------------------------------------- */
/* taarray                                                              */
/* -------------------------------------------------------------------- */

typedef struct
{
 KFloat   *data;
 int	size;
 int    width;
 int    element;
 int    type; /* 0=KFloat, 1=int */
} taArray;

typedef struct
{
 int32	*data;
 int	size;
 int    width;
 int    element;
 int    type; /* 0=KFloat, 1=int */
} taArrayI;

typedef struct
{
 taArrayI dt, ti;
 taArray  op, hi, lo, cl, vol, turnov; // to for turnover equity
 taArrayI oi,sprd; // spread for FOREX,Option etc
 unsigned int    size;
 int    datasize;
 long   reccnt;
 int    frequency;      /* I D W M Q Y */
 int    datatype;       /* C S M O B X F */
 int    optiontype;     /* C P */
 int    period;         /* Intraday , period in seconds, such as 60 for M1, 300 for M5 */
 KFloat strikeprice;
 char   name[32];
} taBars;

typedef struct
{
 taArrayI dt, ti;
 taArray  bid, ask, bvol, avol, vol;
 unsigned int    size;
 int    datasize;
 long   reccnt;
 char   symbol[32];
} taTicks;


extern taArray  taNOARRAY;
extern taArray *taNULLARRAY;
extern taBars   taNOBARS;
extern taBars  *taNULLBARS;

#define taIsNullBars(b1) (b1->dt.data==NULL)
#define taIsNullArray(a1) (a1->data==NULL)
#define taIsNoBars(b1) (b1.dt.data==NULL)
#define taIsNoArray(a1) (a1.data==NULL)

#define taArrayItem(a1,pos) a1.data[(pos)*a1.width+a1.element]
#define taArrayItemP(a1,pos) a1->data[(pos)*a1->width+a1->element]
#define taArrayItemM(a1,el,pos) a1.data[(pos)*a1.width+(el)]
#define taArrayItemPM(a1,el,pos) a1->data[(pos)*a1->width+(el)]
#define taBarSize(b1) (b1.size)
#define taBarSizeP(b1) (b1->size)

#define SIZEOFBARDATA (4*sizeof(int)+6*sizeof(KFloat))

/* file type */
#define taCSI 1
#define taCT  2
#define taDJ  3
#define taFS  4
#define taTC  5
#define taTT  6
#define taTXT 7
#define taMT4 8

/* frequency */
#define taUNKNOWN    -1
#define taINTRADAY   0
#define taDAILY      1
#define taWEEKLY     2
#define taMONTHLY    3
#define taQUARTERLY  4
#define taYEARLY     5

/* datatype */
#define taBOND          1
#define taCOMMODITY     2
#define taMUTUALFUND    3
#define taOPTION        4
#define taSTOCK         5
#define taINDEX         6
#define taFOREX         7
#define	taFUTURES	    8

/* optiontype */
#define taCALL       1
#define taPUT        2

int DLL_EXPORT taAllocArray(taArray *a1, size_t size);
int DLL_EXPORT taAllocArrayI(taArrayI *a1, size_t size);
int DLL_EXPORT taAllocArrayM(taArray *a1, int width, size_t size);
int DLL_EXPORT taAllocArrayIM(taArrayI *a1, int width, size_t size);
int DLL_EXPORT taAllocBars(taBars *b1, size_t size);
long DLL_EXPORT taDecimals(KFloat value, int places);
void DLL_EXPORT taFreeArray(taArray *a1);
void DLL_EXPORT taFreeArrayI(taArrayI *a1);
void DLL_EXPORT taFreeBars(taBars *b1);
KFloat DLL_EXPORT taGetArrayItem(taArray *a1, unsigned int pos);
KFloat DLL_EXPORT taGetArrayItemM(taArray *a1, int element, unsigned int pos);
int DLL_EXPORT taIEEEtoMS(float *ieee, float *ms);
int DLL_EXPORT taMStoIEEE(float *ms, float *ieee);
int DLL_EXPORT taReAllocBars(taBars *b1, size_t size);
int DLL_EXPORT taReSizeBars(taBars *b1, size_t size);
void DLL_EXPORT taSetArrayItem(taArray *a1, unsigned int pos, KFloat value);
void DLL_EXPORT taSetArrayItemM(taArray *a1, int element, unsigned int pos, KFloat value);
int DLL_EXPORT taErrDesc(int err, char *buffer);

void taFree(void *buffer);
void *taMalloc(unsigned int size);

#ifdef _WINDOWS
int sprintf(char *text, const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);
#endif
/* -------------------------------------------------------------------- */
/* tadate                                                               */
/* -------------------------------------------------------------------- */

#define DTFORMAT_YY_MM_DD    1
#define DTFORMAT_CCYY_MM_DD  2
#define DTFORMAT_MM_DD_YY    3
#define DTFORMAT_MM_DD_CCYY  4
#define DTFORMAT_DD_MM_YY    5
#define DTFORMAT_DD_MM_CCYY  6

#define DTFORMAT_YYMMDD      7
#define DTFORMAT_CCYYMMDD    8
#define DTFORMAT_MMDDYY      9
#define DTFORMAT_MMDDCCYY   10
#define DTFORMAT_DDMMYY     11
#define DTFORMAT_DDMMCCYY   12

#define TIFORMAT_HH_MM_SS    1
#define TIFORMAT_HH_MM       2
#define TIFORMAT_HHMMSS      3
#define TIFORMAT_HHMM        4

unsigned int DLL_EXPORT taDateTxtDif(char *date1, char *date2, int format1, int format2);
unsigned int DLL_EXPORT taDateTxtToJulian(char *date, int format);
int          DLL_EXPORT taDateTxtVal(char *date, int format);
unsigned int DLL_EXPORT taHMSDif(int hr1, int min1, int sec1, int hr2, int min2, int sec2);
unsigned int DLL_EXPORT taHMSToJulian(int hr, int min, int sec);
unsigned int DLL_EXPORT taJulianDif(unsigned int dti1, unsigned int dti2);
int          DLL_EXPORT taJulianDOW(unsigned int date);
void        DLL_EXPORT taJulianToDateTxt(unsigned int date, int format, char *sep, char *buffer);
void        DLL_EXPORT taJulianToHMS(unsigned int time, int *hr, int *min, int *sec);
void        DLL_EXPORT taJulianToTimeTxt(unsigned int time, int format, char *sep, char *buffer);
void        DLL_EXPORT taJulianToYMD(unsigned int date, int *year, int *month, int *day);
unsigned int DLL_EXPORT taTimeTxtDif(char *time1, char *time2, int format1, int format2);
unsigned int DLL_EXPORT taTimeTxtToJulian(char *time, int format);
unsigned int DLL_EXPORT taYMDDif(int year1, int month1, int day1, int year2, int month2, int day2);
unsigned int DLL_EXPORT taYMDToJulian(int year, int month, int day);
/* -------------------------------------------------------------------- */
/* tastr                                                                */
/* -------------------------------------------------------------------- */

int     taChrCnt(char *string, char chr);
int     taChrsCnt(char *string, char *chrs);
void    taLtrim(char *string);
void    taRtrim(char *string);
void    taTrim(char *string);
int     taStrCpyChr(char *to, char *from, char chr);
int     taStrNthChr(char *str, char chr, int n);

/* -------------------------------------------------------------------- */
/* taio                                                                 */
/* -------------------------------------------------------------------- */

int DLL_EXPORT taPNFchart(taBars *pnfbars, KFloat box, char *outfile, int append);
int DLL_EXPORT taReadDOPfile(char *filename, int *cols);
int DLL_EXPORT taTXT2Array(char *infile, taArray *a1, char *delimit, int skip, int startcol, int stopcol);

#if !defined(_WINDOWS) || defined(_WIN32)
int DLL_EXPORT taArrayPrintf(char *outfile, int append, int start, int stop, char *heading, char *format, ...);
#endif

/* -------------------------------------------------------------------- */
/* tacalc                                                               */
/* -------------------------------------------------------------------- */

#define taZERO   0
#define taUP     1
#define taDOWN  -1
#define taTRUE   1
#define taFALSE  0

#define BY_DAY   1
#define BY_WEEK  2
#define BY_MONTH 3

#define ExpMA      0
#define LstSqrMA   1
#define SimpleMA   2
#define WeightedMA 3
#define WellesMA   4

int DLL_EXPORT taAccumulate(taArray *a1, taArray *a2, int start);
int DLL_EXPORT taADXR(taBars *b1, taArray *a2, int term1, int term2, int term3, int start);
int DLL_EXPORT taADX(taBars *b1, taArray *a2, int term1, int term2, int start);
int DLL_EXPORT taArmsEMV(taBars *b1, taArray *a2, long volinc, int start);
int DLL_EXPORT taArmsIdx(taArray *a1, taArray *a2, taArray *a3, taArray *a4, taArray *a5, int start, int stop);
int DLL_EXPORT taArrayStats(taArray *a1, KFloat *mini, KFloat *maxi, KFloat *avg, KFloat *sum, int start, int stop, int nonzero);
int DLL_EXPORT taArrayStatsPos(taArray *a1, int *mini, int *maxi, int start, int stop, int nonzero);
int DLL_EXPORT taATR(taBars *b1, taArray *a2, int term, int start);
KFloat DLL_EXPORT taAvgPrice(taBars *b1, int pos);
int DLL_EXPORT taBars2PNF(taBars *b1, taBars *b2, taArray *index, KFloat box, KFloat rev, int start);
int DLL_EXPORT taBars2PNFhilo(taBars *b1, taBars *b2, taArray *index, KFloat box, KFloat rev, int start);
int DLL_EXPORT taBarStats(taBars *b1, KFloat *min1, KFloat *max1, int start, int stop);
int DLL_EXPORT taBollinger(taBars *b1, taArray *a2, taArray *a3, taArray *a4, KFloat factor, int term, int start);
int DLL_EXPORT taBoltonTremblay(taArray *a1, taArray *a2, taArray *a3, taArray *a4, int start);
int DLL_EXPORT taCCI(taBars *b1, taArray *a2, int term, int start);
int DLL_EXPORT taCFTPP(taBars *b1, taArray *a2, taArray *a3, taArray *a4, taArray *a5, int start);
int DLL_EXPORT taChaikinMoneyFlow(taBars *b1, taArray *a2, int term, int start);
KFloat DLL_EXPORT taCorCoef(taArray *a1, taArray *a2, int samp, int start, int stop);
int DLL_EXPORT taCorCoefMv(taArray *a1, taArray *a2, taArray *a3, int samp, int term, int start);
KFloat DLL_EXPORT taCovariance(taArray *a1, taArray *a2, int start, int stop);
int DLL_EXPORT taCovarianceMv(taArray *a1, taArray *a2, taArray *a3, int term, int start);
int DLL_EXPORT taDemandIdx(taBars *b1, taArray *a2, int term, int stock, int start);
int DLL_EXPORT taDeTrend(taArray *a1, taArray *a2, int start, int stop);
int DLL_EXPORT taDI(taBars *b1, taArray *a2, taArray *a3, int term, int start);
KFloat DLL_EXPORT taDispersion(taArray *a1, int start, int stop);
int DLL_EXPORT taDispersionMv(taArray *a1, taArray *a2, int term, int start);
int DLL_EXPORT taDMI(taBars *b1, taArray *a2, taArray *a3, int start);
int DLL_EXPORT taDownAverage(taArray *a1, taArray *a2, int term, int start);
int DLL_EXPORT taDX(taBars *b1, taArray *a2, int term, int start);
int DLL_EXPORT taEnvelope(taArray *a1, taArray *a2, taArray *a3, taArray *a4, KFloat pct, int start);
int DLL_EXPORT taEnvelopePct(taArray *a1, taArray *a2, taArray *a3, KFloat pct, int start);
int DLL_EXPORT taExpMA(taArray *a1, taArray *a2, int term, int start);
KFloat DLL_EXPORT taExpToPeriod(KFloat e1);
int DLL_EXPORT taFastD(taBars *b1, taArray *a2, int term1, int term2, int start);
int DLL_EXPORT taFibonacciProject(KFloat f1, KFloat f2, KFloat *project1, KFloat *project2, KFloat *project3);
int DLL_EXPORT taFibonacciRetrace(KFloat f1, KFloat f2, KFloat *retrace1, KFloat *retrace2, KFloat *retrace3);
int DLL_EXPORT taFillInterval(taArray *a1, taArray *a2, taArray *a3, int start);
int DLL_EXPORT taGroupBars(taBars *b1, KFloat *op, KFloat *hi, KFloat *lo, KFloat *cl, KFloat *vol, KFloat *to, int term, int start);
int DLL_EXPORT taGroupBarsArray(taBars *b1, taBars *b2, int term, int start);
int DLL_EXPORT taGroupBarsDate(taBars *b1, taBars *b2, int period, unsigned int startdt, unsigned int startti, unsigned int enddt, unsigned int endti);
int DLL_EXPORT taGroupBarsTime(taBars *b1, taBars *b2, unsigned int period, unsigned int startdt, unsigned int startti, unsigned int enddt, unsigned int endti);
int DLL_EXPORT taHerrickPayoffIndex(taBars *b1, taArray *a2, KFloat mult, KFloat factor, int start);
int DLL_EXPORT taLastHigh(taArray *a1, int start, int stop);
int DLL_EXPORT taLastLow(taArray *a1, int start, int stop);
int DLL_EXPORT taLineOsc(taArray *a1, taArray *a2, taArray *a3, int term, int start);
int DLL_EXPORT taLstSqrMA(taArray *a1, taArray *a2, int term, int start);
int DLL_EXPORT taMACD(taArray *a1, taArray *a2, taArray *a3, taArray *a4, taArray *a5, taArray *a6, int term1, int term2, int term3, int start);
int DLL_EXPORT taMAEnvelope(taArray *a1, taArray *a2, taArray *a3, KFloat pct, int type, int term, int start);
int DLL_EXPORT taMAHighLow(taBars *b1, taArray *a2, taArray *a3, int type, int term, int start);
int DLL_EXPORT taMcClellanOscSum(taArray *a1, taArray *a2, taArray *a3, taArray *a4, int term1, int term2, int start);
int DLL_EXPORT taMFI(taBars *b1, taArray *a2, int start);
int DLL_EXPORT taMomentum(taArray *a1, taArray *a2, int term, int start);
int DLL_EXPORT taMovingSum(taArray *a1, taArray *a2, int term, int start);
KFloat DLL_EXPORT taOBOi(taBars *b1, int start, int stop);
int DLL_EXPORT taOBOiMv(taBars *b1, taArray *a2, int term, int start);
KFloat DLL_EXPORT taOBVol(taBars *b1, int start, int stop);
int DLL_EXPORT taOBVolExp(taBars *b1, taArray *obv, taArray *breakout, taArray *fieldtrend, long seedvol, int start);
int DLL_EXPORT taOBVolMv(taBars *b1, taArray *a2, int term, int start);
int DLL_EXPORT taOscillator(taArray *a1, taArray *a2, taArray *a3, int start);
KFloat DLL_EXPORT taPeriodToExp(KFloat n1);
int DLL_EXPORT taRateOfChange(taArray *a1, taArray *a2, int factor, int term, int start);
int DLL_EXPORT taRatio(taArray *a1, taArray *a2, taArray *a3, int start);
int DLL_EXPORT taRawK(taBars *b1, taArray *a2, int term, int start);
int DLL_EXPORT taRegressionLine(taArray *a1, KFloat *slope, KFloat *constant, int start, int stop);
int DLL_EXPORT taRegressionLineMv(taArray *a1, taArray *a2, taArray *a3, int term, int start, int stop);
int DLL_EXPORT taRSI(taArray *a1, taArray *a2, int term, int start);
int DLL_EXPORT taRSIC(taBars *b1, taBars *b2, taArray *a1, int term, unsigned int startdate);
int DLL_EXPORT taRWI(taBars *b1, taArray *a2, taArray *a3, int term, int start);
int DLL_EXPORT taScaleArray(taArray *a1, taArray *a2, KFloat amin, KFloat amax, KFloat mini, KFloat maxi, int nonzero, int start);
int DLL_EXPORT taSimpleMA(taArray *a1, taArray *a2, int term, int start);
KFloat DLL_EXPORT taSlopeArrayPts(taArray *a1, int x1, int x2);
KFloat DLL_EXPORT taSlopePts(KFloat x1, KFloat y1, KFloat x2, KFloat y2);
int DLL_EXPORT taSlowD(taBars *b1, taArray *a2, int term1, int term2, int term3, int start);
KFloat DLL_EXPORT taStdDev(taArray *a1, int samp, int start, int stop);
int DLL_EXPORT taStdDevMv(taArray *a1, taArray *a2, int samp, int term, int start);
int DLL_EXPORT taStochastic(taArray *a1, taArray *a2, int term, int start);
KFloat DLL_EXPORT taSumPrev(taArray *a1, int term, int start);
int DLL_EXPORT taTR(taBars *b1, taArray *a2, int start);
int DLL_EXPORT taTrendScore(taArray *a1, taArray *a2, int term, int start);
KFloat DLL_EXPORT taTrueHigh(taBars *b1, int pos);
KFloat DLL_EXPORT taTrueLow(taBars *b1, int pos);
KFloat DLL_EXPORT taTrueRange(taBars *b1, int pos);
int DLL_EXPORT taTSI(taArray *a1, taArray *a2, int term1, int term2, int start);
int DLL_EXPORT taTXT2Array(char *infile, taArray *a1, char *delimit, int skip, int startcol, int stopcol);
int DLL_EXPORT taUpAverage(taArray *a1, taArray *a2, int term, int start);
int DLL_EXPORT taUltimateOsc(taBars *b1, taArray *a2, int term1, int term2, int term3, int start);
int DLL_EXPORT taVHF(taArray *a1, taArray *a2, int term, int start);
int DLL_EXPORT taWeightedMA(taArray *a1, taArray *a2, int term, int start);
int DLL_EXPORT taWellesMA(taArray *a1, taArray *a2, int term, int start);
int DLL_EXPORT taWellesSum(taArray *a1, taArray *a2, int term, int start);
int DLL_EXPORT taWellesVolatility(taBars *b1, taArray *a2, taArray *a3, taArray *a4, taArray *a5, KFloat constant, int term, int start);
int DLL_EXPORT taWilliamsAD(taBars *b1, taArray *a2, int start);
int DLL_EXPORT taWilliamsR(taBars *b1, taArray *a2, int term, int start);
int DLL_EXPORT taFormationFill(taBars *b1, taArray *a2, char *formfunc, int start, int stop);
int DLL_EXPORT taCPR(taBars *b1, int pos);
int DLL_EXPORT taGap(taBars *b1, int pos);
int DLL_EXPORT taIslandReversal(taBars *b1, int pos);
int DLL_EXPORT taInsideBar(taBars *b1, int pos);
int DLL_EXPORT taKeyReversal(taBars *b1, int pos);
int DLL_EXPORT taOutsideBar(taBars *b1, int pos);
int DLL_EXPORT taCrossOver(taArray *a1, taArray *a2, int pos);
int DLL_EXPORT taCrossOverFill(taArray *a1, taArray *a2, taArray *a3, int start, int stop);
int DLL_EXPORT taTurn(taArray *a1, int pos);
int DLL_EXPORT taTurnFill(taArray *a1, taArray *a2, int start, int stop);
/* -------------------------------------------------------------------- */
/* tacandle                                                             */
/* -------------------------------------------------------------------- */


#define taNONE      0
#define taBLACKCNDL 1
#define taWHITECNDL 2
#define taDOJI      0
#define taSMALL     1
#define taMEDIUM    2
#define taLARGE     3

#pragma pack(1)
struct taCandles
{
 unsigned int color:2;      /*taNONE taBLACKCNDL taWHITECNDL */
 unsigned int shape:2;      /*taLARGE taSMALL  taDOJI  */
 unsigned int topshadow:2;  /*taLARGE taMEDIUM taSMALL */
 unsigned int botshadow:2;  /*taLARGE taMEDIUM taSMALL */
};
#pragma pack()

struct taCandleInfo
{
 struct taCandles *candle;
 KFloat  largeshadow;         /* Length pct to determine if shadow is taLARGE */
 KFloat  smallshadow;         /* Length pct to determine if shadow is taSMALL */
 KFloat  equalpct;            /* Tolerance level to determine equality */
};


int DLL_EXPORT taCandleConstruct(taBars *b1, struct taCandleInfo *info);
void  DLL_EXPORT taCandleFree(struct taCandleInfo *info);
int DLL_EXPORT taCandleFill(taBars *b1, struct taCandleInfo *info, taArray *a2,
                          char *candlefunct, int start, int stop);
int DLL_EXPORT taApproxEqual(KFloat f1, KFloat f2, KFloat pct);
KFloat DLL_EXPORT taBodyMidpoint(taBars *b1, int pos);
int DLL_EXPORT taBodyGapUp(taBars *b1, int pos);
int DLL_EXPORT taBodyGapDn(taBars *b1, int pos);
int DLL_EXPORT taWesternGapUp(taBars *b1, int pos);
int DLL_EXPORT taWesternGapDn(taBars *b1, int pos);
int DLL_EXPORT taAbandonedBabyBottom(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taAbandonedBabyTop(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taAdvanceBlock(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taBeltHoldLineBear(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taBeltHoldLineBull(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taCounterAttackBear(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taCounterAttackBull(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taDarkCloudCover(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taEngulfingLineBear(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taEngulfingLineBull(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taEveningDojiStar(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taEveningStar(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taFalling3Method(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taGravestoneDoji(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taHammer(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taHangingman(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taHaramiBlack(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taHaramiCrossBlack(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taHaramiCrossWhite(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taHaramiWhite(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taHighWave(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taInNeckLine(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taInvertedHammer(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taLongLeggedDoji(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taMatHoldPattern(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taMorningDojiStar(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taMorningStar(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taPiercingLine(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taRising3Method(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taSeperatingLineBear(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taSeperatingLineBull(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taShootingStar(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taSideBySideWhiteGapDn(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taSideBySideWhiteGapUp(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taStalledPattern(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taTasukiDownsideGap(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taTasukiUpsideGap(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taThreeGapsUp(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taThreeGapsDown(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taThreeWhiteSoldiers(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taThrustingLine(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taUpsideGap2Crows(taBars *b1, struct taCandleInfo *info, int pos);
int DLL_EXPORT taCandleColor(struct taCandleInfo *info, int pos);
int DLL_EXPORT taCandleShape(struct taCandleInfo *info, int pos);
int DLL_EXPORT taCandleTopShadow(struct taCandleInfo *info, int pos);
int DLL_EXPORT taCandleBotShadow(struct taCandleInfo *info, int pos);
/* -------------------------------------------------------------------- */
/* tafin                                                                */
/* -------------------------------------------------------------------- */

#define taANNUAL_PAYMENTS       1
#define taSEMIANNUAL_PAYMENTS   2
#define taQUARTERLY_PAYMENTS    4
#define taMONTHLY_PAYMENTS     12
#define taDAILY_PAYMENTS      360

struct taOption
{
 KFloat price;
 KFloat strike;
 int    type;
 unsigned int expiredate;
};

struct taOptionSpreads
{
 KFloat stockprice;
 struct taOption buy[2];
 struct taOption sell[2];
 KFloat maxprofit;
 KFloat riskreward;
 KFloat riskamt;
 KFloat breakevenup;
 KFloat breakevendn;
 KFloat pctbreakevenup;
 KFloat pctbreakevendn;
};

struct taTimeValue
{
 KFloat principal;
 KFloat rate;
 KFloat periods;
 int    compound;
 //char   filler[2];
};

struct taTimeValueA
{
 KFloat principal;
 KFloat discrate;
 KFloat numpayments;
};

struct taBondInfo
{
 KFloat facevalue;
 KFloat price;
 KFloat coupon;
 int    pymtsperyr;
 unsigned int maturitydate;
};

KFloat DLL_EXPORT taApprxCumProbDist(KFloat d1);
int  DLL_EXPORT taBearCall(struct taOptionSpreads *ops); /* Buy Hi strike Sell Lo strike */
int  DLL_EXPORT taBearPut(struct taOptionSpreads *ops); /* Buy Hi strike Sell Lo strike */
KFloat DLL_EXPORT taBlackScholes(struct taOption *opt, KFloat stockprice, KFloat intrate, KFloat variance, unsigned int asofdate);
KFloat DLL_EXPORT taBondAccrInt(KFloat coupon, unsigned int lastpaydate, unsigned int asofdate, KFloat daysinyr);
KFloat DLL_EXPORT taBondDuration(struct taBondInfo *bond, KFloat discrate, unsigned int asofdate);
KFloat DLL_EXPORT taBondDurationPriceChg(KFloat price, KFloat olddiscrate, KFloat duration, KFloat discrate);
KFloat DLL_EXPORT taBondPricing (struct taBondInfo *bond, KFloat discrate, unsigned int asofdate);
int  DLL_EXPORT taBullCall(struct taOptionSpreads *ops); /* Buy Lo strike Sell Hi strike */
int  DLL_EXPORT taBullPut(struct taOptionSpreads *ops); /* Buy Lo strike Sell Hi strike */
KFloat DLL_EXPORT taFutureValue (struct taTimeValue *tvalue);
KFloat DLL_EXPORT taFutureValuePrinc (struct taTimeValue *tvalue, KFloat futurevalue);
KFloat DLL_EXPORT taFutureValueInt (struct taTimeValue *tvalue, KFloat futurevalue);
KFloat DLL_EXPORT taFutureValueAnnuity(struct taTimeValue *tvalue);
int  DLL_EXPORT taLongStraddle(struct taOptionSpreads *ops);
int  DLL_EXPORT taLongStrangle(struct taOptionSpreads *ops);
KFloat DLL_EXPORT taPresentValue(struct taTimeValue *tvalue);
KFloat DLL_EXPORT taPresentValueAnnuity(struct taTimeValueA *tvaluea);
int  DLL_EXPORT taOptionInit(struct taOptionSpreads *ops);
int  DLL_EXPORT taShortStraddle(struct taOptionSpreads *ops);
int  DLL_EXPORT taShortStrangle(struct taOptionSpreads *ops);
KFloat DLL_EXPORT taTBillDiscYield (struct taBondInfo *bond, unsigned int asofdate);
KFloat DLL_EXPORT taTBillBondEqYield(struct taBondInfo *bond, unsigned int asofdate);
KFloat DLL_EXPORT taYieldtoMaturity(struct taBondInfo *bond, unsigned int asofdate);
/* -------------------------------------------------------------------- */
/* tatrade                                                              */
/* -------------------------------------------------------------------- */

#define taMARKET 1        /* at ind */
#define taSTOP   2        /* at ind */
#define taLIMIT  3        /* at ind */
#define taDAY    1        /* time in force */
#define taGTC    2        /* time in force */
#define taCLOSE  1
#define taOPEN   2
#define taBUY    1
#define taSELL   2
#define taLONG   1
#define taSHORT  2

/* Trade Graph Constants */
#define taBO     1         /* buy to open   */
#define taSO     2         /* sell to open  */
#define taBC     4         /* buy to close  */
#define taSC     8         /* sell to close */

struct taOrderParms
{
 char   systemname[80];
 KFloat ticksize;        /* Tick increment for this issue */
 KFloat valueoftick;
 KFloat slippage;
 KFloat commission;
 KFloat margin;
 long   ordertimedelay;  /* number of seconds before At Market order can occur */
 int    reversal;        /* allow auto exit and re-entry */
 int    bestprice;       /* enter at open if better price */
 char   ttreport[80];    /* Name of Trade By Trade Report file or "\0" */
};

struct taCurrent
{
 int    barnum;
 int    position;        /* Long Short Nothing                  */
 long   longvol;         /* Number of Shares or Contracts Long  */
 long   shortvol;        /* Number of Shares or Contracts Short */
 long   totalvolume;     /* Contract                            */
 KFloat avgentryprice;   /* Weigthed Avg of all open executions */
 unsigned int entrydate; /* Of Latest Trade */
 unsigned int entrytime; /* Of Latest Trade */
 int    longduration;
 int    shortduration;
 int    duration;        /* Number of bars in current position  */
 //char   filler[2];
};

struct taOrder
{
 char   name[20];     /* Order Description     */
 char   fromname[20]; /* Used by Close Orders  */
 long   volume;       /* Volume                */
 int    type;         /* (O)pen (C)lose Cancel(X)           */
 int    buysell;      /* (B)uy (S)ell                       */
 KFloat reqprice;     /* Requested Price                    */
 int    atind;        /* At (M)arket, (S)top or (L)imit     */
                        /* (S)top = Next Tick higher for Buy  */
                        /* (S)top = Next Tick lower  for Sell */
                        /* (L)imit = or Better       for Buy  */
                        /* (L)imit = or Better       for Sell */
 int    timeinforce;  /* Day / GTC             */
 unsigned int date;   /* Order Date            */
 unsigned int time;   /* Order Time            */
 int    deletesw;     /* For Close Orders only */
 //char   filler[2];
};

struct taExecution
{
 char   name[20];     /* Order Description */
 int    barnum;       /* Bar Number        */
 int    buysell;      /* Buy / Sell        */
 unsigned int date;   /* Order Date        */
 unsigned int time;   /* Order Time        */
 KFloat price;        /* Actual Price      */
 long   volume;       /* Volume            */
 long   remvolume;    /* Remaining Volume  */
};

struct taPerformance
{
 KFloat netprofit;       /* NP */
 KFloat grossprofit;     /* GP */
 KFloat grossloss;       /* GL */
 long   trades;          /* NT */
 KFloat pctprofitable;   /* PP */
 int    win;             /* NW */
 int    loss;            /* NL */
 KFloat largestwin;      /* LW */
 KFloat largestloss;     /* LL */
 KFloat avgwin;          /* AW */
 KFloat avgloss;         /* AL */
 KFloat avgwintoavgloss; /* WL */
 KFloat avgtrade;        /* AT */
 int    maxconwin;       /* MW */
 int    maxconloss;      /* ML */
 KFloat avgwinstreak;    /* WS */
 KFloat avglosstreak;    /* LS */
 KFloat maxdrawdown;     /* MD */
 KFloat profitfactor;    /* PF */
 KFloat maxcontractsheld;/* MC */
 KFloat acctsizereqd;    /* AS */
 KFloat rtnonacct;       /* RA */
 KFloat prr;             /* PR */
 KFloat openpospl;
 int    winbars;
 int    lossbars;
 int    winstreak;
 int    lossstreak;
};

struct taOrderSystem
{
 struct taOrderParms  OrderParms;
 struct taCurrent     Current;
 struct taPerformance Long;
 struct taPerformance Short;
 struct taPerformance Net;
 struct taOrder      *Order;
 struct taExecution  *InExec;
 taBars bars;
 taArray tradegraph;
 int    OrderSize;
 int    OrderCnt;
 int    InExecSize;
 int    InExecCnt;
};

struct taOptVar
{
 KFloat from;
 KFloat to;
 KFloat incr;
 long   count;
 KFloat value;
};

int DLL_EXPORT taOrderSystemInit(struct taOrderSystem *taOS, char *systemname, taBars *bars, int maxorders);
void  DLL_EXPORT taOrderSystemReset(struct taOrderSystem *taOS);
void  DLL_EXPORT taOrderSystemClose(struct taOrderSystem *taOS);
int DLL_EXPORT taProcessOrders(struct taOrderSystem *taOS);
int DLL_EXPORT taBuyToOpen(struct taOrderSystem *taOS, char *name, long volume, KFloat price, int atind, int timeinforce);
int DLL_EXPORT taSellToOpen(struct taOrderSystem *taOS, char *name, long volume, KFloat price, int atind, int timeinforce);
int DLL_EXPORT taSellToClose(struct taOrderSystem *taOS, char *name, char *fromname, long volume, KFloat price, int atind, int timeinforce);
int DLL_EXPORT taBuyToClose(struct taOrderSystem *taOS, char *name, char *fromname, long volume, KFloat price, int atind, int timeinforce);
int DLL_EXPORT taCancelOrder(struct taOrderSystem *taOS, char *name);

int DLL_EXPORT taPrintOrders(struct taOrderSystem *taOS, char *outfile, int append);
int DLL_EXPORT taProfitRpt(struct taOrderSystem *taOS, char *outfile, int append);
int DLL_EXPORT taProfitRpt1(struct taOrderSystem *taOS, char *outfile, int append, char *heading, char *fields);

long  DLL_EXPORT taOptVarCount(struct taOptVar *vars, int numvars);
int DLL_EXPORT taOptVarValue(struct taOptVar *vars, int numvars, long iteration);

/* -------------------------------------------------------------------- */
/* tafile                                                               */
/* -------------------------------------------------------------------- */

int DLL_EXPORT taFILEtype(char *type);
long  DLL_EXPORT taFILEfindrec(char *type, char *path, char *name, unsigned int date, unsigned int time,
		         unsigned int *actualdate, unsigned int *actualtime);
int DLL_EXPORT taFILElist(char *type, char *path, char *outfile, int append);
int DLL_EXPORT taFILEprintinfo(taBars *b1, char *outfile, int append);
int DLL_EXPORT taFILEread(char *type, taBars *b1, char *path, char *name, long start, int cnt, int allocate);

/* -------------------------------------------------------------------- */
/* tacsi                                                                */
/* -------------------------------------------------------------------- */

long DLL_EXPORT taCSIfindrec(char *path, char *name, unsigned int date, unsigned int *actualdate);
int DLL_EXPORT taCSIlist(char *path, char *outfile, int append);
int DLL_EXPORT taCSIread(taBars *b1, char *path, char *name, long start, int cnt, int allocate);
/* -------------------------------------------------------------------- */
/* tact                                                                 */
/* -------------------------------------------------------------------- */

long  DLL_EXPORT taCTfindrec(char *path, char *name, unsigned int date, unsigned int time,
              unsigned int *actualdate, unsigned int *actualtime);
int DLL_EXPORT taCTlist(char *path, char *outfile, int append);
int DLL_EXPORT taCTread(taBars *b1, char *path, char *name, long start, int cnt, int allocate);
/* -------------------------------------------------------------------- */
/* tatxt                                                                */
/* -------------------------------------------------------------------- */

struct taTXTinfo
{
 char   delimit[16];    /* Character(s) that delimit the columns, space = _ */
 int    skip;           /* Number of lines to skip at begining of file      */
 int    dt, ti;         /* Column numbers (or zero if field is not in file) */
 int    op, hi, lo, cl; /* Column numbers (or zero if field is not in file) */
 int    vol, oi;        /* Column numbers (or zero if field is not in file) */
 int    dtformat;       /* Format of date */
 int    tiformat;       /* Format of time */
 int    filler;
};

long DLL_EXPORT taTXTfindrec(char *path, char *name, struct taTXTinfo *info,
		        unsigned int date, unsigned int time,
		        unsigned int *actualdate, unsigned int *actualtime);
int DLL_EXPORT taTXTread(taBars *b1, char *path, char *name, struct taTXTinfo *info,
		        long start, int cnt, int allocate);
int DLL_EXPORT taTXTreadinfo(char *path, char *infile, struct taTXTinfo *info);
int DLL_EXPORT taTXTwriteinfo(char *path, char *outfile, struct taTXTinfo *info);
/* -------------------------------------------------------------------- */
/* tadj                                                                 */
/* -------------------------------------------------------------------- */

long  DLL_EXPORT taDJfindrec(char *path, char *name, unsigned int date, unsigned int *actualdate);
int DLL_EXPORT taDJlist(char *path, char *outfile, int append);
int DLL_EXPORT taDJread(taBars *b1, char *path, char *name, long start, int cnt, int allocate);
/* -------------------------------------------------------------------- */
/* tafs                                                                 */
/* -------------------------------------------------------------------- */

long  DLL_EXPORT taFSfindrec(char *path, char *name, unsigned int date, unsigned int time, unsigned int *actualdate, unsigned int *actualtime);
int DLL_EXPORT taFSread(taBars *b1, char *path, char *name, long start, int cnt, int allocate);
/* -------------------------------------------------------------------- */
/* tatc                                                                 */
/* -------------------------------------------------------------------- */

long  DLL_EXPORT taTCfindrec(char *path, char *name, unsigned int date, unsigned int *actualdate);
int DLL_EXPORT taTClist(char *path, char *outfile, int append);
int DLL_EXPORT taTCread(taBars * b1, char *path, char *name, long start, int cnt, int allocate);
/* -------------------------------------------------------------------- */
/* tatt                                                                 */
/* -------------------------------------------------------------------- */

long  DLL_EXPORT taTTfindrec(char *path, char *name, unsigned int date, unsigned int *actualdate);
int DLL_EXPORT taTTread(taBars *b1, char *path, char *name, long start, int cnt, int allocate);
/* -------------------------------------------------------------------- */
/* tachart                                                              */
/* -------------------------------------------------------------------- */

#define taCHART_HILOCL    1
#define taCHART_OPHILOCL  2
#define taCHART_CANDLE    3
#define taCHART_HIST      4
#define taCHART_LINE      5
#define taCHART_POINT     6
#define taCHART_FILL      7
#define taCHART_TRADES    8

#define taCHART_DEFINED   12345
#define taCHART_SUBCNTMAX 8 /* MUST be multiple of 4 to preserve 32bit structure alignment */

struct taChartLine
{
 taArray    a1;
 taBars     b1;
 int    chartnum;
 int    type;
 KFloat ystart;
 KFloat ystop;
 long   color1;
 long   color2;
 long   style;
};

struct taChartGrids
{
 int    chartnum;
 int    filler;
 KFloat value;
 KFloat ystart;
 KFloat ystop;
 long   color;
 long   style;
};

 struct taChartScreen
{
 int    defined;           /* == taCHART_DEFINED */
 int    scalepos;          /* 0=off 1=left 2=right */
 int    datestyle;         /* 0=off 1=auto 2=J 3=Jan 4=January 5=31Jan */
 int    vgrid;             /* linestyle, taNULLSTYLE=off */
 int    hgrid;             /* linestyle, taNULLSTYLE=off */
 int    xwidth;
 int    xstart;
 int    xstop;
 int    linecnt;
 int    gridcnt;
 int    linemax;
 int    gridmax;
 int    x1;
 int    y1;
 int    x2;
 int    y2;
 char   heading[160];
 char   footing[160];
 long   fgdcolor;
 long   bgdcolor;
 long   numcolor;
 long   linecolor;
 long   gridcolor;
 struct taChartLine *chartline;
 struct taChartGrids *chartgrid;
 int    subchartsize[taCHART_SUBCNTMAX];
};

struct taChartInfo
{
 int    xmax;
 int    xscale;
 int    top;
 int    bottom;
 int    left;
 int    right;
 int    scaleleft;
 int    scaleright;
 int    dateleft;
 int    subtop[taCHART_SUBCNTMAX + 1];
 KFloat subystart[taCHART_SUBCNTMAX];
 KFloat subystop[taCHART_SUBCNTMAX];
 int    fontwidth;
 int    fontheight;
 taBars   masterbars;
};

#ifdef _WINDOWS

#define taSOLID          0
#define taDASH           1
#define taDOT            2
#define taDASHDOT        3
#define taDASHDOTDOT     4
#define taNULLSTYLE      5
#define taBLACK          0x0
#define taBLUE           0x800000
#define taGREEN          0x008000
#define taCYAN           0x808000
#define taRED            0x000080
#define taMAGENTA        0x800080
#define taBROWN          0x008080
#define taWHITE          0xC0C0C0
#define taGRAY           0xC0C0C0
#define taGREY           0x808080
#define taBRIGHTBLUE     0xFF0000
#define taBRIGHTGREEN    0x00FF00
#define taBRIGHTCYAN     0xFFFF00
#define taBRIGHTRED      0x0000FF
#define taBRIGHTMAGENTA  0xFF00FF
#define taBRIGHTBROWN    0x00FFFF
#define taYELLOW         0x00FFFF
#define taBRIGHTWHITE    0xFFFFFF

int DLL_EXPORT taChart(HDC hdc, taArray *pa1, taBars *pb1, int xstart, int xstop, int xwidth,
       		   KFloat ystart, KFloat ystop, int x1, int y1, int x2, int y2,
		         int type, long color1, long color2, long style);
int DLL_EXPORT taChartGrid(HDC hdc, KFloat level, KFloat ystart, KFloat ystop,
       		   int x1, int y1, int x2, int y2, long color, long style);
int DLL_EXPORT taChartSetup(HDC hdc, struct taChartScreen *screen, struct taChartInfo *info);
int DLL_EXPORT taChartPaint(HDC hdc, struct taChartScreen *screen, struct taChartInfo *info);
#else /* _WINDOWS not defined */

#define taSOLID          0xFFFF
#define taDASH           0xF0F0
#define taDOT            0x5555
#define taDASHDOT        0xF6F6
#define taDASHDOTDOT     0xDADA
#define taNULLSTYLE       0
#define taBLACK           0
#define taBLUE            1
#define taGREEN           2
#define taCYAN            3
#define taRED             4
#define taMAGENTA         5
#define taBROWN           6
#define taWHITE           7
#define taGRAY            8
#define taGREY            8
#define taBRIGHTBLUE      9
#define taBRIGHTGREEN    10
#define taBRIGHTCYAN     11
#define taBRIGHTRED      12
#define taBRIGHTMAGENTA  13
#define taBRIGHTBROWN    14
#define taYELLOW         14
#define taBRIGHTWHITE    15

extern char *taChartPath;

int DLL_EXPORT taChart(taArray *pa1, taBars *pb1, int xstart, int xstop, int xwidth,
        KFloat ystart, KFloat ystop, int x1, int y1, int x2, int y2,
        int type, long color1, long color2, long style);
int DLL_EXPORT taChartGrid(KFloat level, KFloat ystart, KFloat ystop,
        int x1, int y1, int x2, int y2, long color, long style);
int DLL_EXPORT taChartSetup(struct taChartScreen *screen, struct taChartInfo *info);
int DLL_EXPORT taChartPaint(struct taChartScreen *screen, struct taChartInfo *info);
#endif
int DLL_EXPORT taChartScreenInit(struct taChartScreen *screen, int linemax, int gridmax);
void  DLL_EXPORT taChartScreenClose(struct taChartScreen *screen);
int DLL_EXPORT taChartLineDef(struct taChartScreen *screen, int chartnum,
        taArray *a1, taBars *b1, KFloat ystart, KFloat ystop,
        int type, long color1, long color2, long style);
int DLL_EXPORT taChartGridDef(struct taChartScreen *screen, int chartnum, KFloat value,
        KFloat ystart, KFloat ystop, long color, long style);
int DLL_EXPORT taChartDraw(struct taChartScreen *screen);
int DLL_EXPORT taChartPosition(struct taChartScreen *screen, struct taChartInfo *info,
		      int x1, int y1, int *barnum, int *barpos, int *subchart, KFloat *yvalue);


/* taChart parameter quick reference                 */
/*                                                   */
/* type      array   bars    color1  color2  style   */
/* ----      -----   ----    ------  ------  -----   */
/* HILOCL    n/a     req'd   lines   n/a     n/a     */
/* OPHILOCL  n/a     req'd   lines   n/a     n/a     */
/* CANDLE    n/a     req'd   outline upcolor dncolor */
/* LINE      req'd   n/a     line    width   style   */
/* HIST      req'd   n/a     fgd     bgd     pattern */
/* POINT     req'd   n/a     point   n/a     n/a     */
/* FILL      req'd   n/a     fgd     bgd     pattern */
/* TRADES    n/a     req'd   buy     sell    close   */


/* -------------------------------------------------------------------- */
#ifdef __cplusplus
 }
#endif
#endif
