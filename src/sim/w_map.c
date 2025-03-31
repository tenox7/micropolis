#include "sim.h"


Tcl_HashTable MapCmds;


extern Tk_ConfigSpec TileViewConfigSpecs[];


Ink *NewInk();


MapCmdconfigure(VIEW_ARGS)
{
  int result = TCL_OK;

  if (argc == 2) {
    result = Tk_ConfigureInfo(interp, view->tkwin, TileViewConfigSpecs,
			      (char *) view, (char *) NULL, 0);
  } else if (argc == 3) {
    result = Tk_ConfigureInfo(interp, view->tkwin, TileViewConfigSpecs,
			      (char *) view, argv[2], 0);
  } else {
    result = ConfigureTileView(interp, view, argc-2, argv+2,
			       TK_CONFIG_ARGV_ONLY);
  }
  return result;
}

MapCmdposition(VIEW_ARGS)
{
  if ((argc != 2) && (argc != 4)) {
    return TCL_ERROR;
  }
  if (argc == 4) {
    if ((Tcl_GetInt(interp, argv[2], &view->w_x) != TCL_OK)
	|| (Tcl_GetInt(interp, argv[3], &view->w_y) != TCL_OK)) {
      return TCL_ERROR;
    }
  }
  sprintf(interp->result, "%d %d", view->w_x, view->w_y);
  return TCL_OK;
}

MapCmdsize(VIEW_ARGS)
{
  if ((argc != 2) && (argc != 4)) {
    return TCL_ERROR;
  }
  if (argc == 4) {
    int w, h;

    if (Tcl_GetInt(interp, argv[2], &w) != TCL_OK) {
      return TCL_ERROR;
    }
    if (Tcl_GetInt(interp, argv[3], &h) != TCL_OK) {
      return TCL_ERROR;
    }
    view->w_width = w;
    view->w_height = h;
  }
  sprintf(interp->result, "%d %d", view->w_width, view->w_height);
  return TCL_OK;
}

MapCmdMapState(VIEW_ARGS)
{
  int state;

  if ((argc != 2) && (argc != 3)) {
    return TCL_ERROR;
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &state) != TCL_OK) ||
	(state < 0) || (state >= NMAPS)) {
      return TCL_ERROR;
    }

    DoSetMapState(view, state); Kick();
  }

  sprintf(interp->result, "%d", view->map_state);
  return TCL_OK;
}

MapCmdShowEditors(VIEW_ARGS)
{
  int val;

  if ((argc != 2) && (argc != 3)) {
    return TCL_ERROR;
  }

  if (argc == 3) {
    if (Tcl_GetInt(interp, argv[2], &val) != TCL_OK) {
      return TCL_ERROR;
    }

    view->show_editors = val;
  }

  sprintf(interp->result, "%d", view->show_editors);
  return TCL_OK;
}

MapCmdPanStart(VIEW_ARGS)
{
  int x, y, left, right, top, bottom, width, height;
  SimView *ed;

  if (argc != 4) {
    return TCL_ERROR;
  }

  if ((Tcl_GetInt(interp, argv[2], &x) != TCL_OK) ||
      (Tcl_GetInt(interp, argv[3], &y) != TCL_OK)) {
    return TCL_ERROR;
  }

  for (ed = sim->editor; ed != NULL; ed = ed->next) {
    if ((ed->x != view->x) || (ed->show_me == 0))
      continue;

    width = ed->w_width;
    height = ed->w_height;
    left = ed->pan_x - (width / 2);
    top = ed->pan_y - (height / 2);
    right = left + width;
    bottom = top + height;

    left = left * 3 / 16 - 4;
    top = top * 3 / 16 - 4;
    right = right * 3 / 16 + 4;
    bottom = bottom * 3 / 16 + 4;

    if ((x >= left) && (x <= right) &&
	(y >= top) && (y <= bottom)) {
      goto gotit;
    }
  }

gotit:
  view->last_x = x;
  view->last_y = y;
  view->track_info = (char *)ed;
  return TCL_OK;
}

MapCmdPanTo(VIEW_ARGS)
{
  int x, y, dx, dy;
  SimView *ed;

  if (argc != 4) {
    return TCL_ERROR;
  }

  if ((Tcl_GetInt(interp, argv[2], &x) != TCL_OK) ||
      (Tcl_GetInt(interp, argv[3], &y) != TCL_OK)) {
    return TCL_ERROR;
  }

  if ((ed = (SimView *)view->track_info) != NULL) {
    dx = x - view->last_x;
    dy = y - view->last_y;
    if (dx || dy) {
      view->last_x = x;
      view->last_y = y;
      dx = dx * 16 / 3;
      dy = dy * 16 / 3;

      ed->skip = 0;
      DoPanBy(ed, dx, dy); Kick();
    }
  }
  return TCL_OK;
}

