#include "sim.h"


short MustUpdateFunds;
short MustUpdateOptions;
QUAD LastCityTime;
QUAD LastCityYear;
QUAD LastCityMonth;
QUAD LastFunds;
QUAD LastR, LastC, LastI;

char *dateStr[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};


void DoUpdateHeads()
{
  showValves();
  doTimeStuff();
  ReallyUpdateFunds();
  updateOptions();
}


void UpdateEditors()
{
  InvalidateEditors();
  DoUpdateHeads();
}


void UpdateMaps()
{
  InvalidateMaps();
}


void UpdateGraphs()
{
  ChangeCensus();
}


void UpdateEvaluation()
{
  ChangeEval();
}


void UpdateHeads()
{
  MustUpdateFunds = ValveFlag = 1;
  LastCityTime = LastCityYear = LastCityMonth = LastFunds = LastR = -999999;
  DoUpdateHeads();
}

UpdateFunds(void)
{
  MustUpdateFunds = 1;
//  Kick();
}


ReallyUpdateFunds(void)
{
  char localStr[256], dollarStr[256], buf[256];

  if (!MustUpdateFunds) return;

  MustUpdateFunds = 0;

  if (TotalFunds < 0) TotalFunds = 0;

  if (TotalFunds != LastFunds) {
    LastFunds = TotalFunds;
    sprintf(localStr, "%d", TotalFunds);
    makeDollarDecimalStr(localStr, dollarStr);

    sprintf(localStr, "Funds: %s", dollarStr);

    sprintf(buf, "UISetFunds {%s}", localStr);
    Eval(buf);
  }
}


doTimeStuff(void)
{
//  if ((CityTime >> 2) != LastCityTime) {
    updateDate();
//  }
}


updateDate(void)
{
  int y;
  int m;
  char str[256], buf[256];
  int megalinium = 1000000;

  LastCityTime = CityTime >> 2;

  y = ((int)CityTime / 48) + (int)StartingYear;
  m = ((int)CityTime % 48) >> 2;

  if (y >= megalinium) {
    SetYear(StartingYear);
    y = StartingYear;
    SendMes(-40);
  }

  doMessage();

  if ((LastCityYear != y) ||
      (LastCityMonth != m)) {

    LastCityYear = y;
    LastCityMonth = m;

    sprintf(str, "%s %d", dateStr[m], y);

    sprintf(buf,
	    //"UISetDate {%s} %d %d",
	    //str, m, y);
	    "UISetDate {%s}",
	    str);
    Eval(buf);
  }
}


showValves(void)
{
  if (ValveFlag) {
    drawValve();
    ValveFlag = 0;
  }
}


drawValve(void)
{
  double r, c, i;

  r = RValve;
  if (r < -1500) r = -1500;
  if (r > 1500) r = 1500;

  c = CValve;
  if (c < -1500) c = -1500;
  if (c > 1500) c = 1500;

  i = IValve;
  if (i < -1500) i = -1500;
  if (i > 1500) i = 1500;

  if ((r != LastR) ||
      (c != LastC) ||
      (i != LastI)) {
    LastR = r;
    LastC = c;
    LastI = i;
    SetDemand(r, c, i);
  }
}


SetDemand(double r, double c, double i)
{
  char buf[256];

  sprintf(buf, "UISetDemand %d %d %d",
	  (int)(r / 100), (int)(c / 100), (int)(i / 100));
  Eval(buf);
}


updateOptions()
{
  int options;

  if (MustUpdateOptions) {
    options = 0;
    if (autoBudget)	options |= 1;
    if (autoGo)		options |= 2;
    if (autoBulldoze)	options |= 4;
    if (!NoDisasters)	options |= 8;
    if (UserSoundOn)	options |= 16;
    if (DoAnimation)	options |= 32;
    if (DoMessages)	options |= 64;
    if (DoNotices)	options |= 128;

    MustUpdateOptions = 0;
    UpdateOptionsMenu(options);
  }
}


UpdateOptionsMenu(int options)
{
  char buf[256];
  sprintf(buf, "UISetOptions %d %d %d %d %d %d",
	  (options&1)?1:0, (options&2)?1:0,
	  (options&4)?1:0, (options&8)?1:0,
	  (options&16)?1:0, (options&32)?1:0);
  Eval(buf);
}


