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

module --force purge
module load nixpkgs/16.09 intel/2019.3 cmake/3.12.3 boost/1.68.0 hdf5/1.10.5

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




