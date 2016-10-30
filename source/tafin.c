/* Technical Analysis Programmer's Toolkit - Copyright (C) 1997 KTA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ktakit.h"


double DLL_EXPORT taApprxCumProbDist(double d1)
{
 double   d2 = 0, d3 = 0, d4 = 0, d5 = 0, acpd = 0;

 d2 = fabs(d1);
 d3 = 1 / (1 + 0.23164191 * d2);
 d4 = .39894232 * exp(-d1 * d1 / 2);
 d5 = ((((1.330274 * d3 - 1.821256) * d3 + 1.781478) * d3 - 0.3565638) * d3 + 0.319381511);
 if (d1 > 0)
  acpd = 1 - d4 * d3 * d5;
 else
  acpd = 1 - (1 - d4 * d3 * d5);
 return (acpd);
}


int DLL_EXPORT taBearCall(struct taOptionSpreads *ops)
{
 ops->maxprofit = ops->sell[0].price - ops->buy[0].price;
 ops->riskamt = fabs(ops->sell[0].strike - ops->buy[0].strike) - ops->maxprofit;
 ops->riskreward = ops->riskamt / ops->maxprofit;
 ops->breakevenup = ops->maxprofit + ops->sell[0].strike;
 ops->breakevendn = ops->maxprofit + ops->sell[0].strike;
 if (ops->stockprice > 0)
 {
  ops->pctbreakevenup = (ops->breakevenup - ops->stockprice) / ops->stockprice;
  ops->pctbreakevendn = (ops->breakevendn - ops->stockprice) / ops->stockprice;
 }
 else
  return (taErrInvalidParm);
 return (0);
}


int DLL_EXPORT taBearPut(struct taOptionSpreads *ops)
{
 ops->riskamt = ops->buy[0].price - ops->sell[0].price;
 ops->maxprofit = (ops->buy[0].strike - ops->sell[0].strike) - ops->riskamt;
 ops->riskreward = ops->riskamt / ops->maxprofit;
 ops->breakevenup = ops->buy[0].strike - ops->riskamt;
 ops->breakevendn = ops->buy[0].strike - ops->riskamt;
 if (ops->stockprice > 0)
 {
  ops->pctbreakevenup = (ops->breakevenup - ops->stockprice) / ops->stockprice;
  ops->pctbreakevendn = (ops->breakevendn - ops->stockprice) / ops->stockprice;
 }
 else
  return (taErrInvalidParm);
 return (0);
}


double DLL_EXPORT taBlackScholes(struct taOption * opt, double stockprice,
            double intrate, double variance, unsigned int asofdate)
{
 double   d1 = 0, d2 = 0, bs = 0, e = 2.71828, f1 = 0, daysdiff = 0;

 intrate = intrate / 100;
 daysdiff = taJulianDif(asofdate, opt->expiredate);
 f1 = daysdiff / 360.00;
 d1 = (log(stockprice / opt->strike) + (intrate * f1)) /
      (sqrt(variance) * sqrt(f1)) + .5 * sqrt(variance) * sqrt(f1);
 d2 = d1 - (sqrt(variance) * sqrt(f1));
 d1 = taApprxCumProbDist(d1);
 d2 = taApprxCumProbDist(d2);
 bs = stockprice * d1 - pow(e, (-1 * intrate * f1)) * opt->strike * d2;
 return (bs);
}


double DLL_EXPORT taBondAccrInt(double coupon, unsigned int lastpaydate, unsigned int asofdate, double daysinyr)
{
 int    daysfromlastpay = 0;

 daysfromlastpay = taJulianDif(asofdate, lastpaydate);
 return ((daysfromlastpay / (daysinyr / 2)) * (coupon / 2));
}


double DLL_EXPORT taBondDuration(struct taBondInfo *bond, double discrate, unsigned int asofdate)
{
 struct taTimeValue tvalue;
 double   f1 = 0, sumpv = 0, pv = 0;
 int    i1 = 0, daysdiff = 0, yrs = 0;

 daysdiff = taJulianDif(asofdate, bond->maturitydate);
 yrs = daysdiff / 365;
 for (i1 = 0; i1 < yrs; i1++)
 {
  if (i1 < yrs - 1)
   f1 = bond->coupon * (i1 + 1);
  else
   f1 = (bond->coupon + bond->facevalue) * (i1 + 1);
  tvalue.principal = f1;
  tvalue.rate = discrate;
  tvalue.periods = i1 + 1;
  tvalue.compound = 1;
  pv = taPresentValue(&tvalue);
  sumpv += pv;
 }
 return (sumpv / bond->price);
}


double DLL_EXPORT taBondDurationPriceChg(double price, double olddiscrate, double duration, double discrate)
{
 if (discrate < 1)
  return (taErrInvalidParm);
 return ((-1 * duration) * (((discrate - olddiscrate) / 100) / (1 + (discrate / 100))) * price);
};

double DLL_EXPORT taBondPricing(struct taBondInfo *bond, double discrate, unsigned int asofdate)
{
 double   c1 = 0, f1 = 0, f2 = 0, period = 0;
 double   f0 = 0;
 double   yrs = 0;
 int    daysdiff = 0;

 c1 = bond->coupon / bond->pymtsperyr;
 period = (double) 1.00 / bond->pymtsperyr;
 daysdiff = taJulianDif(asofdate, bond->maturitydate);
 yrs = ((daysdiff + 182) / 365);
 for (f0 = period; f0 <= yrs; f0 += period)
 {
  if (f0 < yrs)
   f1 = c1 / pow(1 + ((discrate / 100)), f0);
  else
   f1 = (c1 + bond->facevalue) / pow(1 + ((discrate / 100)), f0);
  f2 += f1;
 }
 return (f2);
}


int DLL_EXPORT taBullCall(struct taOptionSpreads *ops)
{
 ops->riskamt = ops->buy[0].price - ops->sell[0].price;
 ops->maxprofit = (ops->sell[0].strike - ops->buy[0].strike) - ops->riskamt;
 ops->riskreward = ops->riskamt / ops->maxprofit;
 ops->breakevenup = ops->riskamt + ops->buy[0].strike;
 ops->breakevendn = ops->riskamt + ops->buy[0].strike;
 if (ops->stockprice > 0)
 {
  ops->pctbreakevenup = (ops->breakevenup - ops->stockprice) / ops->stockprice;
  ops->pctbreakevendn = (ops->breakevendn - ops->stockprice) / ops->stockprice;
 }
 else
  return (taErrInvalidParm);
 return (0);
}


int DLL_EXPORT taBullPut(struct taOptionSpreads *ops)
{
 ops->maxprofit = ops->sell[0].price - ops->buy[0].price;
 ops->riskamt = (ops->sell[0].strike - ops->buy[0].strike) - ops->maxprofit;
 ops->riskreward = ops->riskamt / ops->maxprofit;
 ops->breakevenup = ops->sell[0].strike - ops->maxprofit;
 ops->breakevendn = ops->sell[0].strike - ops->maxprofit;
 if (ops->stockprice > 0)
 {
  ops->pctbreakevenup = (ops->breakevenup - ops->stockprice) / ops->stockprice;
  ops->pctbreakevendn = (ops->breakevendn - ops->stockprice) / ops->stockprice;
 }
 else
  return (taErrInvalidParm);
 return (0);
}

double DLL_EXPORT taFutureValue(struct taTimeValue *tvalue)
{
 double   power = 0;
 double   fv = 0;
 double   f1 = 0;

 f1 = tvalue->rate / 100;
 power = tvalue->compound * tvalue->periods;
 if (tvalue->compound > 0)
  f1 = f1 / tvalue->compound;
 else
  return (taErrInvalidParm);

 fv = tvalue->principal * pow(1 + f1, power);
 return (fv);
}


double DLL_EXPORT taFutureValueAnnuity(struct taTimeValue *tvalue)
{
 double   fv = 0;

 fv = tvalue->rate / 100;
 fv = ((pow(1 + fv, tvalue->periods) - 1) / fv);
 fv = fv * tvalue->principal;
 return (fv);
}


/* Interest Rate required for an investment to reach a Future Value */
double DLL_EXPORT taFutureValueInt(struct taTimeValue *tvalue, double futurevalue)
{
 double   power = 0;

 power = 1 / tvalue->periods;
 tvalue->rate = 100 * (pow((futurevalue / tvalue->principal), power) - 1);
 return (tvalue->rate);
}


