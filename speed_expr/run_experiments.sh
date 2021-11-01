#! /bin/bash

# run the experiments from the build dir as follows
# /path/to/run_experiments.sh input_files[input_files+]
# input_files = the graph streams to test upon

# make a results directory
mkdir ./speed_results

for input in "$@"
do
	echo "Processing input file: $input"

	mkdir results/test_$input

	# run speed_experiment as seperate process
	cgexec -g memory:16_GB ./speed_experiment $input results/test_$(input)/runtime_stats &
	pid=$!

	#setup top logging
	top -b -Em -p $pid > results/test_$input/memory_stats
	top_pid=$!

	# wait for speed experiment to finish then kill top
	wait $pid
	kill -9 $top_pid

#pid=$!
#top -b -Em -p $pid > aspen_experiment_results/erdos_17_16GB_mem &
#top_pid=$!
#wait $pid
#kill -9 $top_pid

done