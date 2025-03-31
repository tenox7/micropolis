#include "sim.h"


/* Initialize Stuff */


/* comefrom: loadFile bootUp main doMessage */
InitWillStuff()
{
  register short x, y, z;

  RandomlySeedRand();
  InitGraphMax();
  RoadEffect = 32;
  PoliceEffect = 1000;		/*added*/
  FireEffect = 1000;
  CityScore = 500;
  CityPop = -1;
  LastCityTime = -1;
  LastCityYear = -1;
  LastCityMonth = -1;
  LastFunds = -1;
  LastR = LastC = LastI = -999999;
  OverRide = 0;
  PendingTool = -1;

  MesNum = 0;
  MessagePort = 0;
  RoadFund = 0;
  PoliceFund = 0;
  FireFund = 0;
  UpdateDelayed = 0;
  ValveFlag = 1;

  DestroyAllSprites();

  DisasterEvent = 0;
  TaxFlag = 0;

  for (x = 0; x < HWLDX; x++) {
    for (y = 0; y < HWLDY; y++) {
      PopDensity[x][y] = 0;
      TrfDensity[x][y] = 0;
      PollutionMem[x][y] = 0;
      LandValueMem[x][y] = 0;
      CrimeMem[x][y] = 0;
    }
  }

  for (x = 0; x < QWX; x++) {
    for (y = 0; y < QWY; y++) {
      TerrainMem[x][y] = 0;
    }
  }

  for (x = 0; x < SmX; x++) {
    for (y = 0; y < SmY; y++) {
      RateOGMem[x][y] = 0;
      FireRate[x][y] = 0;
      ComRate[x][y] = 0;
      PoliceMap[x][y] = 0;
      PoliceMapEffect[x][y] = 0;
      FireRate[x][y] = 0;
    }
  }

  ResetLastKeys();
  DoNewGame();
  DoUpdateHeads();
}


ResetMapState()
{
  SimView *view;

  for (view = sim->map; view != NULL; view = view->next) {
    view->map_state = ALMAP;
  }
}


ResetEditorState()
{
  SimView *view;

  for (view = sim->editor; view != NULL; view = view->next) {
    view->tool_state = dozeState;
    view->tool_state_save = -1;
  }
}


