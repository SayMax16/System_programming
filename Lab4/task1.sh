#!/bin/bash

# Function to check if a number is even or odd using bitwise AND
check_even_odd() {
    local num=$1
    if (( (num & 1) == 0 )); then
        echo "$num is Even"
    else
        echo "$num is Odd"
    fi
}

# Function to calculate factorial using recursion
calculate_factorial() {
    local n=$1
    if (( n < 0 )); then
        echo "Factorial is not defined for negative numbers."
        return 1
    elif (( n == 0 )); then
        echo "Factorial of 0 is 1"
        return 0
    fi
    
    local result=1
    local i=1
    while (( i <= n )); do
        result=$((result * i))
        i=$((i + 1))
    done
    echo "Factorial of $n is $result"
}

# Main function to process both operations
process_numbers() {
    local num1=$1
    local num2=$2
    
    echo "Processing first number:"
    check_even_odd "$num1"
    
    echo -e "\nProcessing second number:"
    calculate_factorial "$num2"
}

# Test cases with different numbers
echo "Test Case 1:"
process_numbers 6 4

echo -e "\nTest Case 2:"
process_numbers 9 0

echo -e "\nTest Case 3:"
process_numbers 11 -3 