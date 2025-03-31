#include "sim.h"


#define VAL_NONE	0
#define VAL_LOW		1
#define VAL_MEDIUM	2
#define VAL_HIGH	3
#define VAL_VERYHIGH	4
#define VAL_PLUS	5
#define VAL_VERYPLUS	6
#define VAL_MINUS	7
#define VAL_VERYMINUS	8

short valMap[] = {
  -1, COLOR_LIGHTGRAY, COLOR_YELLOW, COLOR_ORANGE, COLOR_RED,
  COLOR_DARKGREEN, COLOR_LIGHTGREEN, COLOR_ORANGE, COLOR_YELLOW
};

short valGrayMap[] = {
  -1, 31, 127, 191, 255,
  223, 255, 31, 0
};


int (*mapProcs[NMAPS])();

int drawAll(SimView *view);
int drawRes(SimView *view);
int drawCom(SimView *view);
int drawInd(SimView *view);
int drawPower(SimView *view);
int drawLilTransMap(SimView *view);
int drawPopDensity(SimView *view);
int drawRateOfGrowth(SimView *view);
int drawTrafMap(SimView *view);
int drawPolMap(SimView *view);
int drawCrimeMap(SimView *view);
int drawLandMap(SimView *view);
int drawFireRadius(SimView *view);
int drawPoliceRadius(SimView *view);
int drawDynamic(SimView *view);


short GetCI(short x)
{
  if (x < 50)	return(VAL_NONE);
  if (x < 100)	return(VAL_LOW);
  if (x < 150)	return(VAL_MEDIUM);
  if (x < 200)	return(VAL_HIGH);
  return(VAL_VERYHIGH);
}


drawPopDensity(SimView *view)
{
  short x, y;

  drawAll(view);
  for (x = 0; x < HWLDX; x++) {
    for (y = 0; y < HWLDY; y++) {
      maybeDrawRect(view, GetCI(PopDensity[x][y]),
		    x * 6, y * 6, 6, 6);
    }
  }
}


drawRateOfGrowth(SimView *view)
{
  short x, y;

  drawAll(view);
  for (x = 0; x < SmX; x++) {
    for (y = 0; y < SmY; y++) {
      short val, z = RateOGMem[x][y];

      if (z > 100) val = VAL_VERYPLUS;
      else {
	if (z > 20)  val = VAL_PLUS;
	else {
	  if (z < -100) val = VAL_VERYMINUS;
	  else {
	    if (z < -20) val = VAL_MINUS;
	    else val = VAL_NONE;
	  }
	}
      }
      maybeDrawRect(view, val,
		    x * 24, y * 24, 24, 24);
    }
  }
}


drawTrafMap(SimView *view)
{
  short x;
  short y;

  drawLilTransMap(view);

  for (x = 0; x < HWLDX; x++) {
    for (y = 0; y < HWLDY; y++) {
      maybeDrawRect(view, GetCI(TrfDensity[x][y]),
		    x * 6, y * 6, 6, 6);
    }
  }
}


drawPolMap(SimView *view)
{
  short x, y;

  drawAll(view);

  for (x = 0; x < HWLDX; x++) {
    for (y = 0; y < HWLDY; y++) {
      maybeDrawRect(view, GetCI(10 + PollutionMem[x][y]),
		    x * 6, y * 6, 6, 6);
    }
  }
}


drawCrimeMap(SimView *view)
{
  short x, y;

  drawAll(view);

  for (x = 0; x < HWLDX; x++) {
    for (y = 0; y < HWLDY; y++) {
      maybeDrawRect(view, GetCI(CrimeMem[x][y]),
		    x * 6, y * 6, 6, 6);
    }
  }
}


drawLandMap(SimView *view)
{
  short x, y;

  drawAll(view);

  for (x = 0; x < HWLDX; x++) {
    for (y = 0; y < HWLDY; y++) {
      maybeDrawRect(view, GetCI(LandValueMem[x][y]),
		    x * 6, y * 6, 6, 6);
    }
  }
}


