#include "sim.h"


/* Stubs */


QUAD TotalFunds;
short PunishCnt;
short autoBulldoze, autoBudget;
QUAD LastMesTime;
short GameLevel;
short InitSimLoad;
short ScenarioID;
short SimSpeed;
short SimMetaSpeed;
short UserSoundOn;
char *CityName;
short NoDisasters;
short MesNum;
short EvalChanged;
short flagBlink;


Spend(int dollars)
{
  SetFunds(TotalFunds - dollars);
}


SetFunds(int dollars)
{
  TotalFunds = dollars;
  UpdateFunds();
}


/* Mac */

QUAD TickCount()
{
  struct timeval time;

  gettimeofday(&time, 0);

  return (QUAD)((time.tv_sec / 60) + (time.tv_usec * 1000000 / 60));
}


Ptr
NewPtr(size)
int size;
{
  return ((Ptr)calloc(size, sizeof(Byte)));
}


/* w_hlhandlers.c */

GameStarted()
{
  InvalidateMaps();
  InvalidateEditors();
  gettimeofday(&start_time, NULL);

  switch (Startup) {
  case -2: /* Load a city */
    if (LoadCity(StartupName)) {
      DoStartLoad();
      StartupName = NULL;
      break;
    }
    StartupName = NULL;
  case -1:
    if (StartupName != NULL) {
      setCityName(StartupName);
      StartupName = NULL;
    } else {
      setCityName("NowHere");
    }
    DoPlayNewCity();
    break;
  case 0:
    DoReallyStartGame();
    break;
  default: /* scenario number */
    DoStartScenario(Startup);
    break;
  }
}


DoPlayNewCity()
{
  Eval("UIPlayNewCity");
}


DoReallyStartGame()
{
  Eval("UIReallyStartGame");
}


DoStartLoad()
{
  Eval("UIStartLoad");
}


DoStartScenario(int scenario)
{
  char buf[256];

  sprintf(buf, "UIStartScenario %d", scenario);
  Eval(buf);
}


DropFireBombs()
{
  Eval("DropFireBombs");
}


InitGame()
{
  sim_skips = sim_skip = sim_paused = sim_paused_speed = heat_steps = 0;
  setSpeed(0);
}


ReallyQuit()
{
  sim_exit(0); // Just sets tkMustExit and ExitReturn
}


