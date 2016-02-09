#!/bin/sh

#
# Configure Java for RXTX on embedded platforms like Raspberry Pi (Raspbian) and Beaglebone (Debian)
#

touch map.xml

java -Djava.library.path=/usr/lib/jni -Dgnu.io.rxtx.SerialPorts=/dev/ttyAMA0 -jar JavaApplication3.jar