set -u

echo "/-------------------------------------------------\\"
echo "|     RUNNING SKETCH QUERY EXPERIMENT (1.5/9)     |"
echo "\\-------------------------------------------------/"

if [[ $# -ne 2 ]]; then
  echo "ERROR: Invalid Arguments!"
  echo "USAGE: run_sketch_query_expr.sh csv_dir prob"
  echo "csv_dir:  Directory where csv files will be placed"
  echo "prob:     Delta value that controls the failure probablilty of a sketch. (int or n)"
  echo "          (special value n indicates that prob should match the vector size)"
  exit 1
fi

csv_dir=$1
prob=$2

mkdir sketch_query_expr_results 2> /dev/null

if [ $prob == "n" ]; then
  echo "Delta = vector_size"

  echo "agm query native experiments ..."
  ./agm_query_native 20000 20000 > sketch_query_expr_results/agm_results
  ./agm_query_native 100000 100000 >> sketch_query_expr_results/agm_results
  ./agm_query_native 1000000 1000000 >> sketch_query_expr_results/agm_results
  ./agm_query_native 10000000 10000000 >> sketch_query_expr_results/agm_results
  ./agm_query_native 100000000 100000000 >> sketch_query_expr_results/agm_results
  ./agm_query_native 1000000000 1000000000 >> sketch_query_expr_results/agm_results

  echo "agm query non-native experiments"
  ./agm_query 10000000000 10000000000 >> sketch_query_expr_results/agm_results
  ./agm_query 100000000000 100000000000 >> sketch_query_expr_results/agm_results
  ./agm_query 1000000000000 1000000000000 >> sketch_query_expr_results/agm_results

  echo "cube sketch query experiments ..."
  ./cube_sketch_query 20000 20000 > sketch_query_expr_results/cube_results
  ./cube_sketch_query 100000 100000 >> sketch_query_expr_results/cube_results
  ./cube_sketch_query 1000000 1000000 >> sketch_query_expr_results/cube_results
  ./cube_sketch_query 10000000 10000000 >> sketch_query_expr_results/cube_results
  ./cube_sketch_query 100000000 100000000 >> sketch_query_expr_results/cube_results
  ./cube_sketch_query 1000000000 1000000000 >> sketch_query_expr_results/cube_results
  ./cube_sketch_query 10000000000 10000000000 >> sketch_query_expr_results/cube_results
  ./cube_sketch_query 100000000000 100000000000 >> sketch_query_expr_results/cube_results
  ./cube_sketch_query 1000000000000 1000000000000 >> sketch_query_expr_results/cube_results
else
  echo "Delta = $prob"

  echo "agm query native experiments ..."
  ./agm_query_native 20000 $prob > sketch_query_expr_results/agm_results
  ./agm_query_native 100000 $prob >> sketch_query_expr_results/agm_results
  ./agm_query_native 1000000 $prob >> sketch_query_expr_results/agm_results
  ./agm_query_native 10000000 $prob >> sketch_query_expr_results/agm_results
  ./agm_query_native 100000000 $prob >> sketch_query_expr_results/agm_results
  ./agm_query_native 1000000000 $prob >> sketch_query_expr_results/agm_results

  echo "agm query non-native experiments"
  ./agm_query 10000000000 $prob >> sketch_query_expr_results/agm_results
  ./agm_query 100000000000 $prob >> sketch_query_expr_results/agm_results
  ./agm_query 1000000000000 $prob >> sketch_query_expr_results/agm_results

  echo "cube sketch query experiments ..."
  ./cube_sketch_query 20000 $prob > sketch_query_expr_results/cube_results
  ./cube_sketch_query 100000 $prob >> sketch_query_expr_results/cube_results
  ./cube_sketch_query 1000000 $prob >> sketch_query_expr_results/cube_results
  ./cube_sketch_query 10000000 $prob >> sketch_query_expr_results/cube_results
  ./cube_sketch_query 100000000 $prob >> sketch_query_expr_results/cube_results
  ./cube_sketch_query 1000000000 $prob >> sketch_query_expr_results/cube_results
  ./cube_sketch_query 10000000000 $prob >> sketch_query_expr_results/cube_results
  ./cube_sketch_query 100000000000 $prob >> sketch_query_expr_results/cube_results
  ./cube_sketch_query 1000000000000 $prob >> sketch_query_expr_results/cube_results
fi

# ./combine_sketch_expr_results sketch_expr_results/agm_results sketch_expr_results/cube_results $csv_dir/sketch_space.csv $csv_dir/sketch_speed.csv

# get rid of sketch expr results
# rm -r sketch_query_expr_results