MapCmdVisible(VIEW_ARGS)
{
  int visible;

  if ((argc != 2) && (argc != 3)) {
    return TCL_ERROR;
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &visible) != TCL_OK) ||
	(visible < 0) || (visible > 1)) {
      return TCL_ERROR;
    }

    visible = visible && Tk_IsMapped(view->tkwin);
    view->visible = visible;
  }

  sprintf(interp->result, "%d", view->visible);
  return TCL_OK;
}

MapCmdViewAt(VIEW_ARGS)
{
  int x, y;

  if (argc != 4) {
    return TCL_ERROR;
  }

  if ((Tcl_GetInt(interp, argv[2], &x) != TCL_OK) ||
      (x < 0) || (x >= WORLD_X) ||
      (Tcl_GetInt(interp, argv[3], &y) != TCL_OK) ||
      (y < 0) || (y >= WORLD_Y)) {
    return TCL_ERROR;
  }

  sprintf(interp->result, "Sorry Not Implemented Yet"); /* XXX */
  return TCL_OK;
}


map_command_init()
{
  int new;
  extern int TileViewCmd(CLIENT_ARGS);

  Tcl_CreateCommand(tk_mainInterp, "mapview", TileViewCmd,
		    (ClientData)MainWindow, (void (*)()) NULL);

  Tcl_InitHashTable(&MapCmds, TCL_STRING_KEYS);

#define MAP_CMD(name) HASHED_CMD(Map, name)

  MAP_CMD(configure);
  MAP_CMD(position);
  MAP_CMD(size);
  MAP_CMD(MapState);
  MAP_CMD(ShowEditors);
  MAP_CMD(PanStart);
  MAP_CMD(PanTo);
  MAP_CMD(Visible);
  MAP_CMD(ViewAt);
}


int
DoMapCmd(CLIENT_ARGS)
{
  SimView *view = (SimView *) clientData;
  Tcl_HashEntry *ent;
  int result = TCL_OK;
  int (*cmd)();

  if (argc < 2) {
    return TCL_ERROR;
  }

  if (ent = Tcl_FindHashEntry(&MapCmds, argv[1])) {
    cmd = (int (*)())ent->clientData;
    Tk_Preserve((ClientData) view);
    result = cmd(view, interp, argc, argv);
    Tk_Release((ClientData) view);
  } else {
    Tcl_AppendResult(interp, "unknown command name: \"",
		     argv[0], " ", argv[1], "\".", (char *) NULL);
    result = TCL_ERROR;
  }
  return result;
}


/*************************************************************************/

DoNewMap(SimView *view)
{
  sim->maps++; view->next = sim->map; sim->map = view;
/* NewMap = 1; */
  view->invalid = 1;
}


int DoUpdateMap(SimView *view)
{
  int dx, dy, i;

  view->updates++;

// fprintf(stderr, "UpdateMaps sim_skips %d skips %d skip %d visible %d\n", sim_skips, view->skips, view->skip, view->visible);

  if (!view->visible) {
    return 0;
  }

  if ((!ShakeNow) &&
      (!view->update) &&
      (sim_skips ||
       view->skips)) {
    if (sim_skips) {
      if (sim_skip > 0) {
	return 0;
      }
    } else {
      if (view->skip > 0) {
	--view->skip;
	return 0;
      } else {
	view->skip = view->skips;
      }
    }
  }

  view->update = 0;
  view->skip = 0;

//  view->invalid = 1;

  if (view->invalid || NewMap || ShakeNow) {

    view->invalid = 0;

    switch (view->type) {

    case X_Mem_View:
      MemDrawMap(view);
      break;

    case X_Wire_View:
      WireDrawMap(view);
      break;
    }

  }

  /* XXX: don't do this stuff if just redrawing overlay */

  for (dx = dy = i = 0; i < ShakeNow; i++) {
    dx += Rand(16) - 8;
    dy += Rand(16) - 8;
  }

  XCopyArea(view->x->dpy, view->pixmap, view->pixmap2, view->x->gc,
	    dx, dy, view->w_width, view->w_height, 0, 0);
  DrawMapInk(view);

  /* XXX: do this if just redrawing overlay */

  XCopyArea(view->x->dpy, view->pixmap2,
	    Tk_WindowId(view->tkwin), view->x->gc,
	    0, 0, view->w_width, view->w_height, 0, 0);

  if (view->show_editors) {
    DrawMapEditorViews(view);
  }

  return 1;
}


DrawMapEditorViews(SimView *view)
{
  Pixmap pm = Tk_WindowId(view->tkwin);
  struct SimView *ed;
  int left, right, top, bottom, width, height;
  int mine;

  XSetLineAttributes(view->x->dpy, view->x->gc, 1,
		     LineSolid, CapButt, JoinBevel);

  for (ed = sim->editor; ed != NULL; ed = ed->next) {
    if ((ed->x != view->x) || (ed->show_me == 0))
      continue;

    width = ed->w_width;
    height = ed->w_height;
    left = ed->pan_x - (width / 2);
    top = ed->pan_y - (height / 2);
    right = left + width;
    bottom = top + height;

    left = left * 3 / 16;
    top = top * 3 / 16;
    right = right * 3 / 16;
    bottom = bottom * 3 / 16;
    width = right - left;
    height = bottom - top;

    XSetForeground(view->x->dpy, view->x->gc,
		   view->pixels[COLOR_WHITE]);
    XDrawRectangle(view->x->dpy, pm, view->x->gc,
		   left - 3, top - 3, width + 3, height + 3);

    XSetForeground(view->x->dpy, view->x->gc,
		   view->pixels[COLOR_BLACK]);
    XDrawRectangle(view->x->dpy, pm, view->x->gc,
		   left - 1, top - 1, width + 3, height + 3);

    XSetForeground(view->x->dpy, view->x->gc,
		   view->pixels[COLOR_YELLOW]);
    XDrawRectangle(view->x->dpy, pm, view->x->gc,
		   left - 2, top - 2, width + 3, height + 3);
  }
}


