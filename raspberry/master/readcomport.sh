#!/bin/sh

while head -c 1 /dev/ttyAMA0
do
:	 # a dummy command that always returns true
done
