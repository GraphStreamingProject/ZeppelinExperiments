#!/usr/bin/env bash

function runcmd {
  echo "Running command $@"
  "$@"
}

get_file_path() {
  # $1 : relative filename
  echo "$(cd "$(dirname "$1")" && pwd)/$(basename "$1")"
}

echo "Our full experiments take a significant amount of time!"
echo "This script can run a limited set of experiments or the full run of all experiments."
echo "In the full experiment run we utilize the kron18 dataset (161GB), run over 24 hours of correctness testing, and timeout Aspen/Terrace after 24hrs."
echo "In the limited set of experiments we omit the kron18 dataset, run a tenth of the correctness tests, and timeout Aspen/Terrace after 6 hours."


declare -i full_expr=-1
while (( $full_expr == -1 )); do
  read -r -p "Would you like to run the full set of experiments? (Y/N): " full_expr_input
  case "$full_expr_input" in
    'N'|'n') full_expr=0;;
    'Y'|'y') full_expr=1;;
  esac
done

echo ""
echo "Now we just need the locations in which to place the datasets and the GraphZeppelin on disk data."
echo "Ideally, these two locations would be on different disks."
echo "We will create directories at the specified locations."

read -r -p "Enter location where datasets will be stored: " dataset_disk_str
if [ ! -d $dataset_disk_str ] && ! mkdir $dataset_disk_str; then
  echo "ERROR: could not create directory at location: $dataset_disk_str"
  exit 1
fi

dataset_disk_loc=$(get_file_path $dataset_disk_str)

read -r -p "Enter location (ideally diff disk) where GZ will place disk data: " GZ_disk_str
if [ ! -d $GZ_disk_str ] && ! mkdir $GZ_disk_str; then
  echo "ERROR: could not create directory at location: $GZ_disk_str"
  exit 1
fi

GZ_disk_loc=$(get_file_path $GZ_disk_str)

echo $dataset_disk_loc
echo $GZ_disk_loc

echo ""

# Setup toprc
cp toprc ~/.config/procps/toprc

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
dataset_link_dir='https://graphzeppelin-datasets.s3.amazonaws.com'

csv_directory=csv_files

# This option is ONLY for debugging this script. Causes all experiments to be run only upon kron13
full_expr=2

corr_idx=(3 5 6 7 8)
declare -a all_idx
declare -a kron_idx
declare -i cont_expr_samples=100
declare -i cont_expr_runs
declare -i aspen_terrace_timeout
if (( $full_expr == 1 )); then
  all_idx=(0 1 2 3 4 5 6 7 8)
  kron_idx=(0 1 2 3 4)
  cont_expr_runs=10
  aspen_terrace_timeout=24
elif (( $full_expr == 2 )); then
  # DEBUGGING option that runs all tests just on kron13
  all_idx=(0)
  kron_idx=(0)
  corr_idx=(0)
  cont_expr_runs=1
  aspen_terrace_timeout=6
else
  all_idx=(0 1 2 3 5 6 7 8)
  kron_idx=(0 1 2 3)
  cont_expr_runs=1
  aspen_terrace_timeout=6
fi

all_datasets=()  # every dataset
kron_datasets=() # datasets for speed experiments (all kron graphs)
corr_datasets=() # datasets for correctness tests

kron_17_dataset=${kron_datasets[3]}
if (( $full_expr == 2 )); then
  # DEBUGGING
  kron_17_dataset=${kron_datasets[0]}
fi

for idx in "${all_idx[@]}"; do
  all_datasets+=(${dataset_filenames[$idx]})
done

for idx in "${kron_idx[@]}"; do
  kron_datasets+=($dataset_disk_loc/${dataset_filenames[$idx]})
done

for idx in "${corr_idx[@]}"; do
  corr_datasets+=($dataset_disk_loc/${dataset_filenames[$idx]})
done

echo 'Running CMake build'

runcmd mkdir -p build
runcmd cd build
runcmd cmake ..
runcmd cmake --build .

echo 'Finished running CMake build'

echo 'Building Aspen and Terrace!'
cd ../comparison_systems/aspen/code
(export CILK=1 ; make -j ingestion_test continuous_query_test)
cd -

cd ../comparison_systems/terrace/code
(export CILK=1 ; make -j ingestion_test continuous_query_test)
cd -

cp ../comparison_systems/terrace/code/ingestion_test terrace_ingest_expr
cp ../comparison_systems/terrace/code/continuous_query_test terrace_query_expr
cp ../comparison_systems/aspen/code/ingestion_test aspen_ingest_expr
cp ../comparison_systems/aspen/code/continuous_query_test aspen_query_expr

echo 'Finished building Aspen and Terrace'

echo 'Downloading datasets'

for dataset in "${all_datasets[@]}"; do
  runcmd wget -nc --tries=10 -O "$dataset_disk_loc/$dataset" "$dataset_link_dir/$dataset"
done

echo 'Finished downloading datasets'

echo 'Running experiments'

echo ${kron_datasets[3]}

runcmd mkdir $csv_directory 2> /dev/null

runcmd ../sketch_expr/run_sketch_expr.sh $csv_directory

runcmd ./speed_experiment yes $aspen_terrace_timeout $csv_directory ${kron_datasets[@]}

runcmd ./query_experiment yes $csv_directory kron_17_dataset

runcmd ../buffer_expr/run_buffer_expr.sh kron_17_dataset $csv_directory

runcmd ./parallel_experiment kron_17_dataset delme "$csv_directory/parallel_expr.csv" 46

runcmd ../cont_expr/run_correctness_test.sh $cont_expr_samples $cont_expr_runs $csv_directory ${corr_datasets[@]}

echo 'Finished running experiments'

# Get rid of top configuration
rm ~/.config/procps/toprc

# TODO: Put plotting code here

