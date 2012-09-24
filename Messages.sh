#! /bin/sh
sed 's/.*\/\/\([a-z ]*\)\/\/.*/i18nc(\"dictionary\", \"\1\");/' analitzaplot/data/plots/*.plots | grep i18nc\(\" > dictionaryplots_i18n.cpp
$XGETTEXT analitzagui/*.cpp analitza/*.cpp dictionaryplots_i18n.cpp -o $podir/analitza.pot

rm -f dictionaryplots_i18n.cpp
