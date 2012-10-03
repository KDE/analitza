#! /bin/sh
perl extractplots --context="dictionary" analitzaplot/data/plots/*.plots >> rc.cpp
$XGETTEXT analitzagui/*.cpp analitza/*.cpp rc.cpp -o $podir/analitza.pot

rm -f rc.cpp
