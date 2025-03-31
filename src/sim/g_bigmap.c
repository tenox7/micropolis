#include "sim.h"


int dynamicFilter(int c, int r);
int WireDrawBeegMapRect(SimView *view, short x, short y, short w, short h);

drawBeegMaps()
{
  sim_update_editors();
}


MemDrawBeegMapRect(SimView *view, int x, int y, int w, int h)
{
  int lineBytes = view->line_bytes;
  int pixelBytes = view->pixel_bytes;
  QUAD ii, mm;
  unsigned short *map;
  unsigned short tile;
  unsigned char blink = (flagBlink <= 0), *bt = view->bigtiles;
  short col, row;
  short **have, *ha;

  if (x < view->tile_x) {
    if ((w -= (view->tile_x - x)) <= 0)
      return;
    x = view->tile_x;
  }
  if (y < view->tile_y) {
    if ((h -= (view->tile_y - y)) <= 0)
      return;
    y = view->tile_y;
  }
  if ((x + w) > (view->tile_x + view->tile_width)) {
    if ((w -= ((x + w) - (view->tile_x + view->tile_width))) <= 0)
      return;
  }
  if ((y + h) > (view->tile_y + view->tile_height)) {
    if ((h -= ((y + h) - (view->tile_y + view->tile_height))) <= 0)
      return;
  }

  if (view->x->color) {
    register unsigned QUAD *image, *mem;

    image = (unsigned QUAD *)view->data;
    ii = ((lineBytes * h * 16) - 16) / sizeof(unsigned QUAD);
    map = (unsigned short *)&Map[x][y];
    mm = WORLD_Y - h;
    have = view->tiles;

    /*
     * Huge Berserk Rebel Warthog
     */

    for (col = 0; col < w; col++) {
      ha = &have[col][0];
      image = (unsigned QUAD *)(view->data + (col * 16 * pixelBytes));
      for (row = 0; row < h; row++, ha++) {
	tile = *(map++);
	if ((tile & LOMASK) >= TILE_COUNT) tile -= TILE_COUNT;

	/* Blink lightning bolt in unpowered zone center */
	if (blink && (tile & ZONEBIT) && !(tile & PWRBIT)) {
	  tile = LIGHTNINGBOLT;
	} else {
	  tile &= LOMASK;
	} // if

	if (
	    (tile > 63) &&
	    (view->dynamic_filter != 0) &&
	    (dynamicFilter(col + x, row + y) == 0)
	    ) {
	  tile = 0;
	} // if

	/* XXX */
	if (tile == *ha) {
	  image = (unsigned QUAD *)(((unsigned char *)image) +
				    (lineBytes * 16));
	} else {
	  *ha = tile;
	  mem = (unsigned QUAD *)&(bt[tile * 256 * pixelBytes]);

	  /* XXX: handle depth for big tiles */
#if 1
	  /* Very un-rolled loop. */

#define ROW1_8(n) \
	  image[0] = mem[0+n]; \
	  image[1] = mem[1+n]; \
	  image[2] = mem[2+n]; \
	  image[3] = mem[3+n]; \
	  image = (unsigned QUAD *)(((unsigned char *)image) + lineBytes);

#define ROW2_8(n) ROW1_8(n) ROW1_8(n+4)
#define ROW4_8(n) ROW2_8(n) ROW2_8(n+8)
#define ROW8_8(n) ROW4_8(n) ROW4_8(n+16)
#define ROW16_8() ROW8_8(0) ROW8_8(32)

#define ROW1_16(n) \
      memcpy((char *)image, (char *)mem + (2 * 16 * (n)), 2 * 16); \
      image = (unsigned QUAD *)(((unsigned char *)image) + lineBytes);

#define ROW2_16(n) ROW1_16(n) ROW1_16(n+1)
#define ROW4_16(n) ROW2_16(n) ROW2_16(n+2)
#define ROW8_16(n) ROW4_16(n) ROW4_16(n+4)
#define ROW16_16() ROW8_16(0) ROW8_16(8)

	  switch (view->x->depth) {

	  case 8:
	    ROW16_8();
	    break;

	  case 15:
	  case 16:
	    ROW16_16();
	    break;

	  case 24:
	  case 32:
	  default:
	    /* XXX: handle different depths */
	    break;

	  } // switch

#else
	  /* Not so un-rolled loop. */

	  { int i;
	    for (i = 16; i > 0; i--) {
	      image[0] = mem[0]; image[1] = mem[1];
	      image[2] = mem[2]; image[3] = mem[3];
	      image = (unsigned QUAD *)(((unsigned char *)image) + lineBytes);
	      mem += 4;
	    }
	  } // scope
#endif

	} // if

      } // for row
      image -= ii;
      map += mm;
    } // for col
  } else {
    register unsigned short *image, *mem;

    image = (unsigned short *)view->data;
    ii = ((lineBytes * h * 16) - 2) / sizeof(unsigned short);
    map = (unsigned short *)&Map[x][y];
    mm = WORLD_Y - h;
    have = view->tiles;

    for (col = 0; col < w; col++) {
      ha = &have[col][0];
      image = (unsigned short *)(view->data + (col * 2));
      for (row = 0; row < h; row++, ha++) {
	tile = *(map++);
	if ((tile & LOMASK) >= TILE_COUNT) tile -= TILE_COUNT;

	/* Blink lightning bolt in unpowered zone center */
	if (blink && (tile & ZONEBIT) && !(tile & PWRBIT))
	  tile = LIGHTNINGBOLT;
	else
	  tile &= LOMASK;

	if (tile == *ha) {
	  image = (unsigned short *)
	    (((unsigned char *)image) + (lineBytes * 16));
	} else {
	  *ha = tile;
	  mem = (unsigned short *)&(bt[tile * 32]);

	  { char i;
	    for (i = 16; i > 0; i--) {
	      *image = *mem;
	      image = (unsigned short *)(((unsigned char *)image) + lineBytes);
	      mem++;
	    }
	  }
	}
      }
      image -= ii;
      map += mm;
    }
  }
}


