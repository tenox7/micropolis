#include "sim.h"


/* Sim City */

char *MicropolisVersion = "4.0";
Sim *sim = NULL;
int sim_loops = 0;
int sim_delay = 50;
int sim_skips = 0;
int sim_skip = 0;
int sim_paused = 0;
int sim_paused_speed = 3;
int sim_tty = 0;
#ifdef CAM
int sim_just_cam = 0;
#endif
int heat_steps = 0;
int heat_flow = -7;
int heat_rule = 0;
int heat_wrap = 3;
struct timeval start_time, now_time, beat_time, last_now_time;
char *CityFileName = NULL;
int Startup = 0;
int StartupGameLevel = 0;
char *StartupName = NULL;
int WireMode = 0;
int MultiPlayerMode = 0;
int TilesAnimated = 0;
int DoAnimation = 1;
int DoMessages = 1;
int DoNotices = 1;
char *Displays = NULL;
char *FirstDisplay = NULL;
int ExitReturn = 0;


sim_exit(int val)
{
  tkMustExit = 1;
  ExitReturn = val;
}


sim_really_exit(int val)
{
  DoStopMicropolis();

  exit(val);
}


#define COPY(FROM, TO) \
  TO = ckalloc(strlen(FROM) + 1); \
  strcpy(TO, FROM);


#define TESTDIR(DIR, NAME) \
  if ((stat(DIR, &statbuf) == -1) || \
      !(S_ISDIR(statbuf.st_mode))) { \
    fprintf(stderr, \
	    "Can't find the directory \"%s\"!\n", DIR); \
    fprintf(stderr, \
	    "The environment variable \"%s\" should name a directory.\n", \
	    NAME); \
    lost = 1; \
  }


void
env_init(void)
{
  char dir[256];
  char *s;
  struct stat statbuf;
  int lost = 0;

  if ((s = getenv("SIMHOME")) == NULL) {
    s = ".";
  }
  COPY(s, HomeDir);
  TESTDIR(HomeDir, "$SIMHOME");

  sprintf(dir, "%s/res/", HomeDir);
  COPY(dir, ResourceDir);
  TESTDIR(ResourceDir, "$SIMHOME/res");

  { extern char *TCL_Library, *TK_Library;
    TCL_Library = TK_Library = ResourceDir;
  }

  if (lost) {
    fprintf(stderr,
	    "Please check the environment or reinstall Micropolis and try again! Sorry!\n");
    sim_exit(1); // Just sets tkMustExit and ExitReturn
    return;
  }

  gettimeofday(&now_time, NULL);
  last_now_time = now_time;
}


void
sim_init(void)
{
  gettimeofday(&start_time, NULL);
  gettimeofday(&beat_time, NULL);

  signal_init();

  UserSoundOn = 1;
  MustUpdateOptions = 1;
  HaveLastMessage = 0;
  ScenarioID = 0;
  StartingYear = 1900;
  tileSynch = 0x01;
  sim_skips = sim_skip = 0;
  autoGo = 1;
  CityTax = 7;
  CityTime = 50;
  NoDisasters = 0;
  PunishCnt = 0;
  autoBulldoze = 1;
  autoBudget = 1;
  MesNum = 0;
  LastMesTime = 0;
  flagBlink = 1;
  SimSpeed = 3;
  ChangeEval();
  MessagePort = 0;
  MesX = 0;
  MesY = 0;
  sim_paused = 0;
  sim_loops = 0;
  InitSimLoad = 2;
  tkMustExit = 0;
  ExitReturn = 0;

  InitializeSound();
  initMapArrays();
  initGraphs();
  InitFundingLevel();
#if 0
  loadObjectData();
#endif
  setUpMapProcs();
  StopEarthquake();
  ResetMapState();
  ResetEditorState();
  ClearMap();
  InitWillStuff();
  SetFunds(5000);
  SetGameLevelFunds(StartupGameLevel);
  setSpeed(0);
  setSkips(0);
}


int triedToBailOnce = 0;

void
SignalExitHandler()
{
  if (triedToBailOnce) {
    exit(-1);
  } else {
    triedToBailOnce = 1;
    fprintf(stderr, "\nMicropolis has been terminated by a signal.\n");
    fprintf(stderr, "Pick a window -- you're leaving!\n\n");
    fflush(stderr);
    sim_really_exit(-1);
  }
}


