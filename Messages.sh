#! /bin/sh
perl extractplots --context="dictionary" analitzaplot/data/plots/*.plots >> rc.cpp
$EXTRACT_TR_STRINGS analitzaplot/*.cpp analitzagui/*.cpp analitza/*.cpp rc.cpp -o $podir/analitza_qt.pot
