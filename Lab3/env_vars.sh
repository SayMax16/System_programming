#!/bin/bash


show_usage() {
    echo "Usage: $0 [options] [variable_name]"
    echo "Options:"
    echo "  -l    List all environment variables"
    echo "  -s    Search for variables containing a pattern"
    echo "  -h    Show this help message"
}


show_var_value() {
    local var_name=$1
    local value=${!var_name}
    if [ -n "$value" ]; then
        echo -e "\e[32m$var_name\e[0m = \e[34m$value\e[0m"
    else
        echo -e "\e[31mVariable '$var_name' not found or empty\e[0m"
    fi
}


while getopts "lsh" opt; do
    case $opt in
        l) 
            echo "All Environment Variables:"
            printenv | sort
            exit 0
            ;;
        s)
            if [ -z "$2" ]; then
                echo "Error: Search pattern required"
                show_usage
                exit 1
            fi
            echo "Searching for variables containing '$2':"
            printenv | grep -i "$2"
            exit 0
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


if [ -n "$1" ]; then
    show_var_value "$1"
else
    echo "Common Environment Variables:"
    for var in HOME USER PATH SHELL TERM LANG; do
        show_var_value "$var"
    done
fi 
