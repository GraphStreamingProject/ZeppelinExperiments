
kron17_loc=$1

echo "Running lim query experiment on $kron17_loc"

# Run memory limited Aspen query test and store result in temp csv
echo ""
echo "ASPEN"
cgexec -g memory:12_GB ./aspen_query_expr $kron17_loc 1000000 1000000 temp_asp.csv 10
