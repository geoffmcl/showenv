#!/bin/sh

TMPSRC=".."
TMPOPTS=""

cmake $TMPSRC $TMPOPTS
if [ ! "$?" = "0" ]; then
    echo "cmake config, gen error"
    exit 1
fi

make
if [ ! "$?" = "0" ]; then
    echo "make error"
    exit 1
fi

echo "Appears successful..."
echo "Consider runnig 'sudo make install'"


# eof