/* Minimum Investment required for Future Value */
double DLL_EXPORT taFutureValuePrinc(struct taTimeValue *tvalue, double futurevalue)
{
 double   power = 0;
 double   f1 = 0;

 f1 = tvalue->rate / 100;
 power = tvalue->compound * tvalue->periods;
 if (tvalue->compound > 0)
  f1 = f1 / tvalue->compound;
 else
  return (taErrInvalidParm);

 tvalue->principal = futurevalue / pow(1 + f1, power);
 return (tvalue->principal);
}


int DLL_EXPORT taLongStraddle(struct taOptionSpreads *ops)
{
 ops->maxprofit = 0;
 ops->riskamt = ops->buy[0].price + ops->buy[1].price;
 ops->riskreward = 0;
 ops->breakevendn = ops->buy[0].strike - ops->riskamt;
 ops->breakevenup = ops->buy[0].strike + ops->riskamt;
 if (ops->stockprice > 0)
 {
  ops->pctbreakevenup = (ops->breakevenup - ops->stockprice) / ops->stockprice;
  ops->pctbreakevendn = (ops->breakevendn - ops->stockprice) / ops->stockprice;
 }
 else
  return (taErrInvalidParm);
 return (0);
}


/* Order for Strangles is important CAll must be slot 0 PUT slot 1 */
int DLL_EXPORT taLongStrangle(struct taOptionSpreads *ops)
{
 if (ops->buy[0].type != taCALL || ops->buy[1].type != taPUT)
  return (taErrInvalidParm);
 ops->maxprofit = 0;
 ops->riskamt = ops->buy[0].price + ops->buy[1].price;
 ops->riskreward = 0;
 ops->breakevendn = ops->buy[1].strike - ops->riskamt;
 ops->breakevenup = ops->buy[0].strike + ops->riskamt;
 if (ops->stockprice > 0)
 {
  ops->pctbreakevenup = (ops->breakevenup - ops->stockprice) / ops->stockprice;
  ops->pctbreakevendn = (ops->breakevendn - ops->stockprice) / ops->stockprice;
 }
 else
  return (taErrInvalidParm);
 return (0);
}


