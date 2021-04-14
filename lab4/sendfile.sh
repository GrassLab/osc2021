#!/bin/bash
#tty=/dev/pts/4
tty=/dev/ttyUSB0
size=`stat --printf="%s" $1`
echo $size
printf "%08x" $size | sudo tee $tty
sudo tee $tty < $1   
