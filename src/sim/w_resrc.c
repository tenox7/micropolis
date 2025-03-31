#include "sim.h"


#ifdef MSDOS
#define PATHSTR	"%s\\%c%c%c%c.%d"
#define PERMSTR	"rb"
#else
#define PATHSTR	"%s/%c%c%c%c.%d"
#define PERMSTR	"r"
#endif


char *HomeDir, *ResourceDir, *KeyDir, *HostName;

struct Resource *Resources = NULL;

struct StringTable {
  QUAD id;
  int lines;
  char **strings;
  struct StringTable *next;
} *StringTables;


Handle GetResource(char *name, QUAD id)
{
  struct Resource *r = Resources;
  char fname[256];
  struct stat st;
  FILE *fp = NULL;

  while (r != NULL) {
    if ((r->id == id) &&
	(strncmp(r->name, name, 4) == 0)) {
      return ((Handle)&r->buf);
    }
    r = r->next;
  }

  r = (struct Resource *)ckalloc(sizeof(struct Resource));

  r->name[0] = name[0];
  r->name[1] = name[1];
  r->name[2] = name[2];
  r->name[3] = name[3];
  r->id = id;

  sprintf(fname, PATHSTR, ResourceDir,
	  r->name[0], r->name[1], r->name[2], r->name[3],
	  r->id);

  if ((stat(fname, &st) < 0) ||
      ((r->size = st.st_size) == 0) ||
      ((r->buf = (char *)ckalloc(r->size)) == NULL) ||
      ((fp = fopen(fname, PERMSTR)) == NULL) ||
      (fread(r->buf, sizeof(char), r->size, fp) != r->size)) {
    if (fp)
      fclose(fp);
    r->buf = NULL;
    r->size = 0;
    fprintf(stderr, "Can't find resource file \"%s\"!\n", fname);
    perror("GetResource");
    return(NULL);
  }
  fclose(fp);
  r->next = Resources; Resources = r;
  return ((Handle)&r->buf);
}


void
ReleaseResource(Handle r)
{
}


QUAD
ResourceSize(Handle h)
{
  struct Resource *r = (struct Resource *)h;

  return (r->size);
}


char *
ResourceName(Handle h)
{
  struct Resource *r = (struct Resource *)h;

  return (r->name);
}


QUAD
ResourceID(Handle h)
{
  struct Resource *r = (struct Resource *)h;

  return (r->id);
}


GetIndString(char *str, int id, short num)
{
  struct StringTable **tp, *st = NULL;
  Handle h;

  tp = &StringTables;

  while (*tp) {
    if ((*tp)->id == id) {
      st = *tp;
      break;
    }
    tp = &((*tp)->next);
  }
  if (!st) {
    QUAD i, lines, size;
    char *buf;

    st = (struct StringTable *)ckalloc(sizeof (struct StringTable));
    st->id = id;
    h = GetResource("stri", id);
    size = ResourceSize(h);
    buf = (char *)*h;
    for (i=0, lines=0; i<size; i++)
      if (buf[i] == '\n') {
	buf[i] = 0;
	lines++;
      }
    st->lines = lines;
    st->strings = (char **)ckalloc(size * sizeof(char *));
    for (i=0; i<lines; i++) {
      st->strings[i] = buf;
      buf += strlen(buf) + 1;
    }
    st->next = StringTables;
    StringTables = st;
  }
  if ((num < 1) || (num > st->lines)) {
    fprintf(stderr, "Out of range string index: %d\n", num);
    strcpy(str, "Well I'll be a monkey's uncle!");
  } {
    strcpy(str, st->strings[num-1]);
  }
}
