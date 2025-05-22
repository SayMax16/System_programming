#!/bin/bash

# Function to validate input
validate_input() {
    local input=$1
    if [[ ! "$input" =~ ^[0-9]+$ ]]; then
        echo "Error: Input must be a non-negative integer"
        return 1
    fi
    return 0
}

# Function to calculate sum of digits using string manipulation
calculate_digit_sum() {
    local number=$1
    local sum=0
    
    # Convert number to string and iterate through each digit
    for (( i=0; i<${#number}; i++ )); do
        digit=${number:$i:1}
        sum=$((sum + digit))
    done
    
    echo $sum
}

# Main program
echo "Sum of Digits Calculator"
echo "----------------------"

while true; do
    echo -n "Enter a number (or 'q' to quit): "
    read input
    
    # Check if user wants to quit
    [[ "$input" == "q" ]] && break
    
    # Validate input
    validate_input "$input" || continue
    
    # Calculate and display result
    result=$(calculate_digit_sum "$input")
    echo "Sum of digits in $input is: $result"
    echo "----------------------"
done

echo "Program terminated." 