/*
 * Sending the whole image is 108108 bytes.
 * Sending points is 4.4 bytes per point.
 * One image is as big as 24570 points.
 * But we have to sort these dang things.
 */

#define MAX_PIX 256
int max_pix = MAX_PIX;

struct Pix {
  long color;
  short x, y;
};

struct Pix pix[MAX_PIX];


CompareColor(struct Pix *p1, struct Pix *p2)
{
  register char c1 = p1->color, c2 = p2->color;

  if (c1 == c2)
    return (0);
  return ((c1 < c2) ? -1 : 1);
}


WireDrawMap(SimView *view)
{
  int different, x, y, i, last, pts;
  unsigned char *old, *new;
  XPoint *points;

  if (!view->x->color) {
    MemDrawMap(view);
    return;
  }

  memcpy(view->other_data, view->data, view->line_bytes * view->m_height); /* XXX: handle depth */
  MemDrawMap(view);

  old = view->other_data; new = view->data; /* XXX: handle depth */
  different = 0;

  /* Scan the pixels that have changed */
  for (y = 0; y < view->m_height; y++) {
    for (x = 0; x < view->m_width; x++) {
      if (old[x] != new[x]) {
	if (different >= max_pix) {
	  /* Wow, lots of the pixels have changed.
	     Maybe we ought to just do it the hard way. */
	  XPutImage(view->x->dpy, view->pixmap, view->x->gc, view->image,
		    0, 0, 0, 0, view->m_width, view->m_height);
	  return;
	}
	pix[different].color = new[x];
	pix[different].x = x;
	pix[different].y = y;
	different++;
      }
    }
    old += view->line_bytes; new += view->line_bytes; /* XXX: handle depth */
  }

  /* Whew, the images are identical! */
  if (different == 0)
    return;

#if 1

  /* Always draw the whole pixmap, for now. */
  XPutImage(view->x->dpy, view->pixmap, view->x->gc, view->image,
	    0, 0, 0, 0, view->m_width, view->m_height);

#else

  /* TODO: Fix this. I disabled this incremental drawing code for now since it seems to be buggy. */

  /* Sort the changed pixels by their color */
  qsort(pix, different, sizeof (struct Pix), (int (*)())CompareColor);

  /* Draw the points of each color that have changed */
  points = (XPoint *)malloc(sizeof (XPoint) * different);
  last = 0; pts = 0;
  for (i = 0; i <= different; i++) {
    if ((i == different) ||
	(pix[i].color != pix[last].color)) {
      XSetForeground(view->x->dpy, view->x->gc, pix[last].color);
      XDrawPoints(view->x->dpy, view->pixmap, view->x->gc,
		  points, pts, CoordModeOrigin);
      if (i == different)
	break;
      pts = 0;
      last = i;
    }
    points[pts].x = pix[i].x;
    points[pts].y = pix[i].y;
    pts++;
  }
  free(points);

#endif

}


DrawMapInk(SimView *view)
{
  Pixmap pm = view->pixmap2;
  SimView *v;
  Ink *ink, *ink2 = NewInk();
  int i, X, Y, x, y;

  XSetLineAttributes(view->x->dpy, view->x->gc, 0,
		     LineSolid, CapButt, JoinBevel);

  for (ink = sim->overlay; ink != NULL; ink = ink->next) {
    X = ink->x; x = (X * 3) >>4;
    Y = ink->y; y = (Y * 3) >>4;

    if (ink->length <= 1) {
      XSetForeground(view->x->dpy, view->x->gc,
		     view->pixels[ink->color]);
      XFillArc(view->x->dpy, pm, view->x->gc,
	       ink->x - 1, ink->y - 1, 1, 1, 0, 360 * 64);
    } else {
      StartInk(ink2, x, y);

      for (i = 1; i < ink->length; i++) {
	X += ink->points[i].x; x = (X * 3) >>4;
	Y += ink->points[i].y; y = (Y * 3) >>4;
	AddInk(ink2, x, y);
      }

      XSetForeground(view->x->dpy, view->x->gc,
		     view->pixels[ink->color]);
      XDrawLines(view->x->dpy, pm, view->x->gc,
		 ink2->points, ink2->length, CoordModePrevious);
    }
  }

  FreeInk(ink2);
}