drawFireRadius(SimView *view)
{
  short x, y;

  drawAll(view);
  for (x = 0; x < SmY; x++) {
    for (y = 0; y < SmY; y++) {
      maybeDrawRect(view, GetCI(FireRate[x][y]),
		    x * 24, y * 24, 24, 24);
    }
  }
}


drawPoliceRadius(SimView *view)
{
  short x, y;

  drawAll(view);
  for (x = 0; x < SmX; x++) {
    for (y = 0; y < SmY; y++) {
      maybeDrawRect(view, GetCI(PoliceMapEffect[x][y]),
		    x * 24, y * 24, 24, 24);
    }
  }
}


setUpMapProcs(void)
{
  mapProcs[ALMAP] = drawAll;
  mapProcs[REMAP] = drawRes;
  mapProcs[COMAP] = drawCom;
  mapProcs[INMAP] = drawInd;
  mapProcs[PRMAP] = drawPower;
  mapProcs[RDMAP] = drawLilTransMap;
  mapProcs[PDMAP] = drawPopDensity;
  mapProcs[RGMAP] = drawRateOfGrowth;
  mapProcs[TDMAP] = drawTrafMap;
  mapProcs[PLMAP] = drawPolMap;
  mapProcs[CRMAP] = drawCrimeMap;
  mapProcs[LVMAP] = drawLandMap;
  mapProcs[FIMAP] = drawFireRadius;
  mapProcs[POMAP] = drawPoliceRadius;
  mapProcs[DYMAP] = drawDynamic;
}


MemDrawMap(SimView *view)
{
  (*mapProcs[view->map_state])(view);
  if (!view->x->color) {
    ditherMap(view);
    XSetForeground(view->x->dpy, view->x->gc, view->pixels[COLOR_BLACK]);
    XSetBackground(view->x->dpy, view->x->gc, view->pixels[COLOR_WHITE]);
    XPutImage(view->x->dpy, view->pixmap, view->x->gc, view->image,
	      0, 0, 0, 0, view->m_width, view->m_height);
  }
}


ditherMap(SimView *view)
{
  int i, x, y, width, height;
  int err, pixel1, pixel8;
  int line_bytes1 = view->line_bytes;
  int line_bytes8 = view->line_bytes8;
  unsigned char *image1 = view->data;
  unsigned char *image8 = view->data8;
  int *errors;

  width = view->m_width; height = view->m_height;

  errors = (int *)malloc(sizeof(int) * (width));

  for (i = 0; i < width; i++)
    errors[i] = (Rand16() & 15) - 7;

  err = (Rand16() & 15) - 7;

  for (y = 0; y < height; y += 2) {
    unsigned char *i1 = image1;
    unsigned char *i8 = image8;

    image1 += line_bytes1;
    image8 += line_bytes8;

    for (x = 0; x < width; x += 8) {
      pixel1 = 0;
      for (i = 0; i < 8; i++) {
	pixel1 <<= 1;
	pixel8 = *(i8++) + err + errors[x + i];
	if (pixel8 > 127) {
	  err = pixel8 - 255;
	} else {
	  pixel1 |= 1;
	  err = pixel8;
	}
	errors[x + i] = err/2;
	err = err/2;
      }
      *(i1++) = pixel1;
    }

    i1 = image1 + (width / 8) - 1;
    i8 = image8 + width - 1;

    image1 += line_bytes1;
    image8 += line_bytes8;

    for (x = width - 8; x >= 0; x -= 8) {
      pixel1 = 0;
      for (i = 7; i >= 0; i--) {
	pixel1 >>= 1;
	pixel8 = *(i8--) + err + errors[x + i];
	if (pixel8 > 127) {
	  err = pixel8 - 255;
	} else {
	  pixel1 |= 128;
	  err = pixel8;
	}
	errors[x + i] = err/2;
	err = err/2;
      }
      *(i1--) = pixel1;
    }
  }

  free(errors);
}


