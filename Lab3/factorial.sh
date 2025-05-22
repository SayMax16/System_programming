#!/bin/bash

#  recursively
factorial_recursive() {
    local n=$1
    if [ $n -eq 0 ] || [ $n -eq 1 ]; then
        echo 1
    else
        local prev=$(factorial_recursive $((n-1)))
        echo $((n * prev))
    fi
}

# iteratively
factorial_iterative() {
    local n=$1
    local result=1
    for ((i=1; i<=n; i++)); do
        result=$((result * i))
    done
    echo $result
}

# Function to display usage
show_usage() {
    echo "Usage: $0 [options] [number]"
    echo "Options:"
    echo "  -r    Use recursive calculation"
    echo "  -i    Use iterative calculation (default)"
    echo "  -h    Show this help message"
}


method="iterative"


while getopts "rih" opt; do
    case $opt in
        r) method="recursive" ;;
        i) method="iterative" ;;
        h) show_usage; exit 0 ;;
        ?) show_usage; exit 1 ;;
    esac
done


if [ -n "${!OPTIND}" ]; then
    num=${!OPTIND}
else
    echo -n "Enter a number: "
    read num
fi


if ! [[ "$num" =~ ^[0-9]+$ ]] || [ "$num" -lt 0 ]; then
    echo "Error: Please enter a non-negative integer."
    exit 1
fi


if [ "$method" = "recursive" ]; then
    result=$(factorial_recursive $num)
    echo "Factorial of $num (recursive) = $result"
else
    result=$(factorial_iterative $num)
    echo "Factorial of $num (iterative) = $result"
fi 