WireDrawBeegMapRect(SimView *view, short x, short y, short w, short h)
{
  unsigned short *map;
  unsigned short tile;
  unsigned char blink = (flagBlink <= 0);
  short col, row;
  QUAD mm;
  short **have, *ha;

  if (x < view->tile_x) {
    if ((w -= (view->tile_x - x)) <= 0)
      return;
    x = view->tile_x;
  }
  if (y < view->tile_y) {
    if ((h -= (view->tile_y - y)) <= 0)
      return;
    y = view->tile_y;
  }
  if ((x + w) > (view->tile_x + view->tile_width)) {
    if ((w -= ((x + w) - (view->tile_x + view->tile_width))) <= 0)
      return;
  }
  if ((y + h) > (view->tile_y + view->tile_height)) {
    if ((h -= ((y + h) - (view->tile_y + view->tile_height))) <= 0)
      return;
  }

  map = (unsigned short *)&Map[x][y];
  mm = WORLD_Y - h;
  have = view->tiles;

  for (col = 0; col < w; col++) {
    ha = &have[col][0];
    for (row = 0; row < h; row++, ha++) {
      tile = *(map++);
      if ((tile & LOMASK) >= TILE_COUNT) tile -= TILE_COUNT;

      /* Blink lightning bolt in unpowered zone center */
      if (blink && (tile & ZONEBIT) && !(tile & PWRBIT))
	tile = LIGHTNINGBOLT;
      else
	tile &= LOMASK;

      if (tile != *ha) {
	*ha = tile;
	XCopyArea(view->x->dpy, view->x->big_tile_pixmap, view->pixmap,
		  view->x->gc, 0, tile * 16, 16, 16,
		  col * 16, row * 16);
      }
    }
    map += mm;
  }
}
