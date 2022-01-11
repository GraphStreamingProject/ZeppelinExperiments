
mkdir sketch_expr_results

echo "agm native experiments ..."
./agm_sketch_native 1000 > agm_native_results
./agm_sketch_native 10000 >> agm_native_results
./agm_sketch_native 100000 >> agm_native_results
./agm_sketch_native 1000000 >> agm_native_results
./agm_sketch_native 10000000 >> agm_native_results
./agm_sketch_native 100000000 >> agm_native_results
./agm_sketch_native 1000000000 >> agm_native_results

echo "agm non-native experiments ..."
./agm_sketch 1000 > agm_results
./agm_sketch 10000 >> agm_results
./agm_sketch 100000 >> agm_results
./agm_sketch 1000000 >> agm_results
./agm_sketch 10000000 >> agm_results
./agm_sketch 100000000 >> agm_results
./agm_sketch 1000000000 >> agm_results
./agm_sketch 10000000000 >> agm_results
./agm_sketch 100000000000 >> agm_results
./agm_sketch 1000000000000 >> agm_results

echo "cube sketch experiments ..."
./cube_sketch 1000 > cube_results
./cube_sketch 10000 >> cube_results
./cube_sketch 100000 >> cube_results
./cube_sketch 1000000 >> cube_results
./cube_sketch 10000000 >> cube_results
./cube_sketch 100000000 >> cube_results
./cube_sketch 1000000000 >> cube_results
./cube_sketch 10000000000 >> cube_results
./cube_sketch 100000000000 >> cube_results
./cube_sketch 1000000000000 >> cube_results
