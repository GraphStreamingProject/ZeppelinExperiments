#!/usr/bin/env bash
mkdir speed_results_aspen 2> /dev/null
mkdir speed_results_terrace 2> /dev/null

for input in "$@"
do
	echo "Processing input file: $input"

	mkdir speed_results_aspen/test_`basename $input` 2> /dev/null

	# run aspen speed experiment with memory limitation
	cgexec -g memory:16_GB ./aspen_ingest_expr $input 100000 100000 speed_results_aspen/test_`basename $input`/runtime_stats &
	pid=$!

	#setup top logging
	top -b -Em -p $pid > speed_results_aspen/test_`basename $input`/memory_stats &
	top_pid=$!

	# wait for aspen speed experiment to finish then kill top
	wait $pid
	kill -9 $top_pid

	# use the top log to get the maximum memory usage
	./top_cleaner < speed_results_aspen/test_`basename $input`/memory_stats | ./top_max_finder > speed_results_aspen/test_`basename $input`/max_memory

	mkdir speed_results_terrace/test_`basename $input` 2> /dev/null

	# run terrace speed experiment with memory limitation
	if [[ `basename $input` != "kron_18_stream_binary" ]]; then
		cgexec -g memory:16_GB ./terrace_ingest_expr $input 100000 100000 speed_results_terrace/test_`basename $input`/runtime_stats &
		pid=$!

		#setup top logging
		top -b -Em -p $pid > speed_results_terrace/test_`basename $input`/memory_stats &
		top_pid=$!

		#wait for terrace speed experiment to finish then kill top
		wait $pid
		kill -9 $top_pid

		# use the top log to get the maximum memory usage
		./top_cleaner < speed_results_terrace/test_`basename $input`/memory_stats | ./top_max_finder > speed_results_terrace/test_`basename $input`/max_memory

	fi
done