maybeDrawRect(SimView *view, int val,
	    int x, int y, int w, int h)
{
  if (val == VAL_NONE) return;

  if (view->x->color) {
    drawRect(view, view->pixels[valMap[val]], 0, x, y, w, h);
  } else {
    drawRect(view, valGrayMap[val], 1, x, y, w, h);
  }
}


drawRect(SimView *view, int pixel, int solid,
	 int x, int y, int w, int h)
{
  int W = view->m_width, H = view->m_height;

  if (x < 0) {
    if ((w += x) < 0) w = 0;
    x = 0;
  } else if (x > W) {
    x = 0; w = 0;
  }
  if (x + w > W) {
    w = W - x;
  }
  if (y < 0) {
    if ((h += y) < 0) h = 0;
    y = 0;
  } else if (y > H) {
    y = 0; h = 0;
  }
  if (y + h > H) {
    h = H - y;
  }

  if (w && h) {
    int i, j, stipple = (x ^ y) & 1;
    unsigned char *data =
      view->x->color ? view->data : view->data8;

    /* In the case of black and white, we use an 8 bit buffer and dither it. */
    int pixelBytes =
      view->x->color ? view->pixel_bytes : 1;
    QUAD line =
      view->x->color ? view->line_bytes : view->line_bytes8;

    unsigned char *image =
      &(data[(line * y) + (x * pixelBytes)]);

    switch (pixelBytes) {

    case 1:
      {
	unsigned char *data =
	  view->data8;
	unsigned char *image =
	  &data[(line * y) + (x * pixelBytes)];

	if (solid) {
	  for (i = h; i > 0; i--) {
	    for (j = w; j > 0; j--) {
	      *image = pixel;
	      image++;
	    }
	    image += line - w;
	  }
	} else {
	  for (i = h; i > 0; i--) {
	    for (j = w; j > 0; j--) {
	      if (stipple++ & 1)
		*image = pixel;
	      image++;
	    }
	    if (!(w & 1))
	      stipple++;
	    image += line - w;
	  }
	}
      }
      break;

    case 2:
      {
	unsigned short *data =
	  (unsigned short *)view->data;
	unsigned short *image;
	line >>= 1; /* Convert from byte offset to short offset */
	image =
	  &data[(line * y) + x];

	if (solid) {
	  for (i = h; i > 0; i--) {
	    for (j = w; j > 0; j--) {
	      *image = pixel;
	      image++;
	    }
	    image += line - w;
	  }
	} else {
	  for (i = h; i > 0; i--) {
	    for (j = w; j > 0; j--) {
	      if (stipple++ & 1)
		*image = pixel;
	      image++;
	    }
	    if (!(w & 1))
	      stipple++;
	    image += line - w;
	  }
	}
      }
      break;

    case 3:
    case 4:
      {
	unsigned char *data =
	  (unsigned char *)view->data;
	unsigned char *image;
	int bitmapPad = view->x->small_tile_image->bitmap_pad;
	int rowBytes = view->x->small_tile_image->bytes_per_line;
	line = rowBytes >> 1; /* Convert from byte offset to short offset */
	image =
	  &data[(line * y) + x];

	if (solid) {
	  for (i = h; i > 0; i--) {
	    for (j = w; j > 0; j--) {
	      *(image++) = (pixel >> 0) & 0xff;
	      *(image++) = (pixel >> 8) & 0xff;
	      *(image++) = (pixel >> 16) & 0xff;
	      if (bitmapPad == 32) {
	        image++;
	      }
	    }
	    image += line - w;
	  }
	} else {
	  for (i = h; i > 0; i--) {
	    for (j = w; j > 0; j--) {
	      if (stipple++ & 1) {
		*(image++) = (pixel >> 0) & 0xff;
		*(image++) = (pixel >> 8) & 0xff;
		*(image++) = (pixel >> 16) & 0xff;
	        if (bitmapPad == 32) {
		  image++;
		}
	      }
	    }
	    if (!(w & 1)) {
	      stipple++;
	    }
	    image += line - w;
	  }
	}
      }
      break;

    default:
      assert(0); /* Undefined depth */
      break;
    }

  }
}



