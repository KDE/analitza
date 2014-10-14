#! /bin/sh
perl extractplots --context="dictionary" analitzaplot/data/plots/*.plots >> rc.cpp
$XGETTEXT_QT analitzaplot/*.cpp analitzagui/*.cpp analitza/*.cpp rc.cpp -o $podir/analitza_qt.pot
