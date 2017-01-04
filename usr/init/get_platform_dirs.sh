#!/bin/sh

exec 3<>"plat/$1/$1.plat"

echo $file
while read -r line <&3;
	do
		find $line -name "*.c"
done 