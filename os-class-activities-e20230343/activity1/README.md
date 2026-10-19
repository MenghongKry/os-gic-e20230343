# Class Activity 1 — System Calls in Practice

- **Student Name:** [Your Name Here]
- **Student ID:** e20230343
- **Date:** [Date of Submission]

---

## Warm-Up: Hello System Call

Screenshot of running `hello_syscall.c` on Linux:

![Hello syscall](screenshots/hello_syscall.png)

Screenshot of running `hello_winapi.c` on Windows (CMD/PowerShell/VS Code):

> Not run: I work on a MacBook (Apple silicon) with an Ubuntu VM in Multipass and have no Windows machine, so the WinAPI program cannot be compiled and run natively. The source is included in `warmup/hello_winapi.c`.

Screenshot of running `copyfilesyscall.c` on Linux:

![Copy file syscall](screenshots/copyfilesyscall.png)

---

## Task 1: File Creator & Reader

### Part A — File Creator

**Describe your implementation:** The system call version does by hand what `fopen`/`fprintf`/`printf` hide: I call `open()` with explicit flags and permissions, `write()` the text with its exact byte length (`strlen`), `close()` the descriptor, and print the confirmation with `write(1, ...)`. There is no `FILE*`, no buffering and no format string; errors are written to fd 2 myself instead of using `perror()`. Both versions print the same line and create the same 36-byte `output.txt`.

**Version A — Library Functions (`file_creator_lib.c`):**

<!-- Screenshot: gcc -o file_creator_lib file_creator_lib.c && ./file_creator_lib && cat output.txt -->
![Task 1A - Library](screenshots/task1_creator_lib.png)

**Version B — POSIX System Calls (`file_creator_sys.c`):**

<!-- Screenshot: gcc -o file_creator_sys file_creator_sys.c && ./file_creator_sys && cat output.txt -->
![Task 1A - Syscall](screenshots/task1_creator_sys.png)

**Questions:**

1. **What flags did you pass to `open()`? What does each flag mean?**

   > `O_WRONLY | O_CREAT | O_TRUNC`. `O_WRONLY` opens the file for writing only. `O_CREAT` creates the file if it does not exist (this is why the third `mode` argument is needed). `O_TRUNC` empties the file to length 0 if it already exists, so old content is overwritten.

2. **What is `0644`? What does each digit represent?**

   > It is the permission mode in octal. The leading `0` marks it as octal. `6` = owner read+write (4+2), `4` = group read only, `4` = others read only, i.e. `rw-r--r--`. The final permissions are also filtered by the process `umask`.

3. **What does `fopen("output.txt", "w")` do internally that you had to do manually?**

   > It translates the mode string `"w"` into the flags `O_WRONLY|O_CREAT|O_TRUNC`, picks the permission `0666` (the umask turns it into `0644`), calls `open()`/`openat()`, and wraps the returned file descriptor in a `FILE` structure with a user-space buffer. `fprintf` then formats into that buffer and `fclose` flushes it with `write()` and calls `close()`. With system calls I had to choose the flags and mode, compute the byte count, and call `write()`/`close()` myself. My strace shows this: `openat(..., O_WRONLY|O_CREAT|O_TRUNC, 0666)` for the library version vs `0644` for mine.

### Part B — File Reader & Display

**Describe your implementation:** I open `output.txt` with `O_RDONLY`, then loop: `read()` up to 256 bytes into a buffer and `write()` exactly the number of bytes returned to fd 1. The loop stops when `read()` returns 0 (end of file) or -1 (error), then I `close()` the file. Unlike `fgets`/`printf`, the data is not treated as strings or lines, just raw bytes.

**Version A — Library Functions (`file_reader_lib.c`):**

![Task 1B - Library](screenshots/task1_reader_lib.png)

**Version B — POSIX System Calls (`file_reader_sys.c`):**

![Task 1B - Syscall](screenshots/task1_reader_sys.png)

**Questions:**

1. **What does `read()` return? How is this different from `fgets()`?**

   > `read()` returns the number of bytes actually read (a `ssize_t`), `0` at end of file, or `-1` on error. It reads raw bytes and does not stop at a newline or add a `'\0'`. `fgets()` is a library function that reads from a buffered `FILE*` up to and including a newline (or until the buffer is full), adds a terminating `'\0'`, and returns a pointer to the buffer or `NULL` at EOF/error.

2. **Why do you need a loop when using `read()`? When does it stop?**

   > One `read()` returns at most the buffer size (256 bytes), and it may return fewer bytes than requested, so a larger file needs several calls. The loop keeps reading and writing each chunk until `read()` returns `0` (end of file) or `-1` (error).

---

## Task 2: Directory Listing & File Info

