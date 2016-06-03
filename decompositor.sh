#!/bin/sh

PROC=4
NET=32

if [ ! -z $1 ]; then
    NET=$1
fi

if [ ! -z $2 ]; then
    PROC=$2
fi

shift
shift

HOLES=

for i in $@; do
    HOLES="$HOLES -P $i"
done
DIR=dec-$PROC-$NET

rm -rf $DIR
mkdir -p $DIR

echo ./decompositor -n $NET -m $NET -p $PROC $HOLES -o $DIR
./decompositor -n $NET -m $NET -p $PROC $HOLES -o $DIR
