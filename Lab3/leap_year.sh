#!/bin/bash

is_leap_year() {
    local year=$1
    if (( year % 4 == 0 )); then
        if (( year % 100 == 0 )); then
            if (( year % 400 == 0 )); then
                return 0
            else
                return 1
            fi
        else
            return 0
        fi
    else
        return 1
    fi
}


show_usage() {
    echo "Usage: $0 [year] or $0 -r [start_year] [end_year]"
    echo "Options:"
    echo "  -r    Check a range of years"
    echo "  -h    Show this help message"
}


if [ "$1" = "-r" ]; then
    if [ -z "$2" ] || [ -z "$3" ]; then
        echo "Error: Start and end years required for range check"
        show_usage
        exit 1
    fi
    start_year=$2
    end_year=$3
    
    echo "Checking leap years between $start_year and $end_year:"
    for (( year=start_year; year<=end_year; year++ )); do
        if is_leap_year $year; then
            echo "$year is a leap year"
        fi
    done
elif [ "$1" = "-h" ]; then
    show_usage
    exit 0
elif [ -n "$1" ]; then
    year=$1
    if is_leap_year $year; then
        echo "$year is a leap year"
    else
        echo "$year is not a leap year"
    fi
else
    echo -n "Enter a year: "
    read year
    if is_leap_year $year; then
        echo "$year is a leap year"
    else
        echo "$year is not a leap year"
    fi
fi 
