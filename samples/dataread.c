/* Technical Analysis Programmer's Toolkit Copyright (C) 1997 KTA */

/*  Sample Data Access Program */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include "ktakit.h"

int      main(int argc, char *argv[])
{
 int      i1 = 0, err = 0, count = 0, allocate = 1, append = 0;
 unsigned date = 0, time = 0, actualdate = 0, actualtime = 0;
 long     start = 0;
 char    *type, *path, *name;
 char     buffer[40];
 taBars   b1;

 fputs("Technical Analysis Programmer's Toolkit, Copyright (c) 1997 KTA\n", stderr);
 fputs("Sample Data Reader Program\n\n", stderr);

 if (argc < 4 || !strcmp(argv[1], "/?"))
 {
  fputs("PARAMETERS: type path name [count] [/Dyymmdd] [/Thhmmss] [/P]\n", stderr);
  fputs("      type: CSI, CT, DJ, FS, TC, TT or text info file name\n", stderr);
  fputs("      name: name, symbol or ? for a list\n", stderr);
  fputs("     count: negative means count from end, ignore date/time\n\n", stderr);
  fputs("   Example (CompuTrac): dataread CT          C:\\KTAKIT IBM     -10\n", stderr);
  fputs("   Example (Text file): dataread txtInfo.txt C:\\KTAKIT IBM.txt -10\n", stderr);
  exit(-1);
 }
 type = argv[1];
 path = argv[2];
 name = argv[3];
 for (i1 = 4; i1 < argc; i1++)
 {
  if (argv[i1][0] != '/')
   count = atoi(argv[i1]);
  if (!(strnicmp(argv[i1], "/D", 2)))
   date = taDateTxtToJulian(argv[i1] + 2, DTFORMAT_YYMMDD);
  if (!(strnicmp(argv[i1], "/T", 2)))
   time = taTimeTxtToJulian(argv[i1] + 2, TIFORMAT_HHMMSS);
  if (!(strnicmp(argv[i1], "/P", 2)))
   append = 1;
 }
 if (!strcmp(name, "?"))
 {
  if ((err = taFILElist(type, path, "stdout", append)))
  {
   taErrDesc(err, buffer);
   printf("%s Error in taFILElist\n", buffer);
  }
  exit (err);
 }
 if (count < 0)
 {
  start = -1;
  count = -count;
 }
 else
 {
  if (count == 0)
   count = -1;
  if (date != 0 || time != 0)
  {
   if ((start = taFILEfindrec(type, path, name, date, time, &actualdate, &actualtime)) < 0)
   {
    err = (int)start;
    taErrDesc(err, buffer);
    printf("%s Error in taFILEfindrec\n", buffer);
    exit (err);
   }
  }
 }
 if ((err = taFILEread(type, &b1, path, name, start, count, allocate)))
 {
  taErrDesc(err, buffer);
  printf("%s Error in taFILEread\n", buffer);
  exit (err);
 }
 taFILEprintinfo(&b1, "stdout", append);
 if (append)
 {
  printf("Press any key to continue . . .");
  getch();
  printf("\r                               \r");
 }
 taArrayPrintf("stdout", append, 0, b1.size - 1,
           "NUMBER    DATE     TIME     OPEN     HIGH      LOW    CLOSE     VOLUME   OP.INT\n",
           "%05r %ymd/i %hms:i %8.3f %8.3f %8.3f %8.3f %10.0f %8.0f\n",
           b1.dt, b1.ti, b1.op, b1.hi, b1.lo, b1.cl, b1.vol, b1.oi);
 exit(0);
}
