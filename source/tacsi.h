/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997 KTA */

#ifndef __TACSI_H__
#define __TACSI_H__

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

struct taCSImrec                /* record of qmaster file           */
{
 char     id[4];                /* CSI ID number                    */
 char     name[20];             /* Item name                        */
 char     type;                 /* File type (D,W,M,S)              */
 char     dm[2];                /* Delivery month                   */
 char     dy[2];                /* Delivery year                    */
 char     cf[2];                /* Conversion Factor                */
 char     pu[5];                /* Pricing Unit                     */
 char     csym[2];              /* Commodity Symbol if a commodity  */
 char     csflag;               /* C if commodity, S if stock       */
 char     oflag;                /* N=normal, P=put, C=call          */
 char     sprice[5];            /* Striking price if an option      */
 char     ssym[6];              /* Stock symbol if a stock          */
 char     dflag;                /* 0=record active, 1=record avail  */
 char     dc[2];                /* Delivery Century                 */
 char     reserved[7];          /* reserved for future expansion    */
 char     dispfact[2];          /* Display Conversion Factor        */
 char     idext[1];             /* Stock ID Number Extension Byte   */
};

struct taCSIhdr                 /* header of csi data file          */
{
 float    ferp;                 /* file end record pointer          */
 float    mdp;                  /* max date pointer (last updated)  */
 float    hh;                   /* highest high                     */
 float    ll;                   /* lowest low                       */
 float    fpdof;                /* first physical date on file      */
 float    lpdof;                /* last physical date on file       */
 char     hna[1];               /* high numbers allowed             */
 char     reserved4[4];         /* reserved for future expansion    */
 char     reserved1[1];         /* reserved for future expansion    */
 char     reserved2[2];         /* reserved for future expansion    */
};

struct taCSIrec                 /* record of csi data file          */
{
 float    date;                 /* YYMMDD format                    */
 char     dow;                  /* day of week (1-5 or 9)           */
 unsigned short open;
 unsigned short high;
 unsigned short low;
 unsigned short close;
 unsigned short noon;
 unsigned short cash;
 unsigned short tvol;           /* total volume                     */
 unsigned char  tvol1;          /* tvol multiplier * 65535          */
 unsigned short toi;            /* total open interest              */
 unsigned char  toi1;           /* toi multiplier * 65535           */
 unsigned short cvol;           /* contract level volume            */
 unsigned char  cvol1;          /* cvol multiplier * 65535          */
 unsigned short coi;            /* contract level open interest     */
 unsigned char  coi1;           /* coi multiplier * 65535           */
 unsigned short excl:2;         /* extension bits for close         */
 unsigned short exlo:2;         /* extension bits for low           */
 unsigned short exhi:2;         /* extension bits for high          */
 unsigned short exop:2;         /* extension bits for open          */
 unsigned short excash2:2;      /* secondary ext bits for cash      */
 unsigned short exnoon2:2;      /* secondary ext bits for noon      */
 unsigned short excash:2;       /* extension bits for cash          */
 unsigned short exnoon:2;       /* extension bits for noon          */
 unsigned short excl2:2;        /* secondary ext bits for close     */
 unsigned short exop2:2;        /* secondary ext bits for open      */
 unsigned short exhi2:2;        /* secondary ext bits for high      */
 unsigned short exlo2:2;        /* secondary ext bits for low       */
};

#pragma pack()

#define sizeof_taCSIrec 32


#ifdef __cplusplus
 }
#endif
#endif