**Describe your implementation:** I kept `opendir()`, `readdir()`, `stat()` and `closedir()`, but replaced every `printf` with `snprintf()` into a 512-byte buffer followed by `write(1, buffer, len)`. The same format strings (`%-30s %10ld`) are used, so the columns line up exactly like the library version.

### Version A — Library Functions (`dir_list_lib.c`)

![Task 2 - Version A](screenshots/task2_lib.png)

### Version B — System Calls (`dir_list_sys.c`)

![Task 2 - Version B](screenshots/task2_sys.png)

### Questions

1. **What struct does `readdir()` return? What fields does it contain?**

   > A pointer to a `struct dirent` (or `NULL` at the end of the directory). On Linux it contains `d_ino` (inode number), `d_off` (position of the next entry), `d_reclen` (length of this record), `d_type` (file type, e.g. `DT_REG`, `DT_DIR`) and `d_name` (the file name). Only `d_ino` and `d_name` are required by POSIX.

2. **What information does `stat()` provide beyond file size?**

   > `struct stat` also gives the file type and permissions (`st_mode`), inode number (`st_ino`), device (`st_dev`), number of hard links (`st_nlink`), owner and group IDs (`st_uid`, `st_gid`), block size and blocks allocated (`st_blksize`, `st_blocks`), and timestamps for last access, last modification and last status change (`st_atime`, `st_mtime`, `st_ctime`).

3. **Why can't you `write()` a number directly — why do you need `snprintf()` first?**

   > `write()` only copies raw bytes from memory. Writing a `long` directly would output its 8 binary bytes, not the readable digits. `snprintf()` converts the number to ASCII text (e.g. `"620"`), applies the padding/alignment, and gives the length to pass to `write()`.

---

## Optional Bonus: Windows API (`file_creator_win.c`)

Screenshot of running on Windows:

> Not run: no Windows machine available (MacBook + Ubuntu VM). The source is included in `task1/file_creator_win.c`, and the questions below are answered.

### Bonus Questions

1. **Why does Windows use `HANDLE` instead of integer file descriptors?**

   > A `HANDLE` is an opaque reference to a kernel object in the process's handle table. Windows uses one handle type for many kinds of objects (files, processes, threads, events, mutexes, registry keys), each with its own access rights and security checks, so it hides the internal value behind an opaque type instead of a small integer index. POSIX file descriptors are also indexes into a per-process table, but they are plain integers and are mainly used for file-like objects.

2. **What is the Windows equivalent of POSIX `fork()`? Why is it different?**

   > `CreateProcess()`. `fork()` duplicates the calling process (a copy that continues from the same point) and `exec()` then loads a new program. `CreateProcess()` creates a new process and loads a given program in one step, with no copy of the parent's memory. Windows was not designed around the fork model, and copying a whole address space is expensive.

3. **Can you use POSIX calls on Windows?**

   > Not natively through the Windows API, but yes through a compatibility layer: WSL runs a real Linux kernel (WSL2) or translates Linux system calls (WSL1), and Cygwin/MSYS2 provide a POSIX library (`cygwin1.dll`) that implements POSIX functions on top of WinAPI. MinGW and MSVC also provide a few POSIX-like functions such as `_open`/`_write`.

---

## Task 3: strace Analysis

**Describe what you observed:** [TODO from your output: e.g. "The library version made __ system calls and the system call version __. Both start with the same program-loading calls (execve, mmap, loading libc.so.6). The real work is identical: one openat, one write of 36 bytes, one close, one write to stdout. The library version adds fstat, brk, ioctl calls for its buffers."]

### strace Output — Library Version (File Creator)

<!-- Screenshot: strace -e trace=openat,read,write,close ./file_creator_lib -->
<!-- IMPORTANT: Highlight/annotate the key system calls in your screenshot -->
![strace - Library version File Creator](screenshots/strace_lib_creator.png)

### strace Output — System Call Version (File Creator)

<!-- Screenshot: strace -e trace=openat,read,write,close ./file_creator_sys -->
<!-- IMPORTANT: Highlight/annotate the key system calls in your screenshot -->
![strace - System call version File Creator](screenshots/strace_sys_creator.png)

### strace Output — Library Version (File Reader or Dir Listing)

![strace - Library version](screenshots/strace_lib_reader.png)

### strace Output — System Call Version (File Reader or Dir Listing)

![strace - System call version](screenshots/strace_sys_reader.png)

### strace -c Summary Comparison

<!-- Screenshot of `strace -c` output for both versions -->
![strace summary - Library](screenshots/strace_summary_lib.png)
![strace summary - Syscall](screenshots/strace_summary_sys.png)

### Questions

1. **How many system calls does the library version make compared to the system call version?**

   > [TODO: read the `total` line of each `strace -c`.] The library version made __ system calls and the system call version made __, so the library version made __ more.

