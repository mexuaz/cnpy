#!/bin/bash
#SBATCH --account=def-thomo
#SBATCH --time=0-03:00:00           # time (DD-HH:MM:SS)
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=16
#SBATCH --mem=64G # 0 to reserve all the available memory on each node assigned to the job. (Cedar:125 GB, Cedar: 187GB)
#SBATCH --output=output%j_cnpy.out   # standard output
#SBATCH --error=error%j_cnpy.err   # standard error

module load gcc/9.1.0 tbb/2018_U5 cmake/3.12.3 boost/1.68.0

RPODIR="$HOME/cnpy"
SRCDIR="$RPODIR"
BINDIR="$HOME/bin-cnpy"
DATDIR="$SCRATCH"
PROGRM="$BINDIR/graph_multi_cvt"
DATAST=("hollywood-2009" "hollywood-2011" "eu-2015-tpd")

cd "$DATDIR"

if [ $# -eq 0 ]; then
    for arg in ${DATAST[*]}; do
	echo "Converting $arg dataset to other formats ..."
        $PROGRM "$DATDIR/$arg.txt"
    done
else
    for arg in "$@"; do
	echo "Converting $arg dataset to other formats ..."
        $PROGRM "$DATDIR/$arg.txt"
    done
fi
