#!/bin/bash

# Lab 1: Introduction to LINUX Commands and Quick Editing with vi Editor
# Author: Saydullo Ismatov
# Date: $(date +"%d %B %Y")

echo "Lab 1: Introduction to LINUX Commands and Quick Editing with vi Editor"
echo "Author: Saydullo Ismatov"
echo "Date: $(date +"%d %B %Y")"
echo ""

# Task 1: Linux Commands Execution

echo "Task 1: Linux Commands Execution"
echo "--------------------------------"

# 1. File Handling
echo "1. File Handling:"
echo "----------------"
mkdir -p lab1_saydullo_files
cd lab1_saydullo_files
echo "Created directory lab1_saydullo_files and changed into it."
pwd
touch saydullo_file.txt
echo "Created saydullo_file.txt."
ls
ls -al
echo "Hello from Saydullo's lab1" > saydullo_file.txt
cat saydullo_file.txt
head -n 1 saydullo_file.txt
tail -n 1 saydullo_file.txt
cp saydullo_file.txt saydullo_file_copy.txt
ls
mv saydullo_file_copy.txt saydullo_file_renamed.txt
ls
rm saydullo_file.txt saydullo_file_renamed.txt
ls
cd ..
rm -r lab1_saydullo_files
echo "Cleaned up lab1_saydullo_files directory."
pwd
ls
echo ""

# 2. Process Management
echo "2. Process Management:"
echo "---------------------"
ps
echo "Running top snapshot (using top -l 1 | cat):"
top -l 1 | cat
echo "Starting a sleep process in the background:"
sleep 600 &
echo "Sleep process started with PID: $!"
ps | grep sleep
echo "Killing the sleep process:"
kill $!
ps | grep sleep
echo "Starting another sleep process:"
sleep 10 &
echo "Sleep process started with PID: $!"
echo "Killing all sleep processes:"
pkill sleep
ps | grep sleep
echo ""

# 3. System Information
echo "3. System Information:"
echo "---------------------"
date
uname -a
df -h
echo "Memory statistics (using vm_stat):"
vm_stat
which ls
which python3
echo ""

# 4. File Compression and Decompression
echo "4. File Compression and Decompression:"
echo "------------------------------------"
mkdir -p compression_saydullo
cd compression_saydullo
echo "Tar test content" > file_for_tar.txt
echo "Gzip test content" > file_for_gzip.txt
echo "Bzip2 test content" > file_for_bzip2.txt
tar cf saydullo_archive.tar file_for_tar.txt
ls
tar xf saydullo_archive.tar
ls
gzip file_for_gzip.txt
ls
gzip -d file_for_gzip.txt.gz
ls
tar cjf saydullo_archive.tar.bz2 file_for_bzip2.txt
ls
tar xjf saydullo_archive.tar.bz2
ls
cd ..
rm -r compression_saydullo
echo "Cleaned up compression_saydullo directory."
echo ""

# 5. File Permissions
echo "5. File Permissions:"
echo "-------------------"
mkdir -p permissions_saydullo
cd permissions_saydullo
touch saydullo_perms.txt
ls -l saydullo_perms.txt
chmod 777 saydullo_perms.txt
ls -l saydullo_perms.txt
chmod 644 saydullo_perms.txt
ls -l saydullo_perms.txt
man chmod | cat
cd ..
rm -r permissions_saydullo
echo "Cleaned up permissions_saydullo directory."
echo ""

# 6. SSH Commands
echo "6. SSH Commands:"
echo "---------------"
echo "SSH commands require a live remote host and valid credentials. Skipping execution."
echo ""

# 7. Searching
echo "7. Searching:"
echo "------------"
mkdir -p search_saydullo
cd search_saydullo
echo "Hello World" > file1.txt
echo "hello there" > file2.txt
mkdir subdir
echo "Hello from subdir" > subdir/file3.txt
grep "Hello" file1.txt
grep -r "Hello" .
ls -l | grep "file"
cd ..
rm -r search_saydullo
echo "Cleaned up search_saydullo directory."
echo ""

# 8. Network Commands
echo "8. Network Commands:"
echo "-------------------"
ping -c 3 localhost
whois google.com
dig google.com A
dig -x 8.8.8.8
echo "wget command not found on this system. It can be installed using Homebrew: brew install wget."
echo ""

# 9. Package Installation
echo "9. Package Installation:"
echo "----------------------"
echo "dpkg and rpm are specific to Linux distributions. On macOS, use Homebrew (brew) for package management."
echo ""

echo "Lab 1 tasks completed successfully." 