2. **What extra system calls appear in the library version? What do they do?**

   > [TODO: check the exact counts in your `strace -c` output.] Extra `fstat` calls: `fopen`/`printf` check the file and stdout to choose a buffer size and buffering mode. Extra `brk` calls: `malloc` grows the heap to allocate the `FILE` buffers. `ioctl(1, TCGETS)`: checks whether stdout is a terminal (line-buffered) or a file/pipe (fully buffered). `getrandom`: the malloc implementation seeds a random value the first time it runs. The `mmap`/`openat` calls that load `libc.so.6` appear in both versions, because both are linked against the C library.

3. **How many `write()` calls does `fprintf()` actually produce?**

   > [TODO: confirm in your trace.] One `write(3, "Hello from Operating Systems cla"..., 36)`, and it appears only at `fclose()`, not at the moment `fprintf` was called. `fprintf` only copies the text into the `FILE` buffer in user space; the buffer is flushed with a single `write()` when it is full or the file is closed. So the number of `write()` calls does not match the number of library calls.

4. **In your own words, what is the real difference between a library function and a system call?**

   > [Rewrite in your own words.] A library function (like `fprintf`) is ordinary code in user space inside libc: it formats, buffers and decides when to ask the kernel for help, and many calls can result in zero or one kernel request. A system call (like `write`) is the actual request to the kernel: the CPU switches to kernel mode, the kernel does the I/O, and returns. strace only sees system calls, which shows that every library I/O function ends up as `openat`/`write`/`close` underneath.

---

## Task 4: Exploring OS Structure

### System Information

> 📸 Screenshot of `uname -a`, `/proc/cpuinfo`, `/proc/meminfo`, `/proc/version`, `/proc/uptime`:

![System Info](screenshots/task4_system_info.png)

### Process Information

> 📸 Screenshot of `/proc/self/status`, `/proc/self/maps`, `ps aux`:

![Process Info](screenshots/task4_process_info.png)

### Kernel Modules

> 📸 Screenshot of `lsmod` and `modinfo`:

![Kernel Modules](screenshots/task4_modules.png)

### OS Layers Diagram

> 📸 Your diagram of the OS layers, labeled with real data from your system:

![OS Layers Diagram](screenshots/task4_os_layers_diagram.png)

### Questions

1. **What is `/proc`? Is it a real filesystem on disk?**

   > `/proc` is a virtual (pseudo) filesystem of type `procfs`. It is not stored on disk: the files have size 0 and their content is generated by the kernel at the moment you read them, from the kernel's own data structures (process table, memory counters, CPU info, loaded modules).

2. **Monolithic kernel vs. microkernel — which type does Linux use?**

   > In a monolithic kernel, almost all OS services (scheduling, memory management, file systems, device drivers, networking) run together in kernel space in one address space. A microkernel keeps only the minimum (IPC, basic scheduling, memory) in the kernel and runs drivers and file systems as user-space servers. Linux is monolithic, but modular: `lsmod` shows [TODO: number] modules such as [TODO: 2–3 names] that are loaded into kernel space at runtime; they run with full kernel privileges, unlike microkernel servers.

3. **What memory regions do you see in `/proc/self/maps`?**

   > [TODO: list what your output shows.] Typically: the program's own code and data (`/usr/bin/cat`, with `r-xp` for code and `rw-p` for data), the `[heap]`, shared libraries such as `libc.so.6` and the loader `ld-linux-x86-64.so.2`, sometimes `locale-archive`, anonymous mappings, the `[stack]`, and kernel-provided `[vvar]` and `[vdso]` pages. Each line shows the address range, permissions (r/w/x, p = private), offset, device, inode and path.

4. **Break down the kernel version string from `uname -a`.**

   > [TODO: paste your line and fill in each part.] Example format: `Linux hostname 6.8.0-45-generic #45-Ubuntu SMP PREEMPT_DYNAMIC <build date> x86_64 x86_64 x86_64 GNU/Linux` = kernel name (`Linux`), hostname, kernel release (`6.8.0` = major.minor.patch, `-45` = Ubuntu's ABI/package number, `generic` = kernel flavour; in WSL it ends in `-microsoft-standard-WSL2`), build number and options (`#45-Ubuntu SMP PREEMPT_DYNAMIC` = multi-processor support, preemptible kernel) with build date, machine/processor/hardware platform (`x86_64`), and operating system (`GNU/Linux`).

5. **How does `/proc` show that the OS is an intermediary between programs and hardware?**

   > User programs like `cat` never access the CPU, RAM or devices directly. When they read `/proc/cpuinfo` or `/proc/meminfo`, they use the normal `open`/`read` system calls, and the kernel answers with information it collected from the hardware. The program only sees what the kernel chooses to expose, through the standard file interface, which is exactly the intermediary role: hardware → kernel → system call → user program.

---

## Reflection

What did you learn from this activity? What was the most surprising difference between library functions and system calls?

> [Write your reflection here]
