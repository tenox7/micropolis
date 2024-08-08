# Micropolis for Vintage Unix

This is a fork of [Micropolis Activity](https://github.com/SimHacker/micropolis/tree/master/micropolis-activity) that targets vintage commercial Unix systems such as AIX, DEC Unix, HPUX, IRIX, OSF/1, SunOS, etc.

This fork of Micropolis uses assets from vintage [DUX SimCity for Unix](https://web.archive.org/web/19970714233606/http://www.dux.com/simctyux.html) instead of Linux Micropolis.

![Micropolis-IRIX](simirix.png)

## Installation

Download a demo version of [DUX SimCity for Unix](https://web.archive.org/web/19970714233306/http://www.dux.com/demo.html) also mirrored [here](http://osarchive.org/apps/simcity) and [here](http://tenox.pdp-11.ru/apps/simcity/).

Unpack the tarball to a folder on your disk, typically somewhere under `/usr`.

Replace `yourfolder/res/sim` with the binary from the releases folder (or compile yourself). Also `cd /usr/yourfolder; ./sim` should work.

## Currently supported OS

- AIX 4.3
- IRIX 6.5

## Planned

- AIX 5.x (ppc/ia64)
- HP-UX 11.x (hppa/ia64)

