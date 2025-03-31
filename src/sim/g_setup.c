#include "sim.h"


#define SIM_SMTILE	385
#define SIM_BWTILE	386
#define SIM_GSMTILE	388
#define SIM_LGTILE	544


#define gray25_width 16
#define gray25_height 16
static unsigned char gray25_bits[] = {
  0x77, 0x77,
  0xdd, 0xdd,
  0x77, 0x77,
  0xdd, 0xdd,
  0x77, 0x77,
  0xdd, 0xdd,
  0x77, 0x77,
  0xdd, 0xdd,
  0x77, 0x77,
  0xdd, 0xdd,
  0x77, 0x77,
  0xdd, 0xdd,
  0x77, 0x77,
  0xdd, 0xdd,
  0x77, 0x77,
  0xdd, 0xdd,
};


#define gray50_width 16
#define gray50_height 16
static unsigned char gray50_bits[] = {
  0x55, 0x55,
  0xaa, 0xaa,
  0x55, 0x55,
  0xaa, 0xaa,
  0x55, 0x55,
  0xaa, 0xaa,
  0x55, 0x55,
  0xaa, 0xaa,
  0x55, 0x55,
  0xaa, 0xaa,
  0x55, 0x55,
  0xaa, 0xaa,
  0x55, 0x55,
  0xaa, 0xaa,
  0x55, 0x55,
  0xaa, 0xaa,
};


#define gray75_width 16
#define gray75_height 16
static unsigned char gray75_bits[] = {
  0x88, 0x88,
  0x22, 0x22,
  0x88, 0x88,
  0x22, 0x22,
  0x88, 0x88,
  0x22, 0x22,
  0x88, 0x88,
  0x22, 0x22,
  0x88, 0x88,
  0x22, 0x22,
  0x88, 0x88,
  0x22, 0x22,
  0x88, 0x88,
  0x22, 0x22,
  0x88, 0x88,
  0x22, 0x22,
};

#define vert_width 16
#define vert_height 16
static unsigned char vert_bits[] = {
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
};


#define horiz_width 16
#define horiz_height 16
static unsigned char horiz_bits[] = {
  0xff, 0xff,
  0x00, 0x00,
  0xff, 0xff,
  0x00, 0x00,
  0xff, 0xff,
  0x00, 0x00,
  0xff, 0xff,
  0x00, 0x00,
  0xff, 0xff,
  0x00, 0x00,
  0xff, 0xff,
  0x00, 0x00,
  0xff, 0xff,
  0x00, 0x00,
  0xff, 0xff,
  0x00, 0x00,
};


#define diag_width 16
#define diag_height 16
static unsigned char diag_bits[] = {
  0x55, 0x55,
  0xee, 0xee,
  0x55, 0x55,
  0xba, 0xbb,
  0x55, 0x55,
  0xee, 0xee,
  0x55, 0x55,
  0xba, 0xbb,
  0x55, 0x55,
  0xee, 0xee,
  0x55, 0x55,
  0xba, 0xbb,
  0x55, 0x55,
  0xee, 0xee,
  0x55, 0x55,
  0xba, 0xbb,
};


Ptr MickGetHexa(short theID)
{
  Handle theData;

  theData = GetResource("hexa", theID);
  return ((Ptr)*theData);
}


Pixmap *
GetObjectXpms(XDisplay *xd, int id, int frames)
{
  int i;
  Pixmap *pixmaps = (Pixmap *)ckalloc(2 * frames * sizeof (Pixmap));
  XpmAttributes attributes;
  char name[256];

  attributes.visual = Tk_DefaultVisual(xd->screen);
  attributes.colormap = Tk_DefaultColormap(xd->screen);
  attributes.depth = Tk_DefaultDepth(xd->screen);
  attributes.valuemask = XpmVisual | XpmColormap | XpmDepth;

  for (i = 0; i < frames; i++) {
    sprintf(name, "%s/images/obj%d-%d.xpm", HomeDir, id, i);
    if (XpmReadFileToPixmap(xd->dpy,
			    RootWindowOfScreen(xd->screen),
			    name,
			    &pixmaps[i + i], &pixmaps[i + i + 1],
			    &attributes) < 0) {
      fprintf(stderr,
	      "Uh oh, Micropolis couldn't read the pixmap file \"%s\".\n",
	      name);
      sim_exit(1); // Just sets tkMustExit and ExitReturn
      return NULL;
    }
  }
  return (pixmaps);
}


GetPixmaps(XDisplay *xd)
{
  if (xd->gray25_stipple == None) {
    xd->gray25_stipple =
      XCreatePixmapFromBitmapData(xd->dpy, xd->root,
				  gray25_bits, gray25_width, gray25_height,
				  1, 0, 1);
    xd->gray50_stipple =
      XCreatePixmapFromBitmapData(xd->dpy, xd->root,
				  gray50_bits, gray50_width, gray50_height,
				  1, 0, 1);
    xd->gray75_stipple =
      XCreatePixmapFromBitmapData(xd->dpy, xd->root,
				  gray75_bits, gray75_width, gray75_height,
				  1, 0, 1);
    xd->vert_stipple =
      XCreatePixmapFromBitmapData(xd->dpy, xd->root,
				  vert_bits, vert_width, vert_height,
				  1, 0, 1);
    xd->horiz_stipple =
      XCreatePixmapFromBitmapData(xd->dpy, xd->root,
				  horiz_bits, horiz_width, horiz_height,
				  1, 0, 1);
    xd->diag_stipple =
      XCreatePixmapFromBitmapData(xd->dpy, xd->root,
				  diag_bits, diag_width, diag_height,
				  1, 0, 1);
  }

  if (xd->objects == NULL) {
    Pixmap **pm;

    xd->objects = pm = (Pixmap **)ckalloc(OBJN * sizeof (Pixmap *));

    pm[0] = NULL; /* no object number 0 */
    pm[TRA] = GetObjectXpms(xd, TRA, 5);
    pm[COP] = GetObjectXpms(xd, COP, 8);
    pm[AIR] = GetObjectXpms(xd, AIR, 11);
    pm[SHI] = GetObjectXpms(xd, SHI, 8);
    pm[GOD] = GetObjectXpms(xd, GOD, 16);
    pm[TOR] = GetObjectXpms(xd, TOR, 3);
    pm[EXP] = GetObjectXpms(xd, EXP, 6);
    pm[BUS] = GetObjectXpms(xd, BUS, 4);
  }
}


