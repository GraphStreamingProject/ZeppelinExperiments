
csv_dir=$1

mkdir sketch_expr_results 2> /dev/null

echo "agm native experiments ..."
./agm_sketch_native 1000 > sketch_expr_results/agm_results
./agm_sketch_native 10000 >> sketch_expr_results/agm_results
./agm_sketch_native 100000 >> sketch_expr_results/agm_results
./agm_sketch_native 1000000 >> sketch_expr_results/agm_results
./agm_sketch_native 10000000 >> sketch_expr_results/agm_results
./agm_sketch_native 100000000 >> sketch_expr_results/agm_results
./agm_sketch_native 1000000000 >> sketch_expr_results/agm_results

echo "agm non-native experiments ..."
./agm_sketch 10000000000 >> sketch_expr_results/agm_results
./agm_sketch 100000000000 >> sketch_expr_results/agm_results
./agm_sketch 1000000000000 >> sketch_expr_results/agm_results

echo "cube sketch experiments ..."
./cube_sketch 1000 > sketch_expr_results/cube_results
./cube_sketch 10000 >> sketch_expr_results/cube_results
./cube_sketch 100000 >> sketch_expr_results/cube_results
./cube_sketch 1000000 >> sketch_expr_results/cube_results
./cube_sketch 10000000 >> sketch_expr_results/cube_results
./cube_sketch 100000000 >> sketch_expr_results/cube_results
./cube_sketch 1000000000 >> sketch_expr_results/cube_results
./cube_sketch 10000000000 >> sketch_expr_results/cube_results
./cube_sketch 100000000000 >> sketch_expr_results/cube_results
./cube_sketch 1000000000000 >> sketch_expr_results/cube_results

./combine_sketch_expr_results sketch_expr_results/agm_results sketch_expr_results/cube_results $csv_dir/sketch_space.csv $csv_dir/sketch_speed.csv

# get rid of sketch expr results
rm -r sketch_expr_results
