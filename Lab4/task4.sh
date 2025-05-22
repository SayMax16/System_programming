#!/bin/bash

# Function to validate numeric input
validate_numbers() {
    local nums=("$@")
    for num in "${nums[@]}"; do
        if [[ ! "$num" =~ ^-?[0-9]+$ ]]; then
            echo "Error: '$num' is not a valid integer"
            return 1
        fi
    done
    return 0
}

# Function to find greatest number using array sorting
find_greatest() {
    local numbers=("$@")
    local greatest=${numbers[0]}
    
    # Iterate through array to find greatest
    for num in "${numbers[@]}"; do
        if (( num > greatest )); then
            greatest=$num
        fi
    done
    
    echo $greatest
}

# Main program
echo "Greatest Number Finder"
echo "--------------------"

while true; do
    echo -n "Enter three numbers separated by spaces (or 'q' to quit): "
    read -a numbers
    
    # Check if user wants to quit
    [[ "${numbers[0]}" == "q" ]] && break
    
    # Check if exactly three numbers were entered
    if [ ${#numbers[@]} -ne 3 ]; then
        echo "Error: Please enter exactly three numbers"
        continue
    fi
    
    # Validate numbers
    validate_numbers "${numbers[@]}" || continue
    
    # Find and display greatest number
    greatest=$(find_greatest "${numbers[@]}")
    echo "The greatest number is: $greatest"
    echo "--------------------"
done

echo "Program terminated." 