signal_init()
{
  signal(SIGHUP, (void (*)())SignalExitHandler);
  signal(SIGINT, (void (*)())SignalExitHandler);
  signal(SIGQUIT, (void (*)())SignalExitHandler);
  signal(SIGTERM, (void (*)())SignalExitHandler);
}


void
sim_update()
{
  gettimeofday(&now_time, NULL);

  flagBlink = (now_time.tv_usec < 500000) ? 1 : -1;

  if (SimSpeed && !heat_steps) {
    TilesAnimated = 0;
  }

  sim_update_editors();

  sim_update_maps();
  sim_update_graphs();
  sim_update_budgets();
  sim_update_evaluations();

  UpdateFlush();
}


sim_update_editors(void)
{
  SimView *view;

  for (view = sim->editor; view != NULL; view = view->next) {
#if 1
    CancelRedrawView(view);
    view->invalid = 1;
    DoUpdateEditor(view);
#else
    EventuallyRedrawView(view);
#endif
  }

  DoUpdateHeads();
}


sim_update_maps(void)
{
  SimView *view;
  int i;

  for (view = sim->map; view != NULL; view = view->next) {
    int mustUpdateMap =
      NewMapFlags[view->map_state] || NewMap || ShakeNow;
    if (mustUpdateMap) {
      view->invalid = 1;
    }
    if (view->invalid) {
#if 1
      if (mustUpdateMap) {
//fprintf(stderr, "sim_update_maps mustUpdateMap\n");
//	view->skip = 0;
      }
      if (DoUpdateMap(view)) {
//          CancelRedrawView(view);
//	  view->invalid = 1;
      }
#else
      EventuallyRedrawView(view);
#endif
    }
  }

  NewMap = 0;
  for (i = 0; i < NMAPS; i++) {
    NewMapFlags[i] = 0;
  }
}


sim_update_graphs(void)
{
  graphDoer();
}


sim_update_budgets(void)
{
  if ((sim_skips != 0) &&
      (sim_skip != 0)) {
    return;
  }

  UpdateBudgetWindow();
}


sim_update_evaluations(void)
{
  if ((sim_skips != 0) &&
      (sim_skip != 0)) {
    return;
  }

  scoreDoer();
}


#ifdef CAM

sim_update_cams(void)
{
  SimCam *scam;

  if ((sim_skips != 0) &&
      (sim_skip != 0)) {
    return;
  }

  for (scam = sim->scam; scam != NULL; scam = scam->next) {
    CancelRedrawView(scam);
    scam->invalid = 1;
    handle_scam(scam);
  }
}

#endif /* CAM */


short *CellSrc = NULL;
short *CellDst = NULL;

#define SRCCOL (WORLD_Y + 2)
#define DSTCOL WORLD_Y

#define CLIPPER_LOOP_BODY(CODE) \
    src = CellSrc; dst = CellDst; \
    for (x = 0; x < WORLD_X;) { \
      short nw, n, ne, w, c, e, sw, s, se; \
      \
      src = CellSrc + (x * SRCCOL); dst = CellDst + (x * DSTCOL); \
      w = src[0]; c = src[SRCCOL]; e = src[2 * SRCCOL]; \
      sw = src[1]; s = src[SRCCOL + 1]; se = src[(2 * SRCCOL) + 1]; \
      \
      for (y = 0; y < WORLD_Y; y++) { \
        nw = w; w = sw; sw = src[2]; \
	n = c; c = s; s = src[SRCCOL + 2]; \
	ne = e; e = se; se = src[(2 * SRCCOL) + 2]; \
	{ CODE } \
	src++; dst++; \
      } \
      x++; /* src += SRCCOL - 3; dst += DSTCOL - 1; */ \
      src = CellSrc + ((x + 1) * SRCCOL) - 3; dst = CellDst + ((x + 1) * DSTCOL) - 1; \
      \
      nw = src[1]; n = src[SRCCOL + 1]; ne = src[(2 * SRCCOL) + 1]; \
      w = src[2]; c = src[SRCCOL + 2]; e = src[(2 * SRCCOL) + 2]; \
      \
      for (y = WORLD_Y - 1; y >= 0; y--) { \
        sw = w; w = nw; nw = src[0]; \
        s = c; c = n; n = src[SRCCOL]; \
        se = e; e = ne; ne = src[2 * SRCCOL]; \
	{ CODE } \
	src--; dst--; \
      } \
      x++; /* src += SRCCOL + 3; dst += DSTCOL + 1; */ \
    }

