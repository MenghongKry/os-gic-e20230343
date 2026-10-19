#!/bin/bash
# os_layers.sh - prints the Task 4 OS layers diagram using real values from this machine
CPU=$(lscpu | sed -n 's/^Model name:[[:space:]]*//p' | head -1)
case "$CPU" in ""|"-") CPU="virtual $(lscpu | sed -n 's/^Vendor ID:[[:space:]]*//p' | head -1) CPU (host: Apple silicon)";; esac
CORES=$(nproc)
ARCH=$(uname -m)
MEM=$(awk '/MemTotal/ {printf "%.1f GiB (%s kB)", $2/1048576, $2}' /proc/meminfo)
KREL=$(uname -r)
KVER=$(uname -v | cut -c1-40)
MODS=$(lsmod 2>/dev/null | awk 'NR>1 {print $1}' | head -3 | paste -sd, - | sed 's/,/, /g')
[ -z "$MODS" ] && MODS="(none loaded)"
SH=$(ps -o comm= -p $PPID)

line() { printf '| %-72s |\n' "$1"; }
bar()  { printf '+%s+\n' "$(printf '%.0s-' $(seq 1 74))"; }

bar
line "USER SPACE  (user mode)"
line "  shell: $SH (PID $PPID)"
line "  my programs: file_creator, file_reader, dir_list (lib + sys versions)"
line "  libc (fopen, fprintf, printf, opendir) -> buffers, then calls below"
bar
line "SYSTEM CALL INTERFACE  (user -> kernel mode switch)"
line "  openat()  read()  write()  close()  getdents64()  newfstatat()  brk()"
bar
line "KERNEL  (kernel mode)  Linux $KREL"
line "  build: $KVER"
line "  scheduler, memory manager, VFS + filesystems (ext4, procfs), drivers"
line "  loaded modules: $MODS"
bar
line "HARDWARE"
line "  CPU: $CPU ($CORES cores, $ARCH)"
line "  RAM: $MEM"
line "  disk, network card, console"
bar
