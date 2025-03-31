#include "sim.h"


/* Allocate Stuff */


short SMapX, SMapY;
short CChr, CChr9;

short RoadTotal, RailTotal, FirePop;

short ResPop, ComPop, IndPop, TotalPop, LastTotalPop;
short ResZPop, ComZPop, IndZPop, TotalZPop;
short HospPop, ChurchPop, StadiumPop;
short PolicePop, FireStPop;
short CoalPop, NuclearPop, PortPop, APortPop;

short NeedHosp, NeedChurch;
short CrimeAverage, PolluteAverage, LVAverage;

QUAD CityTime;
short StartingYear;

short *Map[WORLD_X];

short ResHisMax, Res2HisMax;
short ComHisMax, Com2HisMax;
short IndHisMax, Ind2HisMax;
short CensusChanged;

short MessagePort;
short MesX, MesY;

QUAD RoadSpend, PoliceSpend, FireSpend;
QUAD RoadFund, PoliceFund, FireFund;
short RoadEffect, PoliceEffect, FireEffect;
QUAD TaxFund;
short CityTax, TaxFlag;
unsigned char tileSynch = 0x01;

Byte *PopDensity[HWLDX];
Byte *TrfDensity[HWLDX];
Byte *PollutionMem[HWLDX];
Byte *LandValueMem[HWLDX];
Byte *CrimeMem[HWLDX];
Byte *tem[HWLDX];
Byte *tem2[HWLDX];
Byte *TerrainMem[QWX];
Byte *Qtem[QWX];

short RateOGMem[SmX][SmY];
short FireStMap[SmX][SmY];
short PoliceMap[SmX][SmY];
short PoliceMapEffect[SmX][SmY];

short FireRate[SmX][SmY];
short ComRate[SmX][SmY];
short STem[SmX][SmY];

Ptr terrainBase;
Ptr qTemBase;
Ptr tem1Base;
Ptr tem2Base;

Ptr popPtr;
Ptr trfPtr;
Ptr polPtr;
Ptr landPtr;
Ptr crimePtr;

Ptr auxPopPtr;
Ptr auxTrfPtr;
Ptr auxPolPtr;
Ptr auxLandPtr;
Ptr auxCrimePtr;
Ptr brettPtr;

unsigned short *mapPtr;
short *ResHis;
short *ComHis;
short *IndHis;
short *MoneyHis;
short *PollutionHis;
short *CrimeHis;
short *MiscHis;
short *PowerMap;


int initMapArrays(void)
{
  short i;
  unsigned short *auxPtr, *temp1;

  if (!mapPtr)
    mapPtr = (unsigned short *)NewPtr(sizeof(unsigned short) *
				      WORLD_X * WORLD_Y);
  auxPtr = mapPtr;

  for (i = 0; i < WORLD_X; i++) {
    temp1 = auxPtr + i * WORLD_Y;
    Map[i] = (short *)temp1;
  }

  popPtr = NewPtr(HWLDX * HWLDY);
  trfPtr = NewPtr(HWLDX * HWLDY);
  polPtr = NewPtr(HWLDX * HWLDY);
  landPtr = NewPtr(HWLDX * HWLDY);
  crimePtr = NewPtr(HWLDX * HWLDY);

  tem1Base = NewPtr(HWLDX * HWLDY);
  tem2Base = NewPtr(HWLDX * HWLDY);

  auxPopPtr = popPtr;
  auxTrfPtr = trfPtr;
  auxPolPtr = polPtr;
  auxLandPtr = landPtr;
  auxCrimePtr = crimePtr;

  for (i = 0; i < HWLDX; i++) {
    PopDensity[i] = (Byte *) auxPopPtr + (i * HWLDY);
    TrfDensity[i] = (Byte *) auxTrfPtr + (i * HWLDY);
    PollutionMem[i] = (Byte *) auxPolPtr + (i * HWLDY);
    LandValueMem[i] = (Byte *) auxLandPtr + (i * HWLDY);
    CrimeMem[i] = (Byte *) auxCrimePtr + (i * HWLDY);

    tem[i] = (Byte *)tem1Base + (i * HWLDY);
    tem2[i] = (Byte *) tem2Base + (i * HWLDY);
  }

  brettPtr = (Ptr) &PopDensity[0][0];

  terrainBase = NewPtr(QWX * QWY);
  qTemBase = NewPtr(QWX * QWY);

  for (i = 0; i < QWX; i++) {
    TerrainMem[i] = (Byte *)terrainBase + (i * QWY);
    Qtem[i] = (Byte *)qTemBase + (i * QWY);
  }

  ResHis = (short *)NewPtr(HISTLEN);
  ComHis = (short *)NewPtr(HISTLEN);
  IndHis = (short *)NewPtr(HISTLEN);
  MoneyHis = (short *)NewPtr(HISTLEN);
  PollutionHis = (short *)NewPtr(HISTLEN);
  CrimeHis = (short *)NewPtr(HISTLEN);
  MiscHis = (short *)NewPtr(MISCHISTLEN);
  PowerMap = (short *)NewPtr(POWERMAPLEN);		/* power alloc */
}
