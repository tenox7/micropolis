#include "sim.h"


float roadPercent = 0.0;
float policePercent = 0.0;
float firePercent = 0.0;
QUAD roadValue;
QUAD policeValue;
QUAD fireValue;
QUAD roadMaxValue;
QUAD policeMaxValue;
QUAD fireMaxValue;
int MustDrawCurrPercents = 0;
int MustDrawBudgetWindow = 0;
int SetBudget(char *flowStr, char *previousStr,
	      char *currentStr, char *collectedStr, short tax);


void InitFundingLevel(void)
{
  firePercent = 1.0;		/* 1.0 */
  fireValue = 0;
  policePercent = 1.0;		/* 1.0 */
  policeValue = 0;
  roadPercent = 1.0;		/* 1.0 */
  roadValue = 0;
  drawBudgetWindow();
  drawCurrPercents();
}


DoBudget()
{
  DoBudgetNow(0);
}


DoBudgetFromMenu()
{
  DoBudgetNow(1);
}


DoBudgetNow(int fromMenu)
{
  QUAD yumDuckets;
  QUAD total;
  QUAD moreDough;
  QUAD fireInt, policeInt, roadInt;

  fireInt = (int)(((float)FireFund) * firePercent);
  policeInt = (int)(((float)PoliceFund) * policePercent);
  roadInt = (int)(((float)RoadFund) * roadPercent);

  total = fireInt + policeInt + roadInt;

  yumDuckets = TaxFund + TotalFunds;

  if (yumDuckets > total) {
    fireValue = fireInt;
    policeValue = policeInt;
    roadValue = roadInt;
  } else if (total > 0) {
    if (yumDuckets > roadInt) {
      roadValue = roadInt;
      yumDuckets -= roadInt;

      if (yumDuckets > fireInt) {
	fireValue = fireInt;
	yumDuckets -= fireInt;

	if (yumDuckets > policeInt) {
	  policeValue = policeInt;
	  yumDuckets -= policeInt;
	} else {
	  policeValue = yumDuckets;
	  if (yumDuckets > 0)
	    policePercent = ((float)yumDuckets) / ((float)PoliceFund);
	  else
	    policePercent = 0.0;
	}
      } else {
	fireValue = yumDuckets;
	policeValue = 0;
	policePercent = 0.0;
	if (yumDuckets > 0)
	  firePercent = ((float)yumDuckets) / ((float)FireFund);
	else
	  firePercent = 0.0;
      }
    } else {
      roadValue = yumDuckets;
      if (yumDuckets > 0)
	roadPercent = ((float)yumDuckets) / ((float)RoadFund);
      else
	roadPercent = 0.0;

      fireValue = 0;
      policeValue = 0;
      firePercent = 0.0;
      policePercent = 0.0;
    }
  } else {
    fireValue = 0;
    policeValue = 0;
    roadValue = 0;
    firePercent = 1.0;
    policePercent = 1.0;
    roadPercent = 1.0;
  }

  fireMaxValue = FireFund;
  policeMaxValue = PoliceFund;
  roadMaxValue = RoadFund;

  drawCurrPercents();

 noMoney:
  if ((!autoBudget) || fromMenu) {
    if (!autoBudget) {
      /* TODO: append the the current year to the budget string */
    }

    ShowBudgetWindowAndStartWaiting();

    if (!fromMenu) {
      FireSpend = fireValue;
      PoliceSpend = policeValue;
      RoadSpend = roadValue;

      total = FireSpend + PoliceSpend + RoadSpend;
      moreDough = (QUAD)(TaxFund - total);
      Spend(-moreDough);
    }
    drawBudgetWindow();
    drawCurrPercents();
    DoUpdateHeads();

  } else { /* autoBudget & !fromMenu */
    if ((yumDuckets) > total) {
      moreDough = (QUAD)(TaxFund - total);
      Spend(-moreDough);
      FireSpend = FireFund;
      PoliceSpend = PoliceFund;
      RoadSpend = RoadFund;
      drawBudgetWindow();
      drawCurrPercents();
      DoUpdateHeads();
    } else {
      autoBudget = 0; /* XXX: force autobudget */
      MustUpdateOptions = 1;
      ClearMes();
      SendMes(29);
      goto noMoney;
    }
  }
}


