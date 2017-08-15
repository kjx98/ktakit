/*  Technical Analysis Programmer's Toolkit Copyright (C) 1997 KTA */

/*  Sample Candlestick Program   */

#include <stdio.h>
#include <stdlib.h>
#include "ktakit.h"

int      main()
{
    int      err = 0, pos = 0, start = 0, stop = 0;
    int      barcount = -1, allocate = 1;
    long     barstart = 0;
    char     path[71] = ".";
    char     name[21] = "IBM.txt";
    char     infoname[21]= "txtInfo.txt";
    char     buffer[40];
    taBars   b1;
    taArray  indicators;
    struct taCandleInfo info;

    /* read bar data from text file */
    if ((err = taFILEread(infoname, &b1, path, name, barstart, barcount, allocate)))
    {
        taErrDesc(err, buffer);
        printf("%s Error in taFILEread\n", buffer);
        exit(err);
    }

    /* allocate an array to store indicators */
    if ((err = taAllocArray(&indicators, b1.size)))
    {
        taErrDesc(err, buffer);
        printf("%s Error in taAllocArray\n", buffer);
        exit(err);
    }

    /* set candle info variables */
    info.largeshadow = 2;
    info.smallshadow = .75;
    info.equalpct = .005;

    /* perform fuzzy logic candlestick analysis */
    if ((err = taCandleConstruct(&b1, &info)))
    {
        taErrDesc(err, buffer);
        printf("%s Error in taCandleConstruct\n", buffer);
        exit(err);
    }

    /* fill indicators array with occurences of one candlestick formation */
    start = 0;
    stop = b1.size - 1;
    taCandleFill(&b1, &info, &indicators, "taBeltHoldLineBull", start, stop);

    /* print value of each individual candlestick formation along with indicators array  */
    for (pos = 0; pos < stop; pos++)
    {
        taJulianToDateTxt(taArrayItem(b1.dt, pos), DTFORMAT_YY_MM_DD, "/", buffer);
        printf("%s ", buffer);
        printf("%1.0f ", taArrayItem(indicators, pos));
        if (taAbandonedBabyBottom(&b1, &info, pos) != 0)
            printf("AbandonedBabyBottom ");
        if (taAbandonedBabyTop(&b1, &info, pos) != 0)
            printf("AbandonedBabyTop ");
        if (taAdvanceBlock(&b1, &info, pos) != 0)
            printf("AdvanceBlock ");
        if (taBeltHoldLineBear(&b1, &info, pos) != 0)
            printf("BeltHoldLineBear ");
        if (taBeltHoldLineBull(&b1, &info, pos) != 0)
            printf("BeltHoldLineBull ");
        if (taCounterAttackBear(&b1, &info, pos) != 0)
            printf("CounterAttackBear ");
        if (taCounterAttackBull(&b1, &info, pos) != 0)
            printf("CounterAttackBull ");
        if (taDarkCloudCover(&b1, &info, pos) != 0)
            printf("DarkCloudCover ");
        if (taEngulfingLineBear(&b1, &info, pos) != 0)
            printf("EngulfingLineBear ");
        if (taEngulfingLineBull(&b1, &info, pos) != 0)
            printf("EngulfingLineBull ");
        if (taEveningDojiStar(&b1, &info, pos) != 0)
            printf("EveningDojiStar ");
        if (taEveningStar(&b1, &info, pos) != 0)
            printf("EveningStar ");
        if (taFalling3Method(&b1, &info, pos) != 0)
            printf("Falling3Method ");
        if (taGravestoneDoji(&b1, &info, pos) != 0)
            printf("GravestoneDoji ");
        if (taHammer(&b1, &info, pos) != 0)
            printf("Hammer ");
        if (taHangingman(&b1, &info, pos) != 0)
            printf("Hangingman ");
        if (taHaramiBlack(&b1, &info, pos) != 0)
            printf("HaramiBlack ");
        if (taHaramiCrossBlack(&b1, &info, pos) != 0)
            printf("HaramiCrossBlack ");
        if (taHaramiCrossWhite(&b1, &info, pos) != 0)
            printf("HaramiCrossWhite ");
        if (taHaramiWhite(&b1, &info, pos) != 0)
            printf("HaramiWhite ");
        if (taHighWave(&b1, &info, pos) != 0)
            printf("HighWave ");
        if (taInNeckLine(&b1, &info, pos) != 0)
            printf("InNeckLine ");
        if (taInvertedHammer(&b1, &info, pos) != 0)
            printf("InvertedHammer ");
        if (taLongLeggedDoji(&b1, &info, pos) != 0)
            printf("LongLeggedDoji ");
        if (taMatHoldPattern(&b1, &info, pos) != 0)
            printf("MatHoldPattern ");
        if (taMorningDojiStar(&b1, &info, pos) != 0)
            printf("MorningDojiStar ");
        if (taMorningStar(&b1, &info, pos) != 0)
            printf("MorningStar ");
        if (taPiercingLine(&b1, &info, pos) != 0)
            printf("PiercingLine ");
        if (taRising3Method(&b1, &info, pos) != 0)
            printf("Rising3Method ");
        if (taSeperatingLineBear(&b1, &info, pos) != 0)
            printf("SeperatingLineBear ");
        if (taSeperatingLineBull(&b1, &info, pos) != 0)
            printf("SeperatingLineBull ");
        if (taShootingStar(&b1, &info, pos) != 0)
            printf("ShootingStar ");
        if (taSideBySideWhiteGapDn(&b1, &info, pos) != 0)
            printf("SideBySideWhiteGapDn ");
        if (taSideBySideWhiteGapUp(&b1, &info, pos) != 0)
            printf("SideBySideWhiteGapUp ");
        if (taStalledPattern(&b1, &info, pos) != 0)
            printf("StalledPattern ");
        if (taTasukiDownsideGap(&b1, &info, pos) != 0)
            printf("TasukiDownsideGap ");
        if (taTasukiUpsideGap(&b1, &info, pos) != 0)
            printf("TasukiUpsideGap ");
        if (taThreeGapsUp(&b1, &info, pos) != 0)
            printf("ThreeGapsUp ");
        if (taThreeGapsDown(&b1, &info, pos) != 0)
            printf("ThreeGapsDown ");
        if (taThreeWhiteSoldiers(&b1, &info, pos) != 0)
            printf("ThreeWhiteSoldiers ");
        if (taThrustingLine(&b1, &info, pos) != 0)
            printf("ThrustingLine ");
        if (taUpsideGap2Crows(&b1, &info, pos) != 0)
            printf("UpsideGap2Crows ");
        printf("\n");
    }
    exit(0);
}


