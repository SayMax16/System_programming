#!/bin/bash

# Function to display usage
show_usage() {
    echo "Usage: $0 [-w] [-l]"
    echo "Options:"
    echo "  -w    Count words instead of characters"
    echo "  -l    Count lines instead of characters"
    echo "  -h    Show this help message"
}

# Default to character count
count_type="chars"

# Parse command line options
while getopts "wlh" opt; do
    case $opt in
        w) count_type="words" ;;
        l) count_type="lines" ;;
        h) show_usage; exit 0 ;;
        ?) show_usage; exit 1 ;;
    esac
done

# Get input from user
echo -n "Enter text (press Ctrl+D when done): "
text=$(cat)

# Count based on selected type
case $count_type in
    chars)
        count=$(echo -n "$text" | wc -m)
        echo "Character count: $count"
        ;;
    words)
        count=$(echo -n "$text" | wc -w)
        echo "Word count: $count"
        ;;
    lines)
        count=$(echo -n "$text" | wc -l)
        echo "Line count: $count"
        ;;
esac 