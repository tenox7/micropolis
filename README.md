# Micropolis Legacy

This is a fork of [Micropolis Activity](https://github.com/SimHacker/micropolis/tree/master/micropolis-activity). It's uses the ["old gen"](https://wiki.laptop.org/go/Micropolis#Old_Generation:_C_TCL/Tk_Micropolis) source code, which had only minor, cosmetic changes compared to the [DUX SimCity for Unix](https://web.archive.org/web/19970714233606/http://www.dux.com/simctyux.html). It expects assets from the original game, effectively reversing changes mentioned in [here](https://raw.githubusercontent.com/SimHacker/micropolis/master/micropolis-activity/src/PLAN.txt). Targets vintage Unix systems not supported by the commercial version.


![Micropolis-IRIX](simirix.png)

## Installation

Download a demo version of [DUX SimCity for Unix](https://web.archive.org/web/19970714233306/http://www.dux.com/demo.html) also mirrored [here](http://osarchive.org/apps/simcity) and [here](http://tenox.pdp-11.ru/apps/simcity/).

Unpack the tarball to a folder on your disk, typically under `/usr/SimCity`.

Replace `/usr/SimCity/res/sim` with the binary from the releases folder (or compile yourself). 

## Troubleshooting

### Couldn't add a player on :0.0...

Most commonly this is an authorization issue. Try running `xhost +`.

Second most common problem is lack of 8-bit color display or visual. Try changing number of colors to 8bpp or add add a 8bpp visual.

### The window editor is too small on a modern screen

Edit `res/weditor.tcl` and change "wm maxsize ..."

## Currently supported OS

- AIX 4.3, 5.1 (ppc, ia64)
- IRIX 6.5 (mips)
- HP-UX 11i-v1 (hppa2)
- HP-UX 11i-v3 (ia64)
- Solaris 9 (amd32)

## Planned

- Solaris 10 (amd64)

## Maybe

- UnixWare
- OpenDesktop
- Dell Unix
- Sinix
- Dynix
- DG/UX
- Minix
- Ultrix
- Interactive Unix
- Solaris (ppc)

## Maybe Not

- Solaris 11 - no X11
- QNX - no X11
- NextStep - no X11
- A/UX - no X11

## Legal

- License: GPL
- Copyright (C) 1989 - 2007 Electronic Arts Inc.
