
kron17_loc=$1

echo "Running unlim query experiment on $kron17_loc"

# Run Aspen query test and store result in temp csv
echo ""
echo "ASPEN"
./aspen_query_expr $kron17_loc 1000000 1000000 temp_asp.csv 10

# Run Terrace query test and store result in temp csv
echo ""
echo "TERRACE"
./terrace_query_expr $kron17_loc 1000000 1000000 temp_ter.csv 10