drawBudgetWindow(void)
{
  MustDrawBudgetWindow = 1;
}


ReallyDrawBudgetWindow(void)
{
  short cashFlow, cashFlow2;
  char numStr[256], dollarStr[256], collectedStr[256],
       flowStr[256], previousStr[256], currentStr[256];

  cashFlow = TaxFund - fireValue - policeValue - roadValue;

  cashFlow2 = cashFlow;
  if (cashFlow < 0)   {
    cashFlow = -cashFlow;
    sprintf(numStr, "%d", cashFlow);
    makeDollarDecimalStr(numStr, dollarStr);
    sprintf(flowStr, "-%s", dollarStr);
  } else {
    sprintf(numStr, "%d", cashFlow);
    makeDollarDecimalStr(numStr, dollarStr);
    sprintf(flowStr, "+%s", dollarStr);
  }

  sprintf(numStr, "%d", TotalFunds);
  makeDollarDecimalStr(numStr, previousStr);

  sprintf(numStr, "%d", cashFlow2 + TotalFunds);
  makeDollarDecimalStr(numStr, currentStr);

  sprintf(numStr, "%d", TaxFund);
  makeDollarDecimalStr(numStr, collectedStr);

  SetBudget(flowStr, previousStr, currentStr, collectedStr, CityTax);
}


drawCurrPercents(void)
{
  MustDrawCurrPercents = 1;
}


ReallyDrawCurrPercents(void)
{
  char num[256];
  char fireWant[256], policeWant[256], roadWant[256];
  char fireGot[256], policeGot[256], roadGot[256];

  sprintf(num, "%d", fireMaxValue);
  makeDollarDecimalStr(num, fireWant);

  sprintf(num, "%d", policeMaxValue);
  makeDollarDecimalStr(num, policeWant);

  sprintf(num, "%d", roadMaxValue);
  makeDollarDecimalStr(num, roadWant);

  sprintf(num, "%d", (int)(fireMaxValue * firePercent));
  makeDollarDecimalStr(num, fireGot);

  sprintf(num, "%d", (int)(policeMaxValue * policePercent));
  makeDollarDecimalStr(num, policeGot);

  sprintf(num, "%d", (int)(roadMaxValue * roadPercent));
  makeDollarDecimalStr(num, roadGot);

  SetBudgetValues(roadGot, roadWant,
		  policeGot, policeWant,
		  fireGot, fireWant);
}


UpdateBudgetWindow()
{
  if (MustDrawCurrPercents) {
    ReallyDrawCurrPercents();
    MustDrawCurrPercents = 0;
  }
  if (MustDrawBudgetWindow) {
    ReallyDrawBudgetWindow();
    MustDrawBudgetWindow = 0;
  }
}


UpdateBudget()
{
  drawCurrPercents();
  drawBudgetWindow();
  Eval("UIUpdateBudget");
}


ShowBudgetWindowAndStartWaiting()
{
  Eval("UIShowBudgetAndWait");

  Pause();
}


SetBudget(char *flowStr, char *previousStr,
	  char *currentStr, char *collectedStr, short tax)
{
  char buf[256];

  sprintf(buf, "UISetBudget {%s} {%s} {%s} {%s} {%d}",
	  flowStr, previousStr, currentStr, collectedStr, tax);
  Eval(buf);
}


SetBudgetValues(char *roadGot, char *roadWant,
		char *policeGot, char *policeWant,
		char *fireGot, char *fireWant)
{
  char buf[256];

  sprintf(buf, "UISetBudgetValues {%s} {%s} %d {%s} {%s} %d {%s} {%s} %d",
	  roadGot, roadWant, (int)(roadPercent * 100),
	  policeGot, policeWant, (int)(policePercent * 100),
	  fireGot, fireWant, (int)(firePercent * 100));
  Eval(buf);
}


