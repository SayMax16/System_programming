#!/bin/bash

# Function to display usage information
usage() {
    echo "Usage: $0 <Your Name> <University Name> <Date of Birth>"
    echo "Example: $0 'John Doe' 'Central Asian University' '01/01/2000'"
    exit 1
}

# Function to validate date format (DD/MM/YYYY)
validate_date() {
    local date=$1
    if ! [[ $date =~ ^[0-9]{2}/[0-9]{2}/[0-9]{4}$ ]]; then
        echo "Error: Invalid date format. Please use DD/MM/YYYY"
        return 1
    fi
    
    # Extract day, month, and year
    local day=${date:0:2}
    local month=${date:3:2}
    local year=${date:6:4}
    
    # Validate ranges
    if [ $day -lt 1 ] || [ $day -gt 31 ]; then
        echo "Error: Invalid day (must be 01-31)"
        return 1
    fi
    if [ $month -lt 1 ] || [ $month -gt 12 ]; then
        echo "Error: Invalid month (must be 01-12)"
        return 1
    fi
    if [ $year -lt 1900 ] || [ $year -gt $(date +%Y) ]; then
        echo "Error: Invalid year (must be 1900-$(date +%Y))"
        return 1
    fi
    
    return 0
}

# Function to get system information
get_system_info() {
    echo "=== System Information ==="
    echo "Username: $(whoami)"
    echo "Hostname: $(hostname)"
    echo "Current Date: $(date '+%A, %B %d, %Y')"
    echo "Current Time: $(date '+%H:%M:%S %Z')"
    echo "Working Directory: $(pwd)"
    echo "Shell: $SHELL"
    echo "Kernel Version: $(uname -r)"
    echo "Memory Usage:"
    free -h 2>/dev/null || vm_stat 2>/dev/null || echo "Memory info not available"
    echo "Disk Usage:"
    df -h . | tail -n 1
}

# Function to display file information
display_file_info() {
    echo -e "\n=== File Information ==="
    echo "Files in current directory:"
    ls -li | while read -r line; do
        if [[ $line =~ ^[0-9]+ ]]; then
            inode=$(echo "$line" | awk '{print $1}')
            perms=$(echo "$line" | awk '{print $2}')
            size=$(echo "$line" | awk '{print $5}')
            date=$(echo "$line" | awk '{print $6, $7, $8}')
            name=$(echo "$line" | awk '{for(i=9;i<=NF;i++) printf $i" "; print ""}')
            printf "Inode: %-10s | Perms: %-10s | Size: %-8s | Date: %-15s | Name: %s\n" \
                   "$inode" "$perms" "$size" "$date" "$name"
        fi
    done
}

# Function to count and display sys files
count_sys_files() {
    local count=0
    local total_size=0
    
    echo -e "\n=== System Files Analysis ==="
    for file in sys*; do
        if [ -f "$file" ]; then
            count=$((count + 1))
            size=$(stat -f%z "$file" 2>/dev/null || stat -c%s "$file" 2>/dev/null)
            total_size=$((total_size + size))
            echo "File: $file"
            echo "  Size: $size bytes"
            echo "  Last Modified: $(stat -f%Sm "$file" 2>/dev/null || stat -c%y "$file" 2>/dev/null)"
            echo "  Content: $(cat "$file")"
            echo "---"
        fi
    done
    
    echo "Total sys files: $count"
    echo "Total size: $total_size bytes"
}

# Check number of arguments
if [ $# -ne 3 ]; then
    usage
fi

# Validate date format
if ! validate_date "$3"; then
    exit 1
fi

# Save arguments to files with timestamps
echo "Name: $1" > sys1
echo "Created: $(date '+%Y-%m-%d %H:%M:%S')" >> sys1

echo "University: $2" > sys2
echo "Created: $(date '+%Y-%m-%d %H:%M:%S')" >> sys2

echo "Date of Birth: $3" > sys3
echo "Created: $(date '+%Y-%m-%d %H:%M:%S')" >> sys3

# Display system information
get_system_info

# Display file information
display_file_info

# Count and display sys files
count_sys_files

echo -e "\nScript completed successfully!" 