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