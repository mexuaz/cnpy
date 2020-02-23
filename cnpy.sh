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
PROGRM="$BINDIR/boost_2_npz"
DATAST=("twitter-2010" "uk-2005" "soc-LiveJournal1" "arabic-2005" "indochina-2004" "amazon")

cd "$DATDIR"

if [ $# -eq 0 ]; then
    for arg in ${DATAST[*]}; do
	echo "Converting $arg dataset to npy format ..."
        $PROGRM "$DATDIR/$arg.ser"
    done
else
    for arg in "$@"; do
	echo "Converting $arg dataset to npy format ..."
        $PROGRM "$DATDIR/$arg.ser"
    done
fi
