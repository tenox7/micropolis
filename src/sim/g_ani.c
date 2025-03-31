#include "sim.h"
#include "animtab.h"


/* comefrom: moveWorld doEditWindow scoreDoer doMapInFront graphDoer */
animateTiles(void)
{
  unsigned short tilevalue, tileflags;
  unsigned short *tMapPtr;
  int i;

  /* Animate whole world */
  tMapPtr = (unsigned short *)&(Map[0][0]);

  for (i = WORLD_X * WORLD_Y; i > 0; i--) {
    tilevalue = (*tMapPtr);
    if (tilevalue & ANIMBIT) {
      tileflags = tilevalue & ALLBITS;
      tilevalue &= LOMASK;
#if 0
      do {
	tilevalue = aniTile[tilevalue];
      } while (!(tileSynch & aniSynch[tilevalue]));
#else
      tilevalue = aniTile[tilevalue];
#endif

      tilevalue |= tileflags;
      (*tMapPtr) = tilevalue;
    }
    tMapPtr++;
  }

#if 0
  tileSynch = (tileSynch == 0x80) ? 0x01 : (tileSynch << 1);
#endif
}
