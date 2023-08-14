#!/bin/bash

#
# dirty script to get things prepped for release/distribution --cypress
#

mkdir -p release
mkdir -p release/id1

make clean && make
cp build/EBOOT.PBP release/
cp -R assets/hooks/ release/
cp -R assets/patches/ release/
cp assets/quake.cmdline release/
cp assets/gpl-2.0.txt release/
cp README.md release/
cp assets/config.cfg release/id1/

#download shareware pak from archive.org
cd release/id1
wget https://archive.org/download/quake-shareware-pak/PAK0.PAK
