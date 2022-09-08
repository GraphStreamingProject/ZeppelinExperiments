#!/usr/bin/env bash

function runcmd {
  echo "Running command $@"
  "$@"
}

echo 'Specify parameters for experiments'
# TODO: cont_test: samples, runs
# TODO: parallel_experiment: max_threads
# TODO: query_test: num_buffer_elems

echo 'Select datasets to download'

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
declare -a selected
while true; do
  # Print menu
  declare -i i
  for ((i = 0; i < ${#datasets[@]}; i++)); do
    selstr="${selected[$i]:+*}"
    selstr="${selstr:- }"
    echo "${selstr} $(($i + 1))) ${datasets[$i]} ${dataset_sizes[$i]}"
  done
  echo 'Enter number to toggle selection, or "done" to continue'

  read -a sel_change

  # If done, check that a dataset has been selected and proceed
  if [[ "${sel_change[*]}" == 'done' ]]; then
    declare -i all_unselected=1
    for ((i = 0; i < ${#datasets[@]}; i++)); do
      if [[ -n "${selected[$i]}" ]]; then
        all_unselected=0
        break
      fi
    done
    if (( all_unselected )); then
      echo 'At least one dataset must be selected'
    else
      break
    fi
  fi

  # Toggle specified selections
  for sel in "${sel_change[@]}"; do
    if [[ "$sel" =~ ^[0-9]+$ ]]; then
      declare -i sel_idx
      sel_idx=$((10#$sel - 1))
      if (( 0 <= $sel_idx && $sel_idx < ${#datasets[@]} )); then
        sel_val=${selected[$sel_idx]:+0}
        sel_val=${sel_val:-01}
        sel_val=${sel_val:1}
        selected[$sel_idx]=$sel_val
      fi
    fi
  done
done

echo 'Running CMake build'

runcmd mkdir -p build
runcmd cd build
runcmd cmake ..
runcmd cmake --build .

echo 'Finished running CMake build'

echo 'Downloading datasets'

runcmd mkdir -p datasets
for ((i = 0; i < ${#datasets[@]}; i++)); do
  if [[ -n "${selected[$i]}" ]]; then
    runcmd wget -O "datasets/${dataset_filenames[$i]}" "${dataset_links[$i]}"
  fi
done

echo 'Finished downloading datasets'

dataset_files=(datasets/*)
echo 'Running experiments'
runcmd mkdir -p buffer_expr_results
for dataset in "${dataset_files[@]}"; do
  runcmd ./buffersize_experiment "$dataset" "buffer_expr_results/${dataset##*/}.out"
done

runcmd mkdir -p cont_expr_results
for dataset in "${dataset_files[@]}"; do
  runcmd ./cont_test "$dataset" "TODO: samples" "TODO: runs"
done

runcmd mkdir -p parallel_expr_results
for dataset in "${dataset_files[@]}"; do
  runcmd ./parallel_experiment "$dataset" "parallel_expr_results/${dataset##/}.out" "TODO: threads"
done

runcmd mkdir -p query_expr_results
for dataset in "${dataset_files[@]}"; do
  runcmd ./query_test "$dataset" "TODO: num_buffer_elems"
done

runcmd ../sketch_expr/run_sketch_expr.sh

runcmd ../speed_expr/run_experiments.sh "${dataset_files[@]}"

runcmd ../speed_expr/run_unlim_experiments.sh "${dataset_files[@]}"

echo 'Finished running experiments'
