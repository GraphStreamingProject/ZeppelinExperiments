#!/usr/bin/env bash

function runcmd {
  echo "Running command $@"
#  "$@"
}


full_expr_msg='Our full experiments take a significant amount of time, and in the interest of time, this script can run a limited set of experiments. '
full_expr_msg+='The kron 18 graph is a 161GB dataset, and not all experiments process it. '
full_expr_msg+='The limited set of experiments omits kron 18. '
full_expr_msg+='The full 10 repetitions of continuous correctness testing takes over 24 hours. '
full_expr_msg+='The limited set of experiments only does 1 run. '
full_expr_msg+='Would you like to run the full set of experiments? (N/Y)'

declare -i full_expr=-1
while (( $full_expr == -1 )); do
  read -r -p "$full_expr_msg" full_expr_input
  case "$full_expr_input" in
    'N'|'n') full_expr=0;;
    'Y'|'y') full_expr=1;;
  esac
done

datasets=(
  'kron13'
  'kron15'
  'kron16'
  'kron17'
  'kron18'
  'p2p-gnutella'
  'rec-amazon'
  'google-plus'
  'web-uk'
)
dataset_sizes=(
  '150.6MB'
  '2.3GB'
  '9.4GB'
  '37.5GB'
  '150.0GB'
  '2.5MB'
  '2.1MB'
  '232.9MB'
  '200.1MB'
)
dataset_filenames=(
  'kron_13_stream_binary'
  'kron_15_stream_binary'
  'kron_16_stream_binary'
  'kron_17_stream_binary'
  'kron_18_stream_binary'
  'p2p_gnutella_stream_binary'
  'rec_amazon_stream_binary'
  'google_plus_stream_binary'
  'web_uk_stream_binary'
)
dataset_links=(
  'https://graphzeppelin-datasets.s3.amazonaws.com/kron_13_stream_binary'
  'https://graphzeppelin-datasets.s3.amazonaws.com/kron_15_stream_binary'
  'https://graphzeppelin-datasets.s3.amazonaws.com/kron_16_stream_binary'
  'https://graphzeppelin-datasets.s3.amazonaws.com/kron_17_stream_binary'
  'https://graphzeppelin-datasets.s3.amazonaws.com/kron_18_stream_binary'
  'https://graphzeppelin-datasets.s3.amazonaws.com/p2p_gnutella_stream_binary'
  'https://graphzeppelin-datasets.s3.amazonaws.com/rec_amazon_stream_binary'
  'https://graphzeppelin-datasets.s3.amazonaws.com/google_plus_stream_binary'
  'https://graphzeppelin-datasets.s3.amazonaws.com/web_uk_stream_binary'
)

declare -a all_datasets
declare -a kron_datasets
declare -i cont_expr_samples=100
declare -i cont_expr_runs
if (( $full_expr == 1 )); then
  all_datasets=(1 2 3 4 5 6 7 8 9)
  kron_datasets=(1 2 3 4 5)
  cont_expr_runs=10
else
  all_datasets=(1 2 3 4 6 7 8 9)
  kron_datasets=(1 2 3 4)
  cont_expr_runs=1
fi



echo 'Running CMake build'

runcmd mkdir -p build
runcmd cd build
runcmd cmake ..
runcmd cmake --build .

echo 'Finished running CMake build'

echo 'Downloading datasets'

runcmd mkdir -p datasets
for dataset in "${all_datasets[@]}"; do
  runcmd wget -O "datasets/${dataset_filenames[$dataset]}" "${dataset_links[$dataset]}"
done

echo 'Finished downloading datasets'

echo 'Running experiments'

runcmd mkdir -p speed_expr_results
for dataset in "${kron_datasets[@]}"; do
  dataset_filename="${dataset_filenames[$dataset]}"
  runcmd ./speed_experiment "speed_expr_results/$dataset_filename.csv" "datasets/$dataset_filename"
done

runcmd mkdir -p cont_expr_results
for dataset in "${dataset_files[@]}"; do
  runcmd ./cont_test "$dataset" "$cont_expr_samples" "$cont_expr_runs"
done

runcmd mkdir -p buffer_expr_results
for dataset in "${dataset_files[@]}"; do
  runcmd ./buffersize_experiment "datasets/kron_17_stream_binary" "buffer_expr_results/bexpr_out.txt" "buffer_expr_results/buffer_csv"
done

runcmd mkdir -p parallel_expr_results
for dataset in "${dataset_files[@]}"; do
  runcmd ./parallel_experiment "datasets/kron_17_stream_binary" "parallel_expr_results/pexpr_out.txt" "parallel_expr_results/parallel_csv" 46
done

runcmd mkdir -p query_expr_results
for dataset in "${dataset_files[@]}"; do
  runcmd ./query_test "$dataset" "TODO: num_buffer_elems"
done

runcmd ../sketch_expr/run_sketch_expr.sh

runcmd ../speed_expr/run_experiments.sh "${dataset_files[@]}"

runcmd ../speed_expr/run_unlim_experiments.sh "${dataset_files[@]}"

echo 'Finished running experiments'
