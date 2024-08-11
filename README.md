# Micropolis for Vintage Unix

This is a fork of [Micropolis Activity](https://github.com/SimHacker/micropolis/tree/master/micropolis-activity). It's uses the ["old gen"](https://wiki.laptop.org/go/Micropolis#Old_Generation:_C_TCL/Tk_Micropolis) source code, which had only small, cosmetic changes compared to original source. It expects assets (including TCL/TK GUI) from the original [DUX SimCity for Unix](https://web.archive.org/web/19970714233606/http://www.dux.com/simctyux.html), effectively undoing changes mentioned in [here](https://raw.githubusercontent.com/SimHacker/micropolis/master/micropolis-activity/src/PLAN.txt). Targets vintage commercial Unix systems such as AIX, HPUX, IRIX, OSF/1, SunOS, etc. 


![Micropolis-IRIX](simirix.png)

## Installation

Download a demo version of [DUX SimCity for Unix](https://web.archive.org/web/19970714233306/http://www.dux.com/demo.html) also mirrored [here](http://osarchive.org/apps/simcity) and [here](http://tenox.pdp-11.ru/apps/simcity/).

Unpack the tarball to a folder on your disk, typically somewhere under `/usr`.

Replace `yourfolder/res/sim` with the binary from the releases folder (or compile yourself). Also `cd /usr/yourfolder; ./sim` should work.

## Currently supported OS

- AIX 4.3, 5.1 (ppc)
- IRIX 6.5

## Planned

- AIX 5.1 (ia64)
- HP-UX 11.x (hppa/ia64)

## Legal

- License: GPL
- Copyright (C) 1989 - 2007 Electronic Arts Inc.
