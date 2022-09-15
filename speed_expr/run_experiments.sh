#! /bin/bash

# run the experiments from the build dir as follows
# /path/to/run_experiments.sh input_files[input_files+]
# input_files = the graph streams to test upon

# make a results directory
mkdir ./speed_results

for input in "$@"
do
	echo "Processing input file: $input"

	mkdir speed_results/test_`basename $input`

	# run speed_experiment as seperate process with memory limitation
	cgexec -g memory:16_GB ./speed_process_stream $input speed_results/test_`basename $input`/runtime_stats &
	pid=$!

	#setup top logging
	top -b -Em -p $pid > speed_results/test_`basename $input`/memory_stats &
	top_pid=$!

	# wait for speed experiment to finish then kill top
	wait $pid
	kill -9 $top_pid

	# use the top log to get the maximum memory usage
	./top_cleaner < speed_results/test_`basename $input`/memory_stats | ./top_max_finder > speed_results/test_`basename $input`/max_memory
done

