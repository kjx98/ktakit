/* Technical Analysis Programmer's Toolkit Copyright (C) 1997 KTA */

/* Financial Functions Sample Program */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ktakit.h"

int      taOptionPrint(struct taOptionSpreads *ops, char *title);


int      main(int argc, char *argv[])
{
 double   bs = 0, fv = 0, fva = 0, pv = 0, pva = 0;
 float    price = 0, strike = 0, intrate = 0, months = 0, variance = 0;
 float    stockprice = 0, ytm = 0, discrate = 0, yield = 0, bd = 0, bdp = 0;
 float    olddiscrate = 0, newdiscrate = 0, accrint = 0;
 int      i = 0;
 int      i1 = 0;
 int      asofdate;
 struct taOptionSpreads ops;
 struct taOption opt;
 struct taTimeValue tvalue;
 struct taTimeValueA tvaluea;
 struct taBondInfo bond;

/* BlackScholes */
 stockprice = 40;
 opt.price = 0;
 opt.strike = 35;
 opt.expiredate = taDateTxtToJulian("1992/03/01", DTFORMAT_CCYY_MM_DD);
 asofdate = taDateTxtToJulian("1991/12/01", DTFORMAT_CCYY_MM_DD);
 intrate = 8;
 variance = .2;
 opt.price = taBlackScholes(&opt, stockprice, intrate, variance, asofdate);
 printf("\nBlack Scholes\n");
 printf("Stock Price : %8.4f\n", stockprice);
 printf("Buy  Strike : %8.4f\n", opt.strike);
 printf("Option Price: %8.4f\n", opt.price);
 printf("Int  Rate   : %8.4f\n", intrate);
 printf("Variance    : %8.4f\n", variance);
 printf("As Of Date  : %s\n", "1991/12/01");
 printf("Expiredate  : %s\n", "1992/03/01");

/* Test Data for  Bull Call */
/* Buy  a call option with strike of 20 and price of 11.875 */
/* Sell a call option with strike of 25 and price of  7.5   */
 i = taOptionInit(&ops);
 ops.stockprice = 31.5;
 ops.buy[0].strike = 20;
 ops.buy[0].price = 11.875;
 ops.sell[0].strike = 25;
 ops.sell[0].price = 7.5;
 i = taBullCall(&ops);
 i = taOptionPrint(&ops, "Bull Call");

/* Test Data for  Bear Call */
/* Buy  a call option with strike of 25  and price of 7.5    */
/* Sell a call option with strike of 20  and price of 11.875 */
 i = taOptionInit(&ops);
 ops.stockprice = 31.5;
 ops.buy[0].strike = 25;
 ops.buy[0].price = 7.5;
 ops.sell[0].strike = 20;
 ops.sell[0].price = 11.875;
 i = taBearCall(&ops);
 i = taOptionPrint(&ops, "Bear Call");

/* Test Data for  Bull Put */
/* Buy  a put  option with strike of 25  and price of  .3125 */
/* Sell a put  option with strike of 35  and price of 4.5    */
 i = taOptionInit(&ops);
 ops.stockprice = 31.5;
 ops.buy[0].strike = 25;
 ops.buy[0].price = .3125;
 ops.sell[0].strike = 35;
 ops.sell[0].price = 4.5;
 i = taBullPut(&ops);
 i = taOptionPrint(&ops, "Bull Put");

/* Test Data for  Bear Put */
/* Buy  a put  option with strike of 35  and price of 4.5    */
/* Sell a put  option with strike of 25  and price of  .3125 */
 i = taOptionInit(&ops);
 ops.stockprice = 31.5;
 ops.buy[0].strike = 35;
 ops.buy[0].price = 4.5;
 ops.sell[0].strike = 25;
 ops.sell[0].price = .3125;
 i = taBearPut(&ops);
 i = taOptionPrint(&ops, "Bear Put");

/* Test Data for  Long Straddle */
/* Buy  a call option with strike of 82     and price of  .25  */
/* Buy  a put  option with strike of 82     and price of 1.25  */
 i = taOptionInit(&ops);
 ops.stockprice = 81;
 ops.buy[0].strike = 82;	/* Buy a Call */
 ops.buy[0].type = taCALL;	/* Call       */
 ops.buy[0].price = .25;
 ops.buy[1].strike = 82;	/* Buy a Put  */
 ops.buy[1].price = 1.25;
 ops.buy[1].type = taPUT;	/* Put        */

 i = taLongStraddle(&ops);
 printf("\nLong Straddle\n");
 printf("Stock      : %8.4f\n", ops.stockprice);
 printf("Buy Strike : %8.4f\n", ops.buy[0].strike);
 printf("Buy Price  : %8.4f\n", ops.buy[0].price);
 printf("Buy Strike : %8.4f\n", ops.buy[1].strike);
 printf("Buy Price  : %8.4f\n", ops.buy[1].price);
 printf("Risk       : %8.4f\n", ops.riskamt);
 printf("Max Profit : %8.4f\n", ops.maxprofit);
 printf("RiskReward : %8.4f\n", ops.riskreward);
 printf("BreakEvenUp: %8.4f\n", ops.breakevenup);
 printf("BreakEvenDn: %8.4f\n", ops.breakevendn);
 printf("Break Up   : %8.4f\n", ops.pctbreakevenup);
 printf("Break Dn   : %8.4f\n", ops.pctbreakevendn);

/* Test Data for  Short Straddle */
/* Sell a call option with strike of .7     and price of  .01  */
/* Sell a put  option with strike of .7     and price of  .01  */
 i = taOptionInit(&ops);
 ops.stockprice = .70;
 ops.sell[0].strike = .7;	/* Sell a Call */
 ops.sell[0].price = .01;
 ops.sell[1].strike = .7;	/* Sell a Put  */
 ops.sell[1].price = .01;

 i = taShortStraddle(&ops);
 printf("\nShort Straddle\n");
 printf("Stock      : %8.4f\n", ops.stockprice);
 printf("Buy Strike : %8.4f\n", ops.sell[0].strike);
 printf("Buy Price  : %8.4f\n", ops.sell[0].price);
 printf("Buy Strike : %8.4f\n", ops.sell[1].strike);
 printf("Buy Price  : %8.4f\n", ops.sell[1].price);
 printf("Risk       : %8.4f\n", ops.riskamt);
 printf("Max Profit : %8.4f\n", ops.maxprofit);
 printf("RiskReward : %8.4f\n", ops.riskreward);
 printf("BreakEvenUp: %8.4f\n", ops.breakevenup);
 printf("BreakEvenDn: %8.4f\n", ops.breakevendn);
 printf("Break Up   : %8.4f\n", ops.pctbreakevenup);
 printf("Break Dn   : %8.4f\n", ops.pctbreakevendn);

/* Test Data for  Long Strangle */
/* Buy  a call option with strike of .53    and price of  .005  */
/* Buy  a put  option with strike of .51    and price of  .0048 */
 i = taOptionInit(&ops);
 ops.stockprice = .52;
 ops.buy[0].strike = .53;	/* Buy a Call */
 ops.buy[0].price = .005;
 ops.buy[0].type = taCALL;
 ops.buy[1].strike = .51;	/* Buy a Put  */
 ops.buy[1].price = .0048;
 ops.buy[1].type = taPUT;

 i = taLongStrangle(&ops);
 printf("\nLong Strangle\n");
 printf("Stock      : %8.4f\n", ops.stockprice);
 printf("Buy Strike : %8.4f\n", ops.buy[0].strike);
 printf("Buy Price  : %8.4f\n", ops.buy[0].price);
 printf("Buy Strike : %8.4f\n", ops.buy[1].strike);
 printf("Buy Price  : %8.4f\n", ops.buy[1].price);
 printf("Risk       : %8.4f\n", ops.riskamt);
 printf("Max Profit : %8.4f\n", ops.maxprofit);
 printf("RiskReward : %8.4f\n", ops.riskreward);
 printf("BreakEvenUp: %8.4f\n", ops.breakevenup);
 printf("BreakEvenDn: %8.4f\n", ops.breakevendn);
 printf("Break Up   : %8.4f\n", ops.pctbreakevenup);
 printf("Break Dn   : %8.4f\n", ops.pctbreakevendn);

/* Test Data for  Short Strangle */
/* Sell a call option with strike of 200    and price of  .80 */
/* Sell a put  option with strike of 170    and price of  .60 */
 i = taOptionInit(&ops);
 ops.stockprice = 185;
 ops.sell[0].strike = 200;	/* Sell a Call */
 ops.sell[0].price = .80;
 ops.sell[0].type = taCALL;
 ops.sell[1].strike = 170;	/* Sell a Put  */
 ops.sell[1].price = .60;
 ops.sell[1].type = taPUT;

 i = taShortStrangle(&ops);
 printf("\nShort Strangle\n");
 printf("Stock      : %8.4f\n", ops.stockprice);
 printf("Buy Strike : %8.4f\n", ops.sell[0].strike);
 printf("Buy Price  : %8.4f\n", ops.sell[0].price);
 printf("Buy Strike : %8.4f\n", ops.sell[1].strike);
 printf("Buy Price  : %8.4f\n", ops.sell[1].price);
 printf("Risk       : %8.4f\n", ops.riskamt);
 printf("Max Profit : %8.4f\n", ops.maxprofit);
 printf("RiskReward : %8.4f\n", ops.riskreward);
 printf("BreakEvenUp: %8.4f\n", ops.breakevenup);
 printf("BreakEvenDn: %8.4f\n", ops.breakevendn);
 printf("Break Up   : %8.4f\n", ops.pctbreakevenup);
 printf("Break Dn   : %8.4f\n", ops.pctbreakevendn);

/* TBill Yield */
 printf("\nTBill Discount Yield\n");
 bond.facevalue = 1000;
 bond.price = 990;
 bond.coupon = 0;
 bond.pymtsperyr = 0;
 bond.maturitydate = taDateTxtToJulian("1991/09/11", DTFORMAT_CCYY_MM_DD);
 asofdate = taDateTxtToJulian("1991/01/01", DTFORMAT_CCYY_MM_DD);
 yield = taTBillDiscYield(&bond, asofdate);
 printf("Face : %7.3f\nPrice: %7.3f\nFrom : %s\nTo   : %s\nYield: %7.5f\n",
	bond.facevalue, bond.price, "1991/01/01", "1991/09/11", yield);

/* TBill Equivalent Yield */
 printf("\nTBill Bond Equivalent Yield\n");
 bond.facevalue = 1000;
 bond.price = 990;
 bond.coupon = 0;
 bond.pymtsperyr = 0;
 bond.maturitydate = taDateTxtToJulian("1991/09/11", DTFORMAT_CCYY_MM_DD);
 asofdate = taDateTxtToJulian("1991/01/01", DTFORMAT_CCYY_MM_DD);
 yield = taTBillBondEqYield(&bond, asofdate);
 printf("Face : %7.3f\nPrice: %7.3f\nFrom : %s\nTo   : %s\nYield: %7.5f\n",
	bond.facevalue, bond.price, "1991/01/01", "1991/09/11", yield);

/* Test Data for Present Value  */
 printf("\nPresent Values\n");
 tvalue.principal = 1;
 tvalue.rate = 9;
 tvalue.periods = 6;
 tvalue.compound = taANNUAL_PAYMENTS;
 pv = taPresentValue(&tvalue);
 printf("P       : %9.3f\nDisc    : %4.2f\nPeriods : %3.2f\nCompound: %2d\nPV      : %7.3f\n",
	tvalue.principal, tvalue.rate, tvalue.periods, tvalue.compound, pv);

/* Test Data for Future  Value */
 printf("\nFuture Values\n");
 tvalue.principal = 1;
 tvalue.rate = 12;
 tvalue.periods = 11;
 tvalue.compound = taANNUAL_PAYMENTS;
 fv = taFutureValue(&tvalue);
 printf("P       : %9.3f\nDisc    : %4.2f\nPeriods : %3.2f\nCompound: %2d\nPV      : %7.3f\n",
	tvalue.principal, tvalue.rate, tvalue.periods, tvalue.compound, fv);

/* Test Data for Future  Value  Principal Required for Future Value */
/* For this calculation the number of periods is ALWAYS years */
 printf("\nFuture Value Principal\n");
 tvalue.rate = 6;
 tvalue.periods = 6;
 tvalue.compound = taANNUAL_PAYMENTS;
 fv = 70.93;
 tvalue.principal = taFutureValuePrinc(&tvalue, fv);
 printf("P       : %9.3f\nDisc    : %4.2f\nPeriods : %3.2f\nCompound: %2d\nPV      : %7.3f\n",
	tvalue.principal, tvalue.rate, tvalue.periods, tvalue.compound, fv);

/* Test Data for Future  Value  Interest  Required for Future Value */
/* For this calculation the number of periods is ALWAYS years */
/* and compounding is always annual                        */
 printf("\nFuture Value Interest Rate\n");
 tvalue.principal = 50;
 tvalue.periods = 6;
 fv = 70.93;
 tvalue.rate = taFutureValueInt(&tvalue, fv);
 printf("P       : %9.3f\nDisc    : %4.2f\nPeriods : %3.2f\nCompound: %2d\nPV      : %7.3f\n",
	tvalue.principal, tvalue.rate, tvalue.periods, tvalue.compound, fv);

/* Test Data for Future  Value  Annuity */
 printf("\nFuture Value Annuity\n");
 tvalue.principal = 1;
 tvalue.rate = 18;
 tvalue.periods = 11;
 pv = taFutureValueAnnuity(&tvalue);
 printf("P       : %9.3f\nDisc    : %4.2f\nPeriods : %3.2f\nCompound: %2d\nPV      : %7.3f\n",
	tvalue.principal, tvalue.rate, tvalue.periods, tvalue.compound, pv);

/* Test Data for Present Value  Annuity */
 printf("\nPresent Value Annuity\n");
 tvaluea.principal = 1;
 tvaluea.discrate = 7;
 tvaluea.numpayments = 11;
 pv = taPresentValueAnnuity(&tvaluea);
 printf("P       : %9.3f\nDisc    : %4.2f\nPeriods : %3.2f\nCompound: %2d\nPV      : %7.3f\n",
	tvaluea.principal, tvaluea.discrate, tvaluea.numpayments, pv);

/* Yield to Maturity */
 printf("\nYield to Maturity\n");
 bond.facevalue = 1000;
 bond.price = 1076.16;
 bond.coupon = 114;
 bond.maturitydate = taDateTxtToJulian("1990/03/01", DTFORMAT_CCYY_MM_DD);
 asofdate = taDateTxtToJulian("1987/02/18", DTFORMAT_CCYY_MM_DD);
 ytm = taYieldtoMaturity(&bond, asofdate);
 printf("Face      : %8.4f\n", bond.facevalue);
 printf("Bond Price: %8.4f\n", bond.price);
 printf("Coupon    : %8.4f\n", bond.coupon);
 printf("Pays/Yr   : %2d\n", bond.pymtsperyr);
 printf("From      : %s\n", "1987/02/18");
 printf("To        : %s\n", "1990/03/01");
 printf("DiscRate  : %8.4f\n", discrate);
 printf("YTM       : %8.4f\n", ytm);

/* Bond Pricing */
 printf("\nBond Pricing\n");
 bond.facevalue = 1000;
 bond.price = 0;
 bond.coupon = 80;
 bond.pymtsperyr = taANNUAL_PAYMENTS;
 bond.maturitydate = taDateTxtToJulian("1994/12/10", DTFORMAT_CCYY_MM_DD);
 asofdate = taDateTxtToJulian("1991/12/20", DTFORMAT_CCYY_MM_DD);
 discrate = 7.5;
 bond.price = taBondPricing(&bond, discrate, asofdate);
 printf("Face      : %8.4f\n", bond.facevalue);
 printf("Bond Price: %8.4f\n", bond.price);
 printf("Coupon    : %8.4f\n", bond.coupon);
 printf("Pays/Yr   : %2d\n", bond.pymtsperyr);
 printf("From      : %s\n", "1991/12/20");
 printf("To        : %s\n", "1994/12/10");
 printf("DiscRate  : %8.4f\n", discrate);

 printf("\nBond Duration\n");
 bond.facevalue = 1000;
 bond.price = 1108.15;
 bond.coupon = 150;
 bond.pymtsperyr = 0;
 bond.maturitydate = taDateTxtToJulian("1996/01/01", DTFORMAT_CCYY_MM_DD);
 asofdate = taDateTxtToJulian("1991/01/01", DTFORMAT_CCYY_MM_DD);
 discrate = 12;
 bd = taBondDuration(&bond, discrate, asofdate);
 printf("Face      : %8.4f\n", bond.facevalue);
 printf("Bond Price: %8.4f\n", bond.price);
 printf("Coupon    : %8.4f\n", bond.coupon);
 printf("Pays/Yr   : %2d\n", bond.pymtsperyr);
 printf("From      : %s\n", "1991/01/01");
 printf("To        : %s\n", "1996/01/01");
 printf("DiscRate  : %8.4f\n", discrate);
 printf("Duration  : %8.4f\n", bd);

 printf("\nBond Duration Price Change\n");
 olddiscrate = 12;
 discrate = 11;
 bdp = taBondDurationPriceChg(bond.price, olddiscrate, bd, discrate);
 printf("Face       : %8.4f\n", bond.facevalue);
 printf("Bond Price : %8.4f\n", bond.price);
 printf("Coupon     : %8.4f\n", bond.coupon);
 printf("Pays/Yr    : %2d\n", bond.pymtsperyr);
 printf("From       : %s\n", "1991/01/01");
 printf("To         : %s\n", "1996/01/01");
 printf("DiscRate   : %8.4f\n", discrate);
 printf("Old IntRate: %8.4f\n", olddiscrate);
 printf("Duration   : %8.4f\n", bd);
 printf("Price Chg  : %8.4f\n", bdp);

 printf("\nBond Accrued Interest\n");
 accrint = taBondAccrInt(90,
	     taDateTxtToJulian("1991/05/09", DTFORMAT_CCYY_MM_DD),
	     taDateTxtToJulian("1991/09/01", DTFORMAT_CCYY_MM_DD),
	     365);
 printf("Coupon    : %8.4f\n", 90.00);
 printf("From      : %s\n", "1991/05/09");
 printf("To        : %s\n", "1991/09/11");
 printf("Days in Yr: %d\n", 365);
 printf("Accr Int  : %8.4f\n", accrint);
}


int      taOptionPrint(struct taOptionSpreads *ops, char *title)
{
 printf("\n%s\n", title);
 printf("Stock Price : %8.4f\n", ops->stockprice);
 printf("Buy  Strike : %8.4f\n", ops->buy[0].strike);
 printf("Buy  Price  : %8.4f\n", ops->buy[0].price);
 printf("Sell Strike : %8.4f\n", ops->sell[0].strike);
 printf("Sell Price  : %8.4f\n", ops->sell[0].price);
 printf("Risk        : %8.4f\n", ops->riskamt);
 printf("Max Profit  : %8.4f\n", ops->maxprofit);
 printf("RiskReward  : %8.4f\n", ops->riskreward);
 printf("BreakEven Up: %8.4f\n", ops->breakevenup);
 printf("BreakEven Dn: %8.4f\n", ops->breakevendn);
 printf("Break Chg Up: %8.4f\n", ops->pctbreakevenup);
 printf("Break Chg Dn: %8.4f\n", ops->pctbreakevendn);
}
