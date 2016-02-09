#!/bin/sh

#
# Configure Java for RXTX on embedded platforms like Raspberry Pi (Raspbian) and Beaglebone (Debian)
#

if [ -e "/dev/ttyAMA0" ] || [ -e "/dev/ttyO0" ]
then
  for port in `find /dev -name 'tty*'`
  do
    PORTS="$PORTS:$port"
  done
  JAVA_OPT="-Djava.library.path=/usr/lib/jni -Dgnu.io.rxtx.SerialPorts=$PORTS"
fi

echo start by next cmd: java $JAVA_OPT -jar JavaApplication3.jar

java $JAVA_OPT -jar JavaApplication3.jar
