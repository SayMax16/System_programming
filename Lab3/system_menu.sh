#!/bin/bash


show_menu() {
    clear
    echo "=== System Management Menu ==="
    echo "1. Process Management"
    echo "2. File Operations"
    echo "3. System Information"
    echo "4. Network Status"
    echo "5. User Management"
    echo "6. Exit"
    echo
    echo -n "Enter your choice (1-6): "
}


process_menu() {
    clear
    echo "=== Process Management ==="
    echo "1. List all processes"
    echo "2. Find process by name"
    echo "3. Kill process by PID"
    echo "4. Back to main menu"
    echo
    echo -n "Enter your choice (1-4): "
    read choice
    
    case $choice in
        1) ps aux | less ;;
        2)
            echo -n "Enter process name: "
            read proc_name
            ps aux | grep -i "$proc_name" | grep -v grep
            ;;
        3)
            echo -n "Enter PID to kill: "
            read pid
            kill -9 $pid 2>/dev/null || echo "Failed to kill process"
            ;;
        4) return ;;
        *) echo "Invalid choice" ;;
    esac
    read -p "Press Enter to continue..."
}


file_menu() {
    clear
    echo "=== File Operations ==="
    echo "1. List files in current directory"
    echo "2. Show file contents"
    echo "3. Create new file"
    echo "4. Back to main menu"
    echo
    echo -n "Enter your choice (1-4): "
    read choice
    
    case $choice in
        1) ls -la ;;
        2)
            echo -n "Enter filename: "
            read filename
            if [ -f "$filename" ]; then
                cat "$filename" | less
            else
                echo "File not found"
            fi
            ;;
        3)
            echo -n "Enter filename: "
            read filename
            touch "$filename"
            echo "File created: $filename"
            ;;
        4) return ;;
        *) echo "Invalid choice" ;;
    esac
    read -p "Press Enter to continue..."
}


system_info() {
    clear
    echo "=== System Information ==="
    echo "Hostname: $(hostname)"
    echo "OS: $(uname -s)"
    echo "Kernel: $(uname -r)"
    echo "CPU: $(sysctl -n machdep.cpu.brand_string)"
    echo "Memory: $(vm_stat | grep "Pages free" | awk '{print $3}') pages free"
    echo "Disk Usage:"
    df -h | grep -v "tmpfs"
    read -p "Press Enter to continue..."
}


network_status() {
    clear
    echo "=== Network Status ==="
    echo "Network Interfaces:"
    ifconfig | grep "inet " | grep -v 127.0.0.1
    echo
    echo "Active Connections:"
    netstat -an | grep LISTEN
    read -p "Press Enter to continue..."
}


user_menu() {
    clear
    echo "=== User Management ==="
    echo "1. List users"
    echo "2. Show current user"
    echo "3. Show user groups"
    echo "4. Back to main menu"
    echo
    echo -n "Enter your choice (1-4): "
    read choice
    
    case $choice in
        1) cat /etc/passwd | cut -d: -f1 ;;
        2) whoami ;;
        3) groups ;;
        4) return ;;
        *) echo "Invalid choice" ;;
    esac
    read -p "Press Enter to continue..."
}

# Main menu loop
while true; do
    show_menu
    read choice
    
    case $choice in
        1) process_menu ;;
        2) file_menu ;;
        3) system_info ;;
        4) network_status ;;
        5) user_menu ;;
        6) echo "Goodbye!"; exit 0 ;;
        *) echo "Invalid choice" ;;
    esac
done 
