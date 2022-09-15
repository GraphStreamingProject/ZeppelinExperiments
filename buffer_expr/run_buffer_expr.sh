#! /bin/bash

kron17_loc=$1
csv_dir=$2

./buffersize_experiment $kron17_loc yes delme in_mem_buffer.csv

cgexec -g memory:8_GB ./buffersize_experiment $kron17_loc no delme disk_buffer.csv

echo "mem,buffer_size,percent_buffer_size,ingestion_rate\n" > $csv_dir/buffer_expr.csv
cat in_mem_buffer.csv >> $csv_dir/buffer_expr.csv
cat disk_buffer.csv >> $csv_dir/buffer_expr.csv

rm in_mem_buffer.csv
rm disk_buffer.csv
rm delme*