GetViewTiles(SimView *view)
{
  char name[256];
  XpmAttributes attributes;

  attributes.visual = Tk_DefaultVisual(view->x->screen);
  attributes.colormap = Tk_DefaultColormap(view->x->screen);
  attributes.depth = Tk_DefaultDepth(view->x->screen);
  attributes.valuemask = XpmVisual | XpmColormap | XpmDepth;

  if (view->class == Editor_Class) {

    sprintf(name, "%s/images/%s", HomeDir,
	    view->x->color ? "tiles.xpm" : "tilesbw.xpm");

    switch (view->type) {

    case X_Mem_View:
      if (view->x->big_tile_image == NULL) {
	if (XpmReadFileToImage(view->x->dpy, name,
			       &view->x->big_tile_image, NULL,
			       &attributes) < 0) {
	  fprintf(stderr,
		  "Uh oh, Micropolis couldn't read the pixmap file \"%s\".\n",
		  name);
	  sim_exit(1); // Just sets tkMustExit and ExitReturn
	  return;
	}
      }
      view->bigtiles = (unsigned char *)view->x->big_tile_image->data;
      break;

    case X_Wire_View:
      if (view->x->big_tile_pixmap == None) {
	if (XpmReadFileToPixmap(view->x->dpy,
				RootWindowOfScreen(view->x->screen),
				name,
				&view->x->big_tile_pixmap, NULL,
				&attributes) < 0) {
	  fprintf(stderr,
		  "Uh oh, Micropolis couldn't read the pixmap file \"%s\".\n",
		  name);
	  sim_exit(1); // Just sets tkMustExit and ExitReturn
	  return;
	}
      }
      break;

    }

  } else if (view->class == Map_Class) {

    if (view->x->small_tile_image == NULL) {
      if (view->x->color) {

	sprintf(name, "%s/images/%s", HomeDir, "tilessm.xpm");
	if (XpmReadFileToImage(view->x->dpy, name,
			       &view->x->small_tile_image, NULL,
			       &attributes) < 0) {
	  fprintf(stderr,
		  "Uh oh, Micropolis couldn't read the pixmap file \"%s\".\n",
		  name);
	  sim_exit(1); // Just sets tkMustExit and ExitReturn
	  return;
	}

      } else {

	view->x->small_tile_image =
	  XCreateImage(view->x->dpy, view->x->visual, 8,
		       ZPixmap,
		       0, (char *)MickGetHexa(SIM_GSMTILE),
		       4, 3 * TILE_COUNT, 8, 4);

      }
    }

    { int x, y, b, tile;
      unsigned char *from, *to;
      int pixelBytes = view->pixel_bytes;
      int rowBytes = view->x->small_tile_image->bytes_per_line;

      if (pixelBytes == 0) {
	/* handle the case of monochrome display (8 bit map) */
	pixelBytes = 1;
      }

      /* from is 4 pixels wide per 3 pixel wide tile */
      from = (unsigned char *)view->x->small_tile_image->data;
      to = (unsigned char *)ckalloc(4 * 4 * TILE_COUNT * pixelBytes);
      view->smalltiles = to;

      switch (pixelBytes) {

      case 1:
	for (tile = 0; tile < TILE_COUNT; tile++) {
	  for (y = 0; y < 3; y++) {
	    for (x = 0; x < 4; x++) {
	      *to++ = *from++;
	    }
	  }
	  for (x = 0; x < 4; x++) {
	    *to++ = 0;
	  }
	}
	break;

      case 2:
	for (tile = 0; tile < TILE_COUNT; tile++) {
	  for (y = 0; y < 3; y++) {
	    for (x = 0; x < 4; x++) {
	      *to++ = *from++;
	      *to++ = *from++;
	    }
	  }
	  for (x = 0; x < 4; x++) {
	    *to++ = 0;
	    *to++ = 0;
	  }
	}
	break;

      case 3:
      case 4:
	for (tile = 0; tile < TILE_COUNT; tile++) {
	  for (y = 0; y < 3; y++) {
	    for (x = 0; x < 4; x++) {
	      *to++ = *from++;
	      *to++ = *from++;
	      *to++ = *from++;
	      if (pixelBytes == 4) {
		*to++ = *from++;
	      }
	    }
	  }
	  for (x = 0; x < 4; x++) {
	    *to++ = 0;
	    *to++ = 0;
	    *to++ = 0;
	    if (pixelBytes == 4) {
	      *to++ = 0;
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
}
