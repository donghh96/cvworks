#!/bin/sh

for file in image_rotate_scale/D*.png

do
./cutRotateScaleImg $file
done