void
sim_heat(void)
{
  int x, y, l, r, u, d;
  static int a = 0;
  short *src, *dst;
  register int fl = heat_flow;

  if (CellSrc == NULL) {
    CellSrc = (short *)ckalloc((WORLD_X + 2) * (WORLD_Y + 2) * sizeof (short));
    CellDst = &Map[0][0];
  }

  src = CellSrc + SRCCOL + 1;
  dst = CellDst;

/*
 * Copy wrapping edges:
 *
 *	0	ff	f0 f1 ... fe ff		f0
 *
 *	1	0f	00 01 ... 0e 0f		00
 *	2	1f	10 11 ... 1e 1f		10
 *		..	.. ..     .. ..		..
 *		ef	e0 e1 ... ee ef		e0
 *	h	ff	f0 f1 ... fe ff		f0
 *
 *	h+1	0f	00 01 ... 0e 0f		00
 *
 * wrap value:	effect:
 *	0	no effect
 *	1	copy future=>past, no wrap
 *	2	no copy, wrap edges
 *	3	copy future=>past, wrap edges
 *	4	copy future=>past, same edges
 */

  switch (heat_wrap) {
  case 0:
    break;
  case 1:
    for (x = 0; x < WORLD_X; x++) {
      memcpy(src, dst, WORLD_Y * sizeof (short));
      src += SRCCOL;
      dst += DSTCOL;
    }
    break;
  case 2:
    for (x = 0; x < WORLD_X; x++) {
      src[-1] = src[WORLD_Y - 1];
      src[WORLD_Y] = src[0];
      src += SRCCOL;
      dst += DSTCOL;
    }
    memcpy(CellSrc,CellSrc + (SRCCOL * WORLD_X),
	  SRCCOL * sizeof (short));
    memcpy(CellSrc + SRCCOL * (WORLD_X + 1), CellSrc + SRCCOL,
	  SRCCOL * sizeof (short));
    break;
  case 3:
    for (x = 0; x < WORLD_X; x++) {
      memcpy(src, dst, WORLD_Y * sizeof (short));
      src[-1] = src[WORLD_Y - 1];
      src[WORLD_Y] = src[0];
      src += SRCCOL;
      dst += DSTCOL;
    }
    memcpy(CellSrc, CellSrc + (SRCCOL * WORLD_X),
	   SRCCOL * sizeof (short));
    memcpy(CellSrc + SRCCOL * (WORLD_X + 1), CellSrc + SRCCOL,
	   SRCCOL * sizeof (short));
    break;
  case 4:
    src[0] = dst[0];
    src[1 + WORLD_Y] = dst[WORLD_Y - 1];
    src[(1 + WORLD_X) * SRCCOL] = dst[(WORLD_X - 1) * DSTCOL];
    src[((2 + WORLD_X) * SRCCOL) - 1] = dst[(WORLD_X * WORLD_Y) - 1];
    for (x = 0; x < WORLD_X; x++) {
      memcpy(src, dst, WORLD_Y * sizeof (short));
      src[-1] = src[0];
      src[WORLD_Y] =  src[WORLD_Y - 1];
      src += SRCCOL;
      dst += DSTCOL;
    }
    memcpy(CellSrc + (SRCCOL * (WORLD_X + 1)), CellSrc + (SRCCOL * WORLD_X),
	   SRCCOL * sizeof (short));
    memcpy(CellSrc, CellSrc + SRCCOL,
	   SRCCOL * sizeof (short));
    break;
  }


  switch (heat_rule) {

  case 0:
#define HEAT \
	a += nw + n + ne + w + e + sw + s + se + fl; \
	dst[0] = ((a >> 3) & LOMASK) | \
		     (ANIMBIT | BURNBIT | BULLBIT); \
	a &= 7;

    CLIPPER_LOOP_BODY(HEAT);
    break;

  case 1:
#define ECOMASK 0x3fc
#define ECO \
      c -= fl; n -= fl; s -= fl; e -= fl; w -= fl; \
      ne -= fl; nw -= fl; se -= fl; sw -= fl; \
      \
      /* anneal */ \
      { int sum = (c&1) + (n&1) + (s&1) + (e&1) + (w&1) + \
		  (ne&1) + (nw&1) + (se&1) + (sw&1), cell; \
	if (((sum > 5) || (sum == 4))) { \
	  /* brian's brain */ \
	  cell = ((c <<1) & (0x3fc)) | \
		 (((((c >>1)&3) == 0) && \
		   (((n&2) + (s&2) + (e&2) + (w&2) + \
		     (ne&2) + (nw&2) + (se&2) + (sw&2)) == (2 <<1)) \
		  ) ? 2 : 0) | \
		 1; \
	} else { \
	  /* anti-life */ \
	  sum = ((n&2) + (s&2) + (e&2) + (w&2) + \
		 (ne&2) + (nw&2) + (se&2) + (sw&2)) >>1; \
	  cell = (((c ^ 2) <<1) & ECOMASK) | \
		 ((c&2) ? ((sum != 5) ? 2 : 0) \
			: (((sum != 5) && (sum != 6)) ? 2 : 0)); \
	} \
	dst[0] = ((fl + cell) & LOMASK) | \
		 (ANIMBIT | BURNBIT | BULLBIT); \
      } \
      c += fl; n += fl; s += fl; e += fl; w += fl; \
      ne += fl; nw += fl; se += fl; sw += fl;

    CLIPPER_LOOP_BODY(ECO);
    break;
  }
}


