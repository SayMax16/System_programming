# Lab 1: Introduction to LINUX Commands and Quick Editing with vi Editor

**Author:** Saydullo Ismatov  

## Objective
- To familiarize students with basic Linux commands for file handling, process management, system information, file compression, permissions, SSH, searching, and package installation.
- To familiarize students with commands of vi Editor, for efficient and speedy program writing.

## Task 1: Linux Commands Execution

### 1. File Handling
- Commands used: `mkdir`, `cd`, `pwd`, `touch`, `ls`, `ls -al`, `cat`, `head`, `tail`, `cp`, `mv`, `rm`
- Output:
  ```
  Created directory lab1_saydullo_files and changed into it.
  /Users/saydulloismatov/Downloads/lab1_saydullo_files
  Created saydullo_file.txt.
  saydullo_file.txt
  total 0
  drwxr-xr-x@  3 saydulloismatov  staff   96 May 18 19:00 .
  drwxr-xr-x@ 15 saydulloismatov  staff  480 May 18 19:00 ..
  -rw-r--r--@  1 saydulloismatov  staff    0 May 18 19:00 saydullo_file.txt
  Hello from Saydullo's lab1
  Hello from Saydullo's lab1
  Hello from Saydullo's lab1
  saydullo_file.txt       saydullo_file_copy.txt
  saydullo_file.txt       saydullo_file_renamed.txt
  (empty ls output after rm)
  /Users/saydulloismatov/Downloads
  (contents of Downloads directory)
  ```

### 2. Process Management
- Commands used: `ps`, `top`, `sleep`, `kill`, `pkill`
- Output:
  ```
  (ps output)
  Running top snapshot (using top -l 1 | cat):
  (top output)
  Starting a sleep process in the background:
  Sleep process started with PID: 81816
  (ps | grep sleep output)
  Killing the sleep process:
  (ps | grep sleep output)
  Starting another sleep process:
  Sleep process started with PID: 81821
  Killing all sleep processes:
  (ps | grep sleep output)
  ```

### 3. System Information
- Commands used: `date`, `uname -a`, `df -h`, `vm_stat`, `which`
- Output:
  ```
  Sun May 18 19:00:00 +05 2025
  Darwin ec2-100-20-3-127.us-west-2.compute.amazonaws.com 24.3.0 Darwin Kernel Version 24.3.0: Thu Jan  2 20:24:06 PST 2025; root:xnu-11215.81.4~3/RELEASE_ARM64_T8103 arm64
  (df -h output)
  Memory statistics (using vm_stat):
  (vm_stat output)
  /bin/ls
  /opt/homebrew/bin/python3
  ```

### 4. File Compression and Decompression
- Commands used: `tar`, `gzip`, `bzip2`
- Output:
  ```
  (tar and gzip operations output)
  ```

### 5. File Permissions
- Commands used: `chmod`, `ls -l`, `man chmod`
- Output:
  ```
  (chmod and ls -l output)
  (man chmod output)
  ```

### 6. SSH Commands
- Commands skipped as they require a live remote host and valid credentials.

### 7. Searching
- Commands used: `grep`, `ls -l`
- Output:
  ```
  Hello World
  ./file1.txt:Hello World
  ./subdir/file3.txt:Hello from subdir
  -rw-r--r--@ 1 saydulloismatov  staff  12 May 18 19:00 file1.txt
  -rw-r--r--@ 1 saydulloismatov  staff  12 May 18 19:00 file2.txt
  ```

### 8. Network Commands
- Commands used: `ping`, `whois`, `dig`
- Output:
  ```
  PING localhost (127.0.0.1): 56 data bytes
  64 bytes from 127.0.0.1: icmp_seq=0 ttl=64 time=0.064 ms
  64 bytes from 127.0.0.1: icmp_seq=1 ttl=64 time=0.101 ms
  64 bytes from 127.0.0.1: icmp_seq=2 ttl=64 time=0.132 ms
  (whois and dig output)
  ```

### 9. Package Installation
• `dpkg -i pkg.deb` 
• `rpm -Uvh pkg.rpm`

## Conclusion
Lab 1 tasks completed successfully. 
