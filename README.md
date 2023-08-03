# Interstice ProQuake v5.00

This repository contains _Interstice_, an interation upon [_Insomnia_](https://github.com/darkduke606/Insomnia-ProQuake-Engine), part of the bloodline of ProQuake sourceports for the PlayStation Portable handheld.

## Building
Interstice is built using the latest [PSPSDK](https://github.com/pspdev/psptoolchain) available, there is a [Docker](https://hub.docker.com/r/pspdev/pspdev) container available to make deployment easy.

Build with `make`.

`build_release.sh` is a script to automate the build process, as well as place controller bindings and other important files from `assets`. It also downloads the shareware version of Quake from archive.org for easy, legal distribution.

<br>
<br>

### 5.00 Changelog
* Several optmizations inherited from [NZ:P Engine (dQuakePlus)](https://github.com/nzp-team/dquakeplus), [ADQuake](https://github.com/st1x51/adquake), and [FTEQW](https://github.com/fte-team/fteqw).
* A unified binary: No separate EBOOT.PBP files for low and high memory systems, with no compromises made to high memory system advantage.
* Restructured source to remove depreciated features, and adopt the `gu_` code standard (as opposed to `video_hardware`).
* A new navigation interface with support for mod detection (via directory name) with information on compability based on your device.