void
sim_timeout_loop(short doSim)
{
  if (SimSpeed) {
    sim_loop(doSim);
  }
  DoTimeoutListen();
}


sim_loop(int doSim)
{
#ifdef CAM
  if (!sim_just_cam) {
#endif
    if (heat_steps) {
      int j;

      for (j = 0; j < heat_steps; j++) {
	sim_heat();
      }

      MoveObjects();
/*
      InvalidateMaps();
*/
      NewMap = 1;
    } else {
      if (doSim) {
	SimFrame();
      }
      MoveObjects();
    }

    sim_loops++;
    sim_update();
#ifdef CAM
  } else {
    sim_update_cams();
    UpdateFlush();
    DoTimeoutListen();
  }
#endif
}


int
MatchArg(char *arg, char *pat)
{
  while (*pat && *arg) {
    if (tolower(*arg) != tolower(*pat)) {
      if (*pat == '_') {
	pat++;
	continue;
      }
      return (0);
    }
    arg++; pat++;
  }
  return (*arg == '\0');
}


int
main(int argc, char *argv[])
{
  int c, errflg = 0;
  extern int isatty();

  printf("Welcome to X11 Multi Player Micropolis version %s by Will Wright, Don Hopkins.\n",
	 MicropolisVersion);
  printf("Copyright (C) 2002 by Electronic Arts, Maxis. All rights reserved.\n");

  while (!errflg &&
	 !tkMustExit &&
	 (c = getopt(argc, argv, "tcwmSR:gs:l:")) != -1) {
    switch (c) {

    case 't': /* TTY mode */
      sim_tty = isatty(0);
      break;

    case 'c': /* Create Own Colormap */
      { extern int TK_CreateColormap;
	TK_CreateColormap = 1;
      }
      break;

    case 'w': /* Wire Mode (don't use shared memory) */
      WireMode = 1;
      break;

    case 'm': /* Multi Player Mode */
      MultiPlayerMode = 1;
      break;


    case 'g': /* Generate New Terrain */
      if (Startup) {
	errflg = 1;
      } else {
	Startup = -1;
      }
      break;

    case 'l': /* Level */
      c = atoi(optarg);
      if (!c) {
	if (MatchArg(optarg, "easy")) {
	  c = 1;
	} else if (MatchArg(optarg, "medium")) {
	  c = 2;
	} else if (MatchArg(optarg, "hard")) {
	  c = 3;
	}
      }
      if ((c < 1) || (c > 3)) {
	errflg = 1;
      } else {
	StartupGameLevel = c - 1;
      }
      break;

    case 's': /* Scenario <name> */
      if (Startup)
	errflg++;
      else {
	c = atoi(optarg);
	if (!c) {
	  if (MatchArg(optarg, "Dullsville")) {
	    c = 1;
	  } else if (MatchArg(optarg, "San_Francisco")) {
	    c = 2;
	  } else if (MatchArg(optarg, "Hamburg")) {
	    c = 3;
	  } else if (MatchArg(optarg, "Bern")) {
	    c = 4;
	  } else if (MatchArg(optarg, "Tokyo")) {
	    c = 5;
	  } else if (MatchArg(optarg, "Detroit")) {
	    c = 6;
	  } else if (MatchArg(optarg, "Boston")) {
	    c = 7;
	  } else if (MatchArg(optarg, "Rio_de_Janeiro")) {
	    c = 8;
	  }
	}
	if ((c < 1) || (c > 8)) {
	  errflg = 1;
	} else {
	  Startup = c;
	}
      }
      break;

    case 'd': /* Display <name> */
      { char *d = Displays;

	if (d == NULL) {
	  Displays = malloc(strlen(optarg) + 3);
	  sprintf(Displays, "{%s}", optarg);

	  if (strchr(optarg, ':') != NULL) {
	    FirstDisplay = malloc(strlen(optarg) + 1);
	    strcpy(FirstDisplay, optarg);
	  } else {
	    FirstDisplay = malloc(strlen(optarg) + 3);
	    sprintf(FirstDisplay, "%s:0", optarg);
	  }
	} else {
	  /* Implicitly set multi player mode if multiple displays given. */
	  MultiPlayerMode = 1;
	  Displays = malloc(strlen(Displays) + strlen(optarg) + 4);
	  sprintf(Displays, "%s {%s}", d, optarg);
	  free(d);
	}
      }
      break;

    case '?':
      errflg++;
      break;
    }
  }

  if (!errflg &&
      !tkMustExit) {
    if ((Startup == -1) ||
	(Startup == -2)) {
      /* Generate New City */
      if ((optind != argc) && (optind != argc - 1)) {
	errflg = 1;
      } else {
	if (optind == argc - 1)
	  StartupName = argv[optind];
      }
    } else if (Startup > 0) {
      /* Scenario */
    } else if (optind == argc - 1) {
      /* Load City */
      Startup = -2;
      StartupName = argv[optind];
    } else if (optind == argc) {
      /* No arguments */
    } else {
      errflg = 1;
    }
  }

  if (errflg) {
    fprintf(stderr,
"usage: %s\n", argv[0]);
    fprintf(stderr,
"        [-s(cenario) number|name]\n");
    fprintf(stderr,
"        [-g(enerate random map and start playing)\n");
    fprintf(stderr,
"        [-l(evel) number|name]\n");
    fprintf(stderr,
"        [-w(ire mode: use X11 networking without shared memory)]\n");
    fprintf(stderr,
"        [-t(ty mode: interactive TCL session on stdin/stdout)]\n");
    fprintf(stderr,
"        [-c(olormap mode: create own X11 colormap on 8 bit screens)]\n");
    fprintf(stderr,
"        [-m(ulti player mode: enable adding multiple players via X11)]\n");
    fprintf(stderr,
"        [NewCityName]\n");
    fprintf(stderr,
"        [SavedFileName.city]\n");
    fprintf(stderr,
"The game level and NewCityName argument are optional, and only apply when\n");
    fprintf(stderr,
"starting a new city or generating new terrain.\n");
    fprintf(stderr,
"Game levels include: 1: Easy, 2: Medium, 3: Hard\n");
    fprintf(stderr,
"Scenarios include: 1: Dullsville, 2: San_Francisco, 3: Hamburg, 4: Bern,\n");
    fprintf(stderr,
"		    5: Tokyo, 6: Detroit, 7: Boston, 8: Rio_de_Janeiro\n");
    fflush(stderr);
    sim_exit(0); // Just sets tkMustExit and ExitReturn
  }

  if ((!tkMustExit) &&
      (Displays == NULL)) {
    char *d = getenv("DISPLAY");

    if (d == NULL) d = ":0";

    Displays = malloc(strlen(d) + 3);
    sprintf(Displays, "{%s}", d);
    if (strchr(d, ':') != NULL) {
      FirstDisplay = malloc(strlen(d) + 1);
      strcpy(FirstDisplay, d);
    } else {
      FirstDisplay = malloc(strlen(d) + 3);
      sprintf(FirstDisplay, "%s:0", d);
    }
  }

  if (!tkMustExit) {
    env_init();
  }

  if (!tkMustExit) {
    tk_main();
  }

  exit (ExitReturn);
}
