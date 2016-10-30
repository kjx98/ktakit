/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997 KTA */

#ifndef __KTAMT4_H__
#define __KTAMT4_H__

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(4)

struct MqlRates
{
UInt64  time;       // Period start time
double  open;       // Open price
double  high;       // The highest price of the period
double  low;        // The lowest price of the period
double  close;      // Close price
Int64   tick_volume;// Tick volume
Int32   spread;     // Spread
Int64   real_volume;// Trade volume
};

struct MqlFXTRates
{
UInt64  time;       // Period start time
double  open;       // Open price
double  high;       // The highest price of the period
double  low;        // The lowest price of the period
double  close;      // Close price
Int32   tick_volume;// Tick volume
Int32   spread;     // Spread
UInt32  last_time;  // Incoming tick last time
Int32   fill0;      // 3
};


struct MqlHST_header
{
Int32   file_version;
char    c_copyright[64];
char    c_symbol[12];
Int32   i_period;
Int32   i_digits;
Int32   i_datetime;     // 0
Int32   fill2;          // 0
Int32   fill_unused[13];
};

struct MqlFXT_header
{
Int32   file_version;       //    0 = 0x0000 + 4
char    c_copyright[64];    // "(C)opyright 2005-2007, MetaQuotes Software Corp."; // 64 bytes   //    4 = 0x0004 + 64
char    c_server[128];      // 128 bytes                        //   68 = 0x0044 + 128
char    c_symbol[12];       // 12 bytes                         //  196 = 0x00C4 + 12
Int32   i_period;                                               //  208 = 0x00D0 + 4
Int32   i_model;            // every tick model                 //  212 = 0x00D4 + 4
Int32   i_bars;             // bars processed                   //  216 = 0x00D8 + 4
UInt32  t_fromdate;         // begin modelling date             //  220 = 0x00DC + 4
UInt32  t_todate;           // end modelling date               //  224 = 0x00E0 + 4
//++++ add 4 bytes to align the next double                     +++++++
Int32   fill0;
double  d_modelquality;                                         //  232 = 0x00E4 + 8
//---- common parameters                                        -------
char    s_currency[12];     // base currency (12 bytes)         //  240 = 0x00F0 + 12
Int32   i_spread;                                               //  252 = 0x00FC + 4
Int32   i_digits;                                               //  256 = 0x0100 + 4
//++++ add 4 bytes to align the next double                     +++++++
Int32   fill1;
double  d_point;                                                //  264 = 0x0108 + 8
Int32   i_lot_min;          // minimal lot size                 //  272 = 0x0110 + 4
Int32   i_lot_max;          // maximal lot size                 //  276 = 0x0114 + 4
Int32   i_lot_step;                                             //  280 = 0x0118 + 4
Int32   i_stops_level;      // stops level value                //  284 = 0x011C + 4
bool    b_gtc_pendings;     // good till cancel                 //  288 = 0x0120 + 4
//---- profit calculation parameters                            -------
//++++ add 4 bytes to align the next double                     +++++++
Int32   fill2;
double  d_contract_size;                                        //  296 = 0x0128 + 8
double  d_tick_value;                                           //  304 = 0x0130 + 8
double  d_tick_size;                                            //  312 = 0x0138 + 8
Int32   i_profit_mode;      // profit calculation mode          //  320 = 0x0140 + 4
//---- swaps calculation                                        -------
bool    b_swap_enable;                                          //  324 = 0x0144 + 4
Int32   i_swap_type;        // type of swap                     //  328 = 0x0148 + 4
//++++ add 4 bytes to align the next double                     +++++++
Int32   fill3;
double  d_swap_long;                                            //  336 = 0x0150 + 8
double  d_swap_short;       // overnight swaps values           //  344 = 0x0158 + 8
Int32   i_swap_rollover3days;// number of day of triple swaps   //  352 = 0x0160 + 4
//---- margin calculation                                       -------
Int32   i_leverage;                                             //  356 = 0x0164 + 4
Int32   i_free_margin_mode; // free margin calculation mode     //  360 = 0x0168 + 4
Int32   i_margin_mode;      // margin calculation mode          //  364 = 0x016C + 4
Int32   i_margin_stopout;   // margin stopout level             //  368 = 0x0170 + 4
Int32   i_margin_stopout_mode;// margin stopout check mode      //  372 = 0x0174 + 4
double  d_margin_initial;   // margin requirements              //  376 = 0x0178 + 8
double  d_margin_maintenance;                                   //  384 = 0x0180 + 8
double  d_margin_hedged;                                        //  392 = 0x0188 + 8
double  d_margin_divider;                                       //  400 = 0x0190 + 8
char    s_margin_currency[12];  // 12 bytes                     //  408 = 0x0198 + 12
//---- commissions calculation                                  -------
//++++ add 4 bytes to align the next double                     +++++++
Int32   fill4;
double  d_comm_base;        // basic commission                 //  424 = 0x01A8 + 8
Int32   i_comm_type;        // basic commission type            //  432 = 0x01B0 + 4
Int32   i_comm_lots;        // commission per lot or per deal   //  436 = 0x01B4 + 4
//---- for internal use                                         -------
Int32   i_from_bar;         // 'fromdate' bar number, to skip   //  440 = 0x01B8 + 4 /8
Int32   i_to_bar;           // 'todate' bar number              //  444 = 0x01BC + 4
Int32   i_start_period[6];                                      //  448 = 0x01C0 + 24 /16
UInt32  i_from;             // must be zero                     //  472 = 0x01D8 + 4
UInt32  i_to;               // must be zero                     //  476 = 0x01DC + 4
Int32   fill5;
Int32   i_freeze_level;     // order's freeze level in points   //  480 = 0x01E0 + 4
Int32   i_reserved[60];     // unused                           //  484 = 0x01E4 + 244 = 600
};

#pragma pack()


#ifdef __cplusplus
 }
#endif
#endif
