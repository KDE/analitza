#! /bin/sh
perl extractplots --context="dictionary" analitzaplot/data/plots/*.plots >> rc.cpp
$EXTRACT_TR_STRINGS `find . -name \*.cpp -print -o -path ./analitzaplot/examples -prune` -o $podir/analitza_qt.pot
