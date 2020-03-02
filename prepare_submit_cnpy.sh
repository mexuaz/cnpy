#!/bin/bash

RPODIR="$HOME/cnpy"
SRCDIR="$RPODIR"
BINDIR="$HOME/bin-cnpy"

if [[ -d "$RPODIR" ]]; then
    echo "Syncing repository."
    cd "$RPODIR"
    git reset --hard
    git pull
else
    cd $HOME
    git clone git@github.com:mexuaz/cnpy
fi

module load gcc/9.1.0 tbb/2018_U5 cmake/3.12.3 boost/1.68.0

if [[ -d "$BINDIR" ]]
then
    echo "Removing old $BINDIR and creating new one."
    rm -rf "$BINDIR"
fi

mkdir "$BINDIR"
cd "$BINDIR"
cmake "$SRCDIR"
make graph_multi_cvt

cd $SCRATCH
sbatch "$SRCDIR/cnpy.sh"




