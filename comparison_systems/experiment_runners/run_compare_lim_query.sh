set -u

kron17_loc=$1
timeout=$2

# Run memory limited Aspen query test and store result in temp csv
echo ""
echo "ASPEN:"
echo "stream: $kron17_loc"
cgexec -g memory:12_GB ./aspen_query_expr $kron17_loc 1000000 1000000 $timeout temp_asp.csv 10
