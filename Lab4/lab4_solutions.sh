#!/bin/bash

# ============================================================================
# LAB 4: Shell Scripting Solutions
# Author: Saydullo Ismatov (ID: 210057)
# ============================================================================

# Function to display task description
show_task_description() {
    local task_num=$1
    echo -e "\n\033[1;34m=== Task $task_num ===\033[0m"
    case $task_num in
        1) echo "Task 1: Checking Even/Odd and Calculating Factorial
   - Takes two numbers as input
   - Checks if first number is even/odd using bitwise operations
   - Calculates factorial of second number using iteration
   - Demonstrates function usage and arithmetic operations"
        ;;
        2) echo "Task 2: Using AND and OR Lists
   - Creates and manages files using AND/OR lists
   - Demonstrates file operations and pattern searching
   - Shows proper error handling and cleanup
   - Uses complex conditional execution"
        ;;
        3) echo "Task 3: Sum of Digits in a Number
   - Calculates sum of digits using string manipulation
   - Implements input validation
   - Uses interactive loop with quit option
   - Demonstrates string operations and loops"
        ;;
        4) echo "Task 4: Finding the Greatest Number
   - Takes three numbers as input
   - Validates numeric input
   - Finds greatest number using array operations
   - Shows array handling and comparison"
        ;;
        5) echo "Task 5: File Comparison and Deletion
   - Compares two files for identity
   - Checks file sizes before content comparison
   - Handles file deletion based on comparison
   - Demonstrates file operations and error handling"
        ;;
    esac
    echo -e "\033[1;34m====================\033[0m\n"
}

# ============================================================================
# Task 1: Even/Odd and Factorial
# ============================================================================

task1_function() {
    local num1=$1
    local num2=$2
    
    # Check even/odd using bitwise AND
    if (( (num1 & 1) == 0 )); then
        echo "$num1 is Even"
    else
        echo "$num1 is Odd"
    fi
    
    # Calculate factorial
    if (( num2 < 0 )); then
        echo "Factorial is not defined for negative numbers."
    elif (( num2 == 0 )); then
        echo "Factorial of 0 is 1"
    else
        local result=1
        local i=1
        while (( i <= num2 )); do
            result=$((result * i))
            i=$((i + 1))
        done
        echo "Factorial of $num2 is $result"
    fi
}

# ============================================================================
# Task 2: AND and OR Lists
# ============================================================================

task2_function() {
    local filename="test_file.txt"
    local content="This is a test file.
It contains the word hello.
Another line for testing."
    
    # Create and populate file
    [ ! -f "$filename" ] && {
        echo "$content" > "$filename" && echo "Created and populated '$filename'"
    } || {
        echo "File '$filename' already exists"
        return 1
    }
    
    # Search for pattern
    [ -f "$filename" ] && {
        echo "Searching for 'hello' in '$filename':"
        grep -q "hello" "$filename" && {
            grep "hello" "$filename"
            return 0
        } || {
            echo "Pattern 'hello' not found"
            return 1
        }
    }
    
    # Cleanup
    [ -f "$filename" ] && rm "$filename" && echo "File removed"
}

# ============================================================================
# Task 3: Sum of Digits
# ============================================================================

task3_function() {
    local number=$1
    local sum=0
    
    # Validate input
    if [[ ! "$number" =~ ^[0-9]+$ ]]; then
        echo "Error: Input must be a non-negative integer"
        return 1
    fi
    
    # Calculate sum using string manipulation
    for (( i=0; i<${#number}; i++ )); do
        digit=${number:$i:1}
        sum=$((sum + digit))
    done
    
    echo "Sum of digits in $number is: $sum"
}

# ============================================================================
# Task 4: Greatest Number
# ============================================================================

task4_function() {
    local numbers=("$@")
    
    # Validate input
    if [ ${#numbers[@]} -ne 3 ]; then
        echo "Error: Please provide exactly three numbers"
        return 1
    fi
    
    # Validate each number
    for num in "${numbers[@]}"; do
        if [[ ! "$num" =~ ^-?[0-9]+$ ]]; then
            echo "Error: '$num' is not a valid integer"
            return 1
        fi
    done
    
    # Find greatest number
    local greatest=${numbers[0]}
    for num in "${numbers[@]}"; do
        if (( num > greatest )); then
            greatest=$num
        fi
    done
    
    echo "The greatest number is: $greatest"
}

# ============================================================================
# Task 5: File Comparison
# ============================================================================

task5_function() {
    local file1=$1
    local file2=$2
    
    # Validate files
    if [ ! -f "$file1" ] || [ ! -f "$file2" ]; then
        [ ! -f "$file1" ] && echo "Error: File '$file1' does not exist"
        [ ! -f "$file2" ] && echo "Error: File '$file2' does not exist"
        return 1
    fi
    
    # Compare file sizes
    local size1=$(stat -f%z "$file1" 2>/dev/null || stat -c%s "$file1")
    local size2=$(stat -f%z "$file2" 2>/dev/null || stat -c%s "$file2")
    
    if [ "$size1" != "$size2" ]; then
        echo "Files have different sizes:"
        echo "'$file1': $size1 bytes"
        echo "'$file2': $size2 bytes"
        return 1
    fi
    
    # Compare content
    if cmp -s "$file1" "$file2"; then
        echo "Files are identical"
        echo "Removing second file: '$file2'"
        rm "$file2" && echo "File removed successfully" || echo "Failed to remove file"
        return 0
    else
        echo "Files have same size but different content"
        return 1
    fi
}

# ============================================================================
# Main Menu
# ============================================================================

while true; do
    echo -e "\n\033[1;32m=== Lab 4 Shell Scripting Solutions ===\033[0m"
    echo "1. Task 1: Even/Odd and Factorial"
    echo "2. Task 2: AND and OR Lists"
    echo "3. Task 3: Sum of Digits"
    echo "4. Task 4: Greatest Number"
    echo "5. Task 5: File Comparison"
    echo "q. Quit"
    echo -n "Select a task (1-5) or 'q' to quit: "
    
    read choice
    
    case $choice in
        1)
            show_task_description 1
            echo -n "Enter first number: "
            read num1
            echo -n "Enter second number: "
            read num2
            task1_function "$num1" "$num2"
            ;;
        2)
            show_task_description 2
            task2_function
            ;;
        3)
            show_task_description 3
            echo -n "Enter a number: "
            read num
            task3_function "$num"
            ;;
        4)
            show_task_description 4
            echo -n "Enter three numbers separated by spaces: "
            read -a nums
            task4_function "${nums[@]}"
            ;;
        5)
            show_task_description 5
            echo -n "Enter first filename: "
            read file1
            echo -n "Enter second filename: "
            read file2
            task5_function "$file1" "$file2"
            ;;
        q|Q)
            echo "Exiting program..."
            exit 0
            ;;
        *)
            echo "Invalid choice. Please try again."
            ;;
    esac
done 