double DLL_EXPORT taPresentValue(struct taTimeValue *tvalue)
{
 double   power = 0;
 double   fv = 0;
 double   f1 = 0;

 f1 = tvalue->rate / 100;
 power = tvalue->compound * tvalue->periods;
 if (tvalue->compound > 0)
  f1 = f1 / tvalue->compound;
 else
  return (taErrInvalidParm);

 fv = tvalue->principal * pow(1 + f1, -1 * power);
 return (fv);

}


double DLL_EXPORT taPresentValueAnnuity(struct taTimeValueA *tvaluea)
{
 double   f1 = 0, fv = 0;

 if (tvaluea->discrate > 0)
  f1 = tvaluea->discrate / 100;
 else
  return (taErrInvalidParm);

 fv = (1 - (1 / pow(1 + f1, tvaluea->numpayments))) / f1;
 fv = tvaluea->principal * fv;
 return (fv);

}


int DLL_EXPORT taShortStraddle(struct taOptionSpreads *ops)
{
 ops->maxprofit = ops->sell[0].price + ops->sell[1].price;
 ops->riskamt = 0;
 ops->riskreward = 0;
 ops->breakevendn = ops->sell[0].strike - ops->maxprofit;
 ops->breakevenup = ops->sell[0].strike + ops->maxprofit;
 if (ops->stockprice > 0)
 {
  ops->pctbreakevenup = (ops->breakevenup - ops->stockprice) / ops->stockprice;
  ops->pctbreakevendn = (ops->breakevendn - ops->stockprice) / ops->stockprice;
 }
 else
  return (taErrInvalidParm);
 return (0);
}


/* Order for Strangles is important CAll must be slot 0 PUT slot 1 */
int DLL_EXPORT taShortStrangle(struct taOptionSpreads *ops)
{
 if (ops->sell[0].type != taCALL || ops->sell[1].type != taPUT)
  return (taErrInvalidParm);
 ops->maxprofit = ops->sell[0].price + ops->sell[1].price;
 ops->riskamt = 0;
 ops->riskreward = 0;
 ops->breakevendn = ops->sell[1].strike - ops->maxprofit;
 ops->breakevenup = ops->sell[0].strike + ops->maxprofit;
 if (ops->stockprice > 0)
 {
  ops->pctbreakevenup = (ops->breakevenup - ops->stockprice) / ops->stockprice;
  ops->pctbreakevendn = (ops->breakevendn - ops->stockprice) / ops->stockprice;
 }
 else
  return (taErrInvalidParm);
 return (0);
}


int DLL_EXPORT taOptionInit(struct taOptionSpreads *ops)
{
 int    i1 = 0;

 for (i1 = 0; i1 < 2; i1++)
 {
  ops->buy[i1].price = 0;
  ops->buy[i1].strike = 0;
  ops->buy[i1].type = 0;
  ops->buy[i1].expiredate = 0;
 }
 for (i1 = 0; i1 < 2; i1++)
 {
  ops->sell[i1].price = 0;
  ops->sell[i1].strike = 0;
  ops->sell[i1].type = 0;
  ops->sell[i1].expiredate = 0;
 }
 ops->stockprice = 0;
 ops->maxprofit = 0;
 ops->riskreward = 0;
 ops->riskamt = 0;
 ops->breakevenup = 0;
 ops->breakevendn = 0;
 ops->pctbreakevenup = 0;
 ops->pctbreakevendn = 0;
 return (0);
}


double DLL_EXPORT taTBillDiscYield(struct taBondInfo *bond, unsigned int asofdate)
{
 int    daysdiff = 0;

 daysdiff = taJulianDif(asofdate, bond->maturitydate);
 return (((((bond->facevalue - bond->price) / daysdiff) * 360)) / bond->facevalue);
}


double DLL_EXPORT taTBillBondEqYield(struct taBondInfo *bond, unsigned int asofdate)
{
 int    daysdiff = 0;

 daysdiff = taJulianDif(asofdate, bond->maturitydate);
 return ((((bond->facevalue - bond->price) / daysdiff) * 365) / bond->price);
}


double DLL_EXPORT taYieldtoMaturity(struct taBondInfo *bond, unsigned int asofdate)
{
 int    daysdiff = 0;
 double   yrs = 0, f1 = 0;

 daysdiff = taJulianDif(asofdate, bond->maturitydate);
 yrs = daysdiff / 365.00;
 f1 = (bond->coupon / bond->price) +
  ((bond->facevalue - bond->price) / bond->price) / yrs;
 return (f1);
}
