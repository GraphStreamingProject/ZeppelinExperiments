
kron17_loc=$1
timeout=$2

echo "Running with timeout of $timeout hours"

# Run Aspen query test and store result in temp csv
echo ""
echo "ASPEN:"
echo "stream: $kron17_loc"
./aspen_query_expr $kron17_loc 1000000 1000000 $timeout temp_asp.csv 10

# Run Terrace query test and store result in temp csv
echo ""
echo "TERRACE:"
echo "stream: $kron17_loc"
./terrace_query_expr $kron17_loc 1000000 1000000 $timeout temp_ter.csv 10
