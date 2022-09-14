#! /bin/bash

# run the experiments from the build dir as follows
# /path/to/run_lim_speed_expr.sh use_guttertree[yes/no] input_files[input_files+]
# use_guttertree = either 'yes' or 'no'. Indicates if guttertree or standalone gutters should be used
# input_files = the graph streams to test upon

use_guttertree=$1
shift 1

result_dir="speed_results"

if [[ "$use_guttertree" == "yes" ]]; then
	result_dir="speed_gutter_results"
fi

# make a results directory
mkdir $result_dir 2> /dev/null

for input in $@
do
	echo ""
	echo "Processing input file: $input"

	mkdir $result_dir/test_`basename $input` 2> /dev/null

	# run speed_experiment as seperate process with memory limitation
	cgexec -g memory:16_GB ./speed_process_stream $input $result_dir/test_`basename $input`/runtime_stats $use_guttertree &
	pid=$!

	#setup top logging
	top -b -Em -p $pid > $result_dir/test_`basename $input`/memory_stats &
	top_pid=$!

	# wait for speed experiment to finish then kill top
	wait $pid
	kill -9 $top_pid

	# use the top log to get the maximum memory usage
	./top_cleaner < $result_dir/test_`basename $input`/memory_stats | ./top_max_finder > $result_dir/test_`basename $input`/max_memory
done

