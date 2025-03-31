#include "sim.h"


/* Sound routines */


int SoundInitialized = 0;
short Dozing;


InitializeSound()
{
  char cmd[256];

  SoundInitialized = 1;

  if (!UserSoundOn) return;

  Eval("UIInitializeSound");
}


ShutDownSound()
{
  if (SoundInitialized) {
    SoundInitialized = 0;
    Eval("UIShutDownSound");
  }
}


MakeSound(char *channel, char *id)
{
  char buf[256];

  if (!UserSoundOn) return;
  if (!SoundInitialized) InitializeSound();

  sprintf(buf, "UIMakeSound \"%s\" \"%s\"", channel, id);
  Eval(buf);
}


MakeSoundOn(SimView *view, char *channel, char *id)
{
  char buf[256];

  if (!UserSoundOn) return;
  if (!SoundInitialized) InitializeSound();

  sprintf(buf, "UIMakeSoundOn %s \"%s\" \"%s\"",
	  Tk_PathName(view->tkwin), channel, id);
  Eval(buf);
}


StartBulldozer(void)
{
  if (!UserSoundOn) return;
  if (!SoundInitialized) InitializeSound();
  if (!Dozing) {
    DoStartSound("edit", "1");
    Dozing = 1;
  }
}


StopBulldozer(void)
{
  if ((!UserSoundOn) || (!SoundInitialized)) return;
  DoStopSound("1");
  Dozing = 0;
}


/* comefrom: doKeyEvent */
SoundOff(void)
{
  if (!SoundInitialized) InitializeSound();
  Eval("UISoundOff");
  Dozing = 0;
}


DoStartSound(char *channel, char *id)
{
  char buf[256];

  sprintf(buf, "UIStartSound %s %s", channel, id);
  Eval(buf);
}


DoStopSound(char *id)
{
  char buf[256];

  sprintf(buf, "UIStopSound %s", id);
  Eval(buf);
}
