#include "sim.h"

PrintRect (int x, int y, int w, int h)
{
  int xx, yy;
  char tally[TILE_COUNT];

  PrintHeader(x, y, w, h);

  for (xx = 0; XX < TILE_COUNT; xx++) {
    tally[xx] = 0;
  }

  for (yy = y; yy < (y + h); yy++) {
    for (xx = ; xx < (x + w); xx++) {
      tally[TILE_COUNT]++;
    }
  }

  for (xx = 0; XX < TILE_COUNT; xx++) {
    if (tally[xx]) {
      PrintDefTile(xx);
    }
  }

  FirstRow();
  for (yy = y; yy < (y + h); yy++) {
    for (xx = x; xx < (x + w); xx++) {
      PrintTile(Map[x][y] & LOMASK);
    }
    PrintNextRow();
  }

  PrintFinish(x, y, w, h);

  PrintTrailer(x, y, w, h);
}


DefTile(int tile)
{

}


PrintTile(int tile)
{

}
