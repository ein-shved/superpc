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
DIR=dec
for i in $@; do
    DIR="$DIR-$i"
    HOLES="$HOLES -P $i"
done
DIR=$DIR-$PROC-$NET

rm -rf $DIR
mkdir -p $DIR

echo ./decompositor -n $NET -m $NET -p $PROC $HOLES -o $DIR
./decompositor -n $NET -m $NET -p $PROC $HOLES -o $DIR

cat >$DIR/run.sh <<- EOS
#!/bin/sh

TIME=0.03
REALTIME=00:05:00

if [ ! -z \$1 ]; then
    TIME=\$1;
fi
if [ ! -z \$2 ]; then
    REALTIME=\$2;
fi

NAME=$PROC-$NET-\$TIME
RES=/gpfs/data/edu-vmk-stud14-521-15/lab-pg/results/
mkdir -p \$RES
echo mpisubmit.bg -n $PROC -w \$REALTIME -e "OMP_NUM_THREADS=2" --stdout=\$RES/\$NAME.out --stderr=\$RES/\$NAME.err calculation -- -n $NET -m $NET -t \$TIME -o \$RES/\$NAME.ppm -d /gpfs/data/edu-vmk-stud14-521-15/lab-pg/$DIR
mpisubmit.bg -n $PROC -w \$REALTIME -e "OMP_NUM_THREADS=2" --stdout=\$RES/\$NAME.out --stderr=\$RES/\$NAME.err calculation -- -n $NET -m $NET -t \$TIME -o \$RES/\$NAME.ppm -d /gpfs/data/edu-vmk-stud14-521-15/lab-pg/$DIR

EOS

chmod +x $DIR/run.sh
echo scp -r $DIR edu-vmk-stud14-521-15@bluegene1.hpc.cs.msu.ru:~/lab-pg
scp -r $DIR edu-vmk-stud14-521-15@bluegene1.hpc.cs.msu.ru:~/lab-pg > /dev/null
