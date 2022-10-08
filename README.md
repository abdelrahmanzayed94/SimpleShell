# Simple Shell for Simple People
This simple shell provides the following features:
* compatibility with linux utilities (ls, pwd, cp, mv, rm, ...)
* setting new local variables (for example, "x=5") besides overwriting them if already found
* printing the available local variables using "set" command
* exporting the local variables to the environment variables using "export" command
* exit the simple shell using "exit" command

command used for compilation:
```
gcc simpleshell.c -o simpleshell
```

run example:
```
wizo@wizo:~/workarea/SimpleShell$ gcc simpleshell.c -o simpleshell
wizo@wizo:~/workarea/SimpleShell$ ./simpleshell 
user@pc:~$ ls
simpleshell  simpleshell.c  simpleshell.c.bak
user@pc:~$ ls -l /
total 2097232
lrwxrwxrwx   1 root root          7 Sep 10 12:53 bin -> usr/bin
drwxr-xr-x   4 root root       4096 Oct  8 14:04 boot
drwxrwxr-x   2 root root       4096 Sep 10 12:56 cdrom
drwxr-xr-x  19 root root       4140 Oct  8 13:52 dev
drwxr-xr-x 134 root root      12288 Oct  8 14:11 etc
drwxr-xr-x   3 root root       4096 Sep 10 12:57 home
lrwxrwxrwx   1 root root          7 Sep 10 12:53 lib -> usr/lib
lrwxrwxrwx   1 root root          9 Sep 10 12:53 lib32 -> usr/lib32
lrwxrwxrwx   1 root root          9 Sep 10 12:53 lib64 -> usr/lib64
lrwxrwxrwx   1 root root         10 Sep 10 12:53 libx32 -> usr/libx32
drwx------   2 root root      16384 Sep 10 12:53 lost+found
drwxr-xr-x   3 root root       4096 Sep 10 13:11 media
drwxr-xr-x   2 root root       4096 Feb 23  2022 mnt
drwxr-xr-x   3 root root       4096 Sep 10 13:32 opt
dr-xr-xr-x 270 root root          0 Oct  8 13:51 proc
drwx------   4 root root       4096 Sep 10 13:09 root
drwxr-xr-x  33 root root        940 Oct  8 14:36 run
lrwxrwxrwx   1 root root          8 Sep 10 12:53 sbin -> usr/sbin
drwxr-xr-x   9 root root       4096 Feb 23  2022 snap
drwxr-xr-x   2 root root       4096 Feb 23  2022 srv
-rw-------   1 root root 2147483648 Sep 10 12:53 swapfile
dr-xr-xr-x  13 root root          0 Oct  8 13:51 sys
drwxrwxrwt  20 root root       4096 Oct  8 15:05 tmp
drwxr-xr-x  14 root root       4096 Feb 23  2022 usr
drwxr-xr-x  14 root root       4096 Feb 23  2022 var
user@pc:~$ pwd
/home/wizo/workarea/SimpleShell
user@pc:~$ echo Hello From Simple Shell
Hello From Simple Shell
user@pc:~$ x=10
user@pc:~$ set
local variable[0]: x = 10
user@pc:~$ y=15
user@pc:~$ set
local variable[0]: x = 10
local variable[1]: y = 15
user@pc:~$ export x
user@pc:~$ env
SHELL=/bin/bash
...
x=10
user@pc:~$ 
user@pc:~$ exit
wizo@wizo:~/workarea/SimpleShell$ 
```
