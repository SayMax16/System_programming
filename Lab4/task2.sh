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