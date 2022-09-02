#!/usr/bin/env bash

function runcmd {
  echo "Running command $@"
  "$@"
}

echo 'Running CMake build'

# runcmd mkdir -p build
# runcmd cd build
# runcmd cmake ..
# runcmd cmake --build .

echo 'Finished running CMake build'

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
datalinks=(
  'http://whereverkron13'
  'http://whereverkron15'
  'http://whereverkron16'
  'http://whereverkron17'
  'http://whereverkron18'
  'http://whereverp2p-guntella'
  'http://whereverrec-amazon'
  'http://wherevergoogle-plus'
  'http://whereverweb-uk'
)
declare -a selected
while true; do
  # Print menu
  declare -i i
  for ((i = 0; i < ${#datasets[@]}; i++)); do
    selstr="${selected[$i]:+*}"
    selstr="${selstr:- }"
    echo "${selstr} $(($i + 1))) ${datasets[$i]}"
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

echo 'Downloading datasets'

runcmd mkdir -p datasets
for ((i = 0; i < ${#datasets[@]}; i++)); do
  if [[ -n "${selected[$i]}" ]]; then
    #TODO: actual download
    echo "$i: ${datasets[$i]}: ${datalinks[$i]}"
  fi
done

echo 'Finished downloading datasets'

echo 'Running experiments'
for DATASET in datasets/*; do
  runcmd ./buffersize_experiment
done
runcmd ./cont_test
runcmd ./parallel_experiment
runcmd ./query_test
runcmd ../sketch_expr/run_sketch_expr.sh
runcmd ../speed_expr/run_experiments.sh
runcmd ../speed_expr/run_unlim_experiments.sh
echo 'Finished running experiments'
