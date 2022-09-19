#! /bin/bash
set -u

# USAGE ./run_correctnes_test.sh num_samples num_runs csv_directory input_files[+]

# Extract number of samples and number of runs
samples=$1
shift 1
runs=$1
shift 1
csv_dir=$1
shift 1


echo "/-------------------------------------------------\\"
echo "|       RUNNING CORRECTNESS EXPERIMENT (9/9)      |"
echo "\\-------------------------------------------------/"

echo "dataset,total_samples,failures" > $csv_dir/correctness.csv

for input in $@
do
  echo "Checking correcness upon stream: $input"

  ./cont_test $input $samples $runs temp_out.csv

  cat temp_out.csv >> $csv_dir/correctness.csv
  rm temp_out.csv
done
