#!/bin/bash


show_usage() {
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  -c    Monitor CPU usage"
    echo "  -m    Monitor memory usage"
    echo "  -d    Monitor disk usage"
    echo "  -a    Monitor all resources"
    echo "  -h    Show this help message"
}


get_cpu_usage() {
    echo "CPU Usage:"
    top -l 1 | grep "CPU usage" | sed 's/CPU usage: //'
}


get_memory_usage() {
    echo "Memory Usage:"
    vm_stat | perl -ne '/page size of (\d+)/ and $size=$1; /Pages free: (\d+)/ and printf "Free: %.2f GB\n", $1 * $size / 1073741824; /Pages active: (\d+)/ and printf "Active: %.2f GB\n", $1 * $size / 1073741824; /Pages inactive: (\d+)/ and printf "Inactive: %.2f GB\n", $1 * $size / 1073741824;'
}


get_disk_usage() {
    echo "Disk Usage:"
    df -h | grep -v "tmpfs" | awk 'NR>1 {print $1 ": " $5 " used (" $3 " of " $2 ")"}'
}


if [ $# -eq 0 ]; then
    show_usage
    exit 1
fi

while getopts "cmdah" opt; do
    case $opt in
        c)
            get_cpu_usage
            ;;
        m)
            get_memory_usage
            ;;
        d)
            get_disk_usage
            ;;
        a)
            echo "=== System Resource Monitor ==="
            echo
            get_cpu_usage
            echo
            get_memory_usage
            echo
            get_disk_usage
            ;;
        h)
            show_usage
            exit 0
            ;;
        ?)
            show_usage
            exit 1
            ;;
    esac
done 
