#ifndef	LAMP_SYSCALLS_H
#define	LAMP_SYSCALLS_H


#define __NR_InitProc                    0
//#define __NR_restart_syscall             0
#define __NR__exit                       1
#define __NR_vfork                       2
#define __NR_vfork_k                     2
#define __NR_SpawnVFork                  2
#define __NR_read                        3
#define __NR_write                       4
#define __NR_open                        5
#define __NR_close                       6
#define __NR_waitpid                     7
#define __NR_realpath_k                  8
#define __NR_link                        9
#define __NR_unlink                     10
#define __NR_execve                     11
#define __NR_chdir                      12
#define __NR_time                       13
#define __NR_mknod                      14
#define __NR_chmod                      15
#define __NR_lchown                     16
#define __NR_fchdir                     17
#define __NR_getcwd_k                   18
#define __NR_lseek                      19
#define __NR_getpid                     20
#define __NR_mount                      21
#define __NR_umount                     22
#define __NR_setuid                     23
#define __NR_getuid                     24
#define __NR_stime                      25
#define __NR_ptrace                     26
#define __NR_alarm                      27
#define __NR_nanosleep                  28
#define __NR_pause                      29
#define __NR_utime                      30
// 31
#define __NR_copyfile                   32
#define __NR_access                     33
#define __NR_nice                       34
#define __NR_ftime                      35
#define __NR_sync                       36
#define __NR_kill                       37
#define __NR_rename                     38
#define __NR_mkdir                      39
#define __NR_rmdir                      40
#define __NR_dup                        41
#define __NR_pipe                       42
#define __NR_times                      43
#define __NR_prof                       44
#define __NR_brk                        45
#define __NR_setgid                     46
#define __NR_getgid                     47
#define __NR_signal                     48
#define __NR_geteuid                    49
#define __NR_getegid                    50
#define __NR_acct                       51
#define __NR_umount2                    52
#define __NR_flock                      53
#define __NR_ioctl                      54
#define __NR_fcntl                      55
#define __NR_getpgid                    56
#define __NR_setpgid                    57
#define __NR_getsid                     58
#define __NR_uname                      59
#define __NR_umask                      60
#define __NR_chroot                     61
#define __NR_ustat                      62
#define __NR_dup2                       63
#define __NR_getppid                    64
#define __NR_sigprocmask                65
#define __NR_setsid                     66
#define __NR_sigaction                  67
#define __NR_sgetmask                   68
#define __NR_ssetmask                   69
#define __NR_setreuid                   70
#define __NR_setregid                   71
#define __NR_sigsuspend                 72
#define __NR_sigpending                 73
#define __NR_sethostname                74
#define __NR_setrlimit                  75
#define __NR_getrlimit                  76
#define __NR_getrusage                  78
#define __NR_gettimeofday               79
#define __NR_settimeofday               80
// 81
// 82
#define __NR_select                     83
#define __NR_symlink                    84
// 85
#define __NR_readlink_k                 86
// 87
// 88
// 89
#define __NR_readdir_k                  90
// 91
// 92
#define __NR_truncate                   93
#define __NR_ftruncate                  94
#define __NR_fchmod                     95
// 96
// 97
// 98
// 99
// 100
// 101
// 102
// 103
// 104
// 105
#define __NR_stat                      106
#define __NR_lstat                     107
#define __NR_fstat                     108
#define __NR_socketpair                109
#define __NR_socket                    110
#define __NR_bind                      111
#define __NR_listen                    112
#define __NR_accept                    113
#define __NR_connect                   114
#define __NR_shutdown                  115
#define __NR_getsockname               116
#define __NR_getpeername               117
#define __NR_getsockopt                118
#define __NR_setsockopt                119
#define __NR_send                      120
#define __NR_recv                      121
#define __NR_sendto                    122
#define __NR_recvfrom                  123
// 124
// 125
// 126
#define __NR_pump                      127
// 128
// 129
// 130
// 131
// 132
#define __NR_opendir                   133
#define __NR_closedir                  134
#define __NR_readdir                   135
#define __NR_rewinddir                 136
#define __NR_seekdir                   137
#define __NR_telldir                   138
#define __NR_dirfd                     139

#define __NR_ttypair                   141

#define __NR_gethostbyname             142
#define __NR_Path2FSSpec               143
#define __NR_AESendBlocking            144
#define __NR_OTInetMailExchange_k      145

#define __NR_fork_and_exit             146

#endif

