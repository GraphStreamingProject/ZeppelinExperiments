#!/usr/bin/env bash

# make a results directory
mkdir ./unlim_results_aspen

for input in "$@"
do
	echo "Processing input file: $input"

	mkdir unlim_results_aspen/test_`basename $input`

	# run speed_experiment as seperate process without
	./speed_process_stream $input unlim_results_aspen/test_`basename $input`/runtime_stats &
	pid=$!

	#setup top logging
	top -b -Em -p $pid > unlim_results_aspen/test_`basename $input`/memory_stats &
	top_pid=$!

	# wait for speed experiment to finish then kill top
	wait $pid
	kill -9 $top_pid

	# use the top log to get the maximum memory usage
	./top_cleaner < unlim_results_aspen/test_`basename $input`/memory_stats | ./top_max_finder > unlim_results_aspen/test_`basename $input`/max_memory
done
