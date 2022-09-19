#!/usr/bin/env bash
mkdir unlim_results_aspen 2> /dev/null
mkdir unlim_results_terrace 2> /dev/null
set -u

timeout=$1
shift 1

for input in $@
do
	mkdir unlim_results_aspen/test_`basename $input` 2> /dev/null

	echo "ASPEN:"
	echo "stream: $input"
	# run aspen speed experiment without memory limitation
	./aspen_ingest_expr $input 100000 100000 $timeout unlim_results_aspen/test_`basename $input`/runtime_stats &
	pid=$!

	#setup top logging
	top -b -Em -p $pid > unlim_results_aspen/test_`basename $input`/memory_stats &
	top_pid=$!

	# wait for aspen speed experiment to finish then kill top
	wait $pid
	kill -9 $top_pid

	# use the top log to get the maximum memory usage
	./top_cleaner < unlim_results_aspen/test_`basename $input`/memory_stats | ./top_max_finder > unlim_results_aspen/test_`basename $input`/max_memory

	mkdir unlim_results_terrace/test_`basename $input` 2> /dev/null
	# run terrace speed experiment with memory limitation
	if [[ `basename $input` != "kron_18_stream_binary" ]]; then
		echo "TERRACE:"
		echo "stream: $input"
		./terrace_ingest_expr $input 100000 100000 $timeout unlim_results_terrace/test_`basename $input`/runtime_stats &
		pid=$!

		#setup top logging
		top -b -Em -p $pid > unlim_results_terrace/test_`basename $input`/memory_stats &
		top_pid=$!

		#wait for terrace speed experiment to finish then kill top
		wait $pid
		kill -9 $top_pid

		# use the top log to get the maximum memory usage
		./top_cleaner < unlim_results_terrace/test_`basename $input`/memory_stats | ./top_max_finder > unlim_results_terrace/test_`basename $input`/max_memory

	fi
done
