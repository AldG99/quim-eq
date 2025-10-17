#!/bin/bash

echo "=== Chemical Equation Balancer - Example Tests ==="
echo ""

# Array of test equations
equations=(
    "H2 + O2 -> H2O"
    "CH4 + O2 -> CO2 + H2O"
    "C6H12O6 + O2 -> CO2 + H2O"
    "Fe + O2 -> Fe2O3"
    "NH3 + O2 -> NO + H2O"
    "C2H6 + O2 -> CO2 + H2O"
    "Al + HCl -> AlCl3 + H2"
    "CaCO3 + HCl -> CaCl2 + CO2 + H2O"
    "Na + H2O -> NaOH + H2"
    "Mg + N2 -> Mg3N2"
)

passed=0
total=${#equations[@]}

echo "Testing $total equations:"
echo ""

for eq in "${equations[@]}"; do
    echo "Testing: $eq"
    if ./chemical_balancer "$eq" > /dev/null 2>&1; then
        echo "✅ PASSED"
        ((passed++))
    else
        echo "❌ FAILED"
    fi
    echo ""
done

echo "=== Test Summary ==="
echo "Passed: $passed/$total"
echo "Success rate: $(( passed * 100 / total ))%"

if [ $passed -eq $total ]; then
    echo "🎉 All tests passed!"
    exit 0
else
    echo "⚠️  Some tests failed"
    exit 1
fi