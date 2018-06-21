#!/bin/sh

rm -f $1 
#for file in ./imageTest/D*.png
for file in ./imageRS/D*.png

do
#./haartransform $file >> haarfeature.data
./haartransform $file >> $1
done

