# Lab 4: Shell Scripting Solutions
**Author:** Saydullo Ismatov (ID: 210057)

## Overview
This report documents the implementation and testing of five shell scripting tasks that demonstrate various aspects of shell programming, including arithmetic operations, file handling, string manipulation, and control structures.

## Task 1: Even/Odd and Factorial
### Description
This task implements two functions:
1. A function to check if a number is even or odd using bitwise operations
2. A function to calculate the factorial of a number using iteration

### Implementation
```bash
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
```

### Output
```
Test Case 1:
Processing first number:
6 is Even

Processing second number:
Factorial of 4 is 24

Test Case 2:
Processing first number:
9 is Odd

Processing second number:
Factorial of 0 is 1

Test Case 3:
Processing first number:
11 is Odd

Processing second number:
Factorial is not defined for negative numbers.
```

## Task 2: AND and OR Lists
### Description
This task demonstrates the use of AND (`&&`) and OR (`||`) lists in shell scripting to:
1. Create and populate a file
2. Search for a pattern in the file
3. Clean up the file after operations

### Implementation
```bash
#!/bin/bash

# Function to create and populate a file
create_and_populate_file() {
    local filename=$1
    local content=$2
    
    # Create file if it doesn't exist and write content
    [ ! -f "$filename" ] && {
        echo "$content" > "$filename" && echo "Created and populated '$filename'"
    } || {
        echo "File '$filename' already exists"
        return 1
    }
}

# Function to search and display content
search_and_display() {
    local filename=$1
    local pattern=$2
    
    # Check if file exists and contains the pattern
    [ -f "$filename" ] && {
        echo "Searching for '$pattern' in '$filename':"
        grep -q "$pattern" "$filename" && {
            grep "$pattern" "$filename"
            return 0
        } || {
            echo "Pattern '$pattern' not found in '$filename'"
            return 1
        }
    } || {
        echo "File '$filename' does not exist"
        return 1
    }
}

# Main script execution
FILENAME="data.txt"
SEARCH_PATTERN="hello"

# Create and populate the file
create_and_populate_file "$FILENAME" "This is a test file.
It contains the word hello.
Another line for testing.
Last line of the file."

# Search for pattern
search_and_display "$FILENAME" "$SEARCH_PATTERN"

# Clean up
[ -f "$FILENAME" ] && {
    rm "$FILENAME" && echo "File '$FILENAME' removed successfully"
} || echo "No file to remove"
```

### Output
```
Created and populated 'data.txt'
Searching for 'hello' in 'data.txt':
It contains the word hello.
File 'data.txt' removed successfully
```

## Task 3: Sum of Digits
### Description
This task calculates the sum of digits in a number using string manipulation.

### Implementation
```bash
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
```

### Output
```
Sum of Digits Calculator
----------------------
Enter a number (or 'q' to quit): 12345
Sum of digits in 12345 is: 15
----------------------
Enter a number (or 'q' to quit): abc
Error: Input must be a non-negative integer
Enter a number (or 'q' to quit): q
Program terminated.
```

## Task 4: Greatest Number
### Description
This task finds the greatest number among three user-input numbers.

### Implementation
```bash
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
```

### Output
```
Greatest Number Finder
--------------------
Enter three numbers separated by spaces (or 'q' to quit): 10 25 15
The greatest number is: 25
--------------------
Enter three numbers separated by spaces (or 'q' to quit): 5 5 5
The greatest number is: 5
--------------------
Enter three numbers separated by spaces (or 'q' to quit): 1 2
Error: Please enter exactly three numbers
Enter three numbers separated by spaces (or 'q' to quit): q
Program terminated.
```

## Task 5: File Comparison
### Description
This task compares two files and handles them based on their content similarity.

### Implementation
```bash
#!/bin/bash

# Function to validate file existence
validate_files() {
    local file1=$1
    local file2=$2
    
    # Check if both files exist
    if [ ! -f "$file1" ] || [ ! -f "$file2" ]; then
        [ ! -f "$file1" ] && echo "Error: File '$file1' does not exist"
        [ ! -f "$file2" ] && echo "Error: File '$file2' does not exist"
        return 1
    fi
    return 0
}

# Function to compare files and handle deletion
compare_and_handle() {
    local file1=$1
    local file2=$2
    
    # Get file sizes
    local size1=$(stat -f%z "$file1" 2>/dev/null || stat -c%s "$file1")
    local size2=$(stat -f%z "$file2" 2>/dev/null || stat -c%s "$file2")
    
    # Compare file sizes first
    if [ "$size1" != "$size2" ]; then
        echo "Files have different sizes:"
        echo "'$file1': $size1 bytes"
        echo "'$file2': $size2 bytes"
        return 1
    fi
    
    # If sizes match, compare content
    if cmp -s "$file1" "$file2"; then
        echo "Files are identical in content"
        echo "Removing second file: '$file2'"
        rm "$file2" && echo "File removed successfully" || echo "Failed to remove file"
        return 0
    else
        echo "Files have same size but different content"
        return 1
    fi
}

# Main program
if [ $# -ne 2 ]; then
    echo "Usage: $0 <file1> <file2>"
    exit 1
fi

# Validate files
validate_files "$1" "$2" || exit 1

# Compare and handle files
compare_and_handle "$1" "$2"
```

### Output
```
# Test Case 1: Identical files
$ ./task5.sh file1.txt file2.txt
Files are identical in content
Removing second file: 'file2.txt'
File removed successfully

# Test Case 2: Different sizes
$ ./task5.sh file1.txt file3.txt
Files have different sizes:
'file1.txt': 100 bytes
'file3.txt': 150 bytes

# Test Case 3: Same size, different content
$ ./task5.sh file1.txt file4.txt
Files have same size but different content
```

## Conclusion
This lab demonstrates various aspects of shell scripting, including:
- Arithmetic and bitwise operations
- File handling and management
- String manipulation
- Array operations
- Error handling
- User input validation
- Control structures

Each task is implemented with proper error handling, input validation, and clear output formatting, making the scripts robust and user-friendly. The actual outputs show how these scripts behave in a real Unix/Linux environment, demonstrating their functionality and error handling capabilities. 