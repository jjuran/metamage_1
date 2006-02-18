/* config.hin.  Generated from configure.ac by autoheader.  */

/* Define if chown is not POSIX compliant regarding IDs of -1. */
#undef CHOWN_FAILS_TO_HONOR_ID_OF_NEGATIVE_ONE

/* Define if chown modifies symlinks. */
#undef CHOWN_MODIFIES_SYMLINK

/* Define to 1 if the `closedir' function returns void instead of `int'. */
#undef CLOSEDIR_VOID

/* Define to one of `_getb67', `GETB67', `getb67' for Cray-2 and Cray-YMP
   systems. This function is required for `alloca.c' support on those systems.
   */
#undef CRAY_STACKSEG_END

/* Define to 1 if using `alloca.c'. */
#undef C_ALLOCA

/* Define to a string giving the full name of the default archive file. */
#undef DEFAULT_ARCHIVE

/* By default produce archives of this format */
#undef DEFAULT_ARCHIVE_FORMAT

/* Define to a number giving the default blocking size for archives. */
#undef DEFAULT_BLOCKING

/* Define full file name of rmt program. */
#undef DEFAULT_RMT_COMMAND

/* 
   [Define], [to], [1], [if], [density], [may], [be], [indicated], [by], [[lmh]], [at], [end], [of], [device.]
   */
#undef DENSITY_LETTER

/* Define to a string giving the prefix of the default device, without the
   part specifying the unit and density. */
#undef DEVICE_PREFIX

/* Define if there is a member named d_ino in the struct describing directory
   headers. */
#undef D_INO_IN_DIRENT

/* Define to 1 if translation of program messages to the user's native
   language is requested. */
#undef ENABLE_NLS

/* Define on systems for which file names may have a so-called `drive letter'
   prefix, define this to compute the length of that prefix, including the
   colon. */
#undef FILE_SYSTEM_ACCEPTS_DRIVE_LETTER_PREFIX

/* Define if the backslash character may also serve as a file name component
   separator. */
#undef FILE_SYSTEM_BACKSLASH_IS_FILE_NAME_SEPARATOR

#if FILE_SYSTEM_ACCEPTS_DRIVE_LETTER_PREFIX
# define FILE_SYSTEM_PREFIX_LEN(Filename) \
  ((Filename)[0] && (Filename)[1] == ':' ? 2 : 0)
#else
# define FILE_SYSTEM_PREFIX_LEN(Filename) 0
#endif

/* Define if gettimeofday clobbers localtime's static buffer. */
#undef GETTIMEOFDAY_CLOBBERS_LOCALTIME_BUFFER

/* Define to 1 if you have the `alarm' function. */
#undef HAVE_ALARM

/* Define to 1 if you have `alloca' after including <alloca.h>, a header that
   may be supplied by this distribution. */
#undef HAVE_ALLOCA

/* Define HAVE_ALLOCA_H for backward compatibility with older code that
   includes <alloca.h> only if HAVE_ALLOCA_H is defined. */
#undef HAVE_ALLOCA_H

/* Define to 1 if you have the `btowc' function. */
#undef HAVE_BTOWC

/* Define to 1 if your system has a working `chown' function. */
#undef HAVE_CHOWN

/* Define to 1 if you have the `chsize' function. */
#undef HAVE_CHSIZE

/* Define to 1 if you have the `clock_gettime' function. */
#undef HAVE_CLOCK_GETTIME

/* Define to 1 if you have the `clock_settime' function. */
#undef HAVE_CLOCK_SETTIME

/* Define if the GNU dcgettext() function is already present or preinstalled.
   */
#undef HAVE_DCGETTEXT

/* Define to 1 if you have the declaration of `clearerr_unlocked', and to 0 if
   you don't. */
#undef HAVE_DECL_CLEARERR_UNLOCKED

/* Define to 1 if you have the declaration of `feof_unlocked', and to 0 if you
   don't. */
#undef HAVE_DECL_FEOF_UNLOCKED

/* Define to 1 if you have the declaration of `ferror_unlocked', and to 0 if
   you don't. */
#undef HAVE_DECL_FERROR_UNLOCKED

/* Define to 1 if you have the declaration of `fflush_unlocked', and to 0 if
   you don't. */
#undef HAVE_DECL_FFLUSH_UNLOCKED

/* Define to 1 if you have the declaration of `fgets_unlocked', and to 0 if
   you don't. */
#undef HAVE_DECL_FGETS_UNLOCKED

/* Define to 1 if you have the declaration of `fputc_unlocked', and to 0 if
   you don't. */
#undef HAVE_DECL_FPUTC_UNLOCKED

/* Define to 1 if you have the declaration of `fputs_unlocked', and to 0 if
   you don't. */
#undef HAVE_DECL_FPUTS_UNLOCKED

/* Define to 1 if you have the declaration of `fread_unlocked', and to 0 if
   you don't. */
#undef HAVE_DECL_FREAD_UNLOCKED

/* Define to 1 if you have the declaration of `fwrite_unlocked', and to 0 if
   you don't. */
#undef HAVE_DECL_FWRITE_UNLOCKED

/* Define to 1 if you have the declaration of `getchar_unlocked', and to 0 if
   you don't. */
#undef HAVE_DECL_GETCHAR_UNLOCKED

/* Define to 1 if you have the declaration of `getcwd', and to 0 if you don't.
   */
#undef HAVE_DECL_GETCWD

/* Define to 1 if you have the declaration of `getc_unlocked', and to 0 if you
   don't. */
#undef HAVE_DECL_GETC_UNLOCKED

/* Define to 1 if you have the declaration of `getenv', and to 0 if you don't.
   */
#undef HAVE_DECL_GETENV

/* Define to 1 if you have the declaration of `getgrgid', and to 0 if you
   don't. */
#undef HAVE_DECL_GETGRGID

/* Define to 1 if you have the declaration of `getpwuid', and to 0 if you
   don't. */
#undef HAVE_DECL_GETPWUID

/* Define to 1 if you have the declaration of `nanosleep', and to 0 if you
   don't. */
#undef HAVE_DECL_NANOSLEEP

/* Define to 1 if you have the declaration of `program_invocation_name', and
   to 0 if you don't. */
#undef HAVE_DECL_PROGRAM_INVOCATION_NAME

/* Define to 1 if you have the declaration of `program_invocation_short_name',
   and to 0 if you don't. */
#undef HAVE_DECL_PROGRAM_INVOCATION_SHORT_NAME

/* Define to 1 if you have the declaration of `putchar_unlocked', and to 0 if
   you don't. */
#undef HAVE_DECL_PUTCHAR_UNLOCKED

/* Define to 1 if you have the declaration of `putc_unlocked', and to 0 if you
   don't. */
#undef HAVE_DECL_PUTC_UNLOCKED

/* Define to 1 if you have the declaration of `strerror_r', and to 0 if you
   don't. */
#undef HAVE_DECL_STRERROR_R

/* Define to 1 if you have the declaration of `strnlen', and to 0 if you
   don't. */
#undef HAVE_DECL_STRNLEN

/* Define to 1 if you have the declaration of `strtoimax', and to 0 if you
   don't. */
#undef HAVE_DECL_STRTOIMAX

/* Define to 1 if you have the declaration of `strtoll', and to 0 if you
   don't. */
#undef HAVE_DECL_STRTOLL

/* Define to 1 if you have the declaration of `strtoull', and to 0 if you
   don't. */
#undef HAVE_DECL_STRTOULL

/* Define to 1 if you have the declaration of `strtoumax', and to 0 if you
   don't. */
#undef HAVE_DECL_STRTOUMAX

/* Define to 1 if you have the declaration of `time', and to 0 if you don't.
   */
#undef HAVE_DECL_TIME

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#undef HAVE_DIRENT_H

/* Define if you have the declaration of environ. */
#undef HAVE_ENVIRON_DECL

/* Define if you have the declaration of errno. */
#undef HAVE_ERRNO_DECL

/* Define to 1 if you have the `fchdir' function. */
#undef HAVE_FCHDIR

/* Define to 1 if you have the <fcntl.h> header file. */
#undef HAVE_FCNTL_H

/* Define to 1 if you have the <features.h> header file. */
#undef HAVE_FEATURES_H

/* Define to 1 if you have the `flockfile' function. */
#undef HAVE_FLOCKFILE

/* Define to 1 if you have the `fsync' function. */
#undef HAVE_FSYNC

/* Define to 1 if you have the `ftruncate' function. */
#undef HAVE_FTRUNCATE

/* Define to 1 if you have the `funlockfile' function. */
#undef HAVE_FUNLOCKFILE

/* Define to 1 if you have the `getdelim' function. */
#undef HAVE_GETDELIM

/* Define to 1 if you have the <getopt.h> header file. */
#undef HAVE_GETOPT_H

/* Define to 1 if you have the `getopt_long_only' function. */
#undef HAVE_GETOPT_LONG_ONLY

/* Define to 1 if you have the `getpagesize' function. */
#undef HAVE_GETPAGESIZE

/* Define if the GNU gettext() function is already present or preinstalled. */
#undef HAVE_GETTEXT

/* Define to 1 if you have the `gettimeofday' function. */
#undef HAVE_GETTIMEOFDAY

/* Define if you have the iconv() function. */
#undef HAVE_ICONV

/* Define to 1 if you have the <iconv.h> header file. */
#undef HAVE_ICONV_H

/* Define if you have the 'intmax_t' type in <stdint.h> or <inttypes.h>. */
#undef HAVE_INTMAX_T

/* Define to 1 if you have the <inttypes.h> header file. */
#undef HAVE_INTTYPES_H

/* Define if <inttypes.h> exists, doesn't clash with <sys/types.h>, and
   declares uintmax_t. */
#undef HAVE_INTTYPES_H_WITH_UINTMAX

/* Define to 1 if you have the `isascii' function. */
#undef HAVE_ISASCII

/* Define to 1 if you have the `iswprint' function. */
#undef HAVE_ISWPRINT

/* Define if you have <langinfo.h> and nl_langinfo(CODESET). */
#undef HAVE_LANGINFO_CODESET

/* Define to 1 if you have the `lchown' function. */
#undef HAVE_LCHOWN

/* Define to 1 if you have the <linewrap.h> header file. */
#undef HAVE_LINEWRAP_H

/* Define to 1 if you have the <linux/fd.h> header file. */
#undef HAVE_LINUX_FD_H

/* Define to 1 if you have the <locale.h> header file. */
#undef HAVE_LOCALE_H

/* Define if you have the 'long double' type. */
#undef HAVE_LONG_DOUBLE

/* Define to 1 if you support file names longer than 14 characters. */
#undef HAVE_LONG_FILE_NAMES

/* Define if you have the 'long long' type. */
#undef HAVE_LONG_LONG

/* Define to 1 if you have the `lstat' function. */
#undef HAVE_LSTAT

/* Define to 1 if mbrtowc and mbstate_t are properly declared. */
#undef HAVE_MBRTOWC

/* Define to 1 if you have the `mbsinit' function. */
#undef HAVE_MBSINIT

/* Define to 1 if you have the `mbsrtowcs' function. */
#undef HAVE_MBSRTOWCS

/* Define to 1 if <wchar.h> declares mbstate_t. */
#undef HAVE_MBSTATE_T

/* Define to 1 if you have the <memory.h> header file. */
#undef HAVE_MEMORY_H

/* Define to 1 if you have the `mempcpy' function. */
#undef HAVE_MEMPCPY

/* Define to 1 if you have the `memset' function. */
#undef HAVE_MEMSET

/* Define to 1 if you have the `mkfifo' function. */
#undef HAVE_MKFIFO

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
#undef HAVE_NDIR_H

/* Define to 1 if you have the <netdb.h> header file. */
#undef HAVE_NETDB_H

/* Define to 1 if you have the <net/errno.h> header file. */
#undef HAVE_NET_ERRNO_H

/* Define to 1 if libc includes obstacks. */
#undef HAVE_OBSTACK

/* Define to 1 if you have the <OS.h> header file. */
#undef HAVE_OS_H

/* Define to 1 if getcwd works, except it sometimes fails when it shouldn't,
   setting errno to ERANGE, ENAMETOOLONG, or ENOENT. If __GETCWD_PREFIX is not
   defined, it doesn't matter whether HAVE_PARTLY_WORKING_GETCWD is defined.
   */
#undef HAVE_PARTLY_WORKING_GETCWD

/* Define to 1 if you have the `pathconf' function. */
#undef HAVE_PATHCONF

/* Define to 1 if you have the `readlink' function. */
#undef HAVE_READLINK

/* Define to 1 if you have the `rmdir' function. */
#undef HAVE_RMDIR

/* Define to 1 if you have the <search.h> header file. */
#undef HAVE_SEARCH_H

/* Define to 1 if you have the `setenv' function. */
#undef HAVE_SETENV

/* Define to 1 if you have the `setlocale' function. */
#undef HAVE_SETLOCALE

/* Define to 1 if you have the <sgtty.h> header file. */
#undef HAVE_SGTTY_H

/* Define to 1 if stdbool.h conforms to C99. */
#undef HAVE_STDBOOL_H

/* Define to 1 if you have the <stddef.h> header file. */
#undef HAVE_STDDEF_H

/* Define to 1 if you have the <stdint.h> header file. */
#undef HAVE_STDINT_H

/* Define if <stdint.h> exists, doesn't clash with <sys/types.h>, and declares
   uintmax_t. */
#undef HAVE_STDINT_H_WITH_UINTMAX

/* Define to 1 if you have the <stdlib.h> header file. */
#undef HAVE_STDLIB_H

/* Define to 1 if you have the `stpcpy' function. */
#undef HAVE_STPCPY

/* Define to 1 if you have the `strcasecmp' function. */
#undef HAVE_STRCASECMP

/* Define to 1 if you have the `strchrnul' function. */
#undef HAVE_STRCHRNUL

/* Define to 1 if you have the `strerror' function. */
#undef HAVE_STRERROR

/* Define to 1 if you have the `strerror_r' function. */
#undef HAVE_STRERROR_R

/* Define to 1 if you have the <strings.h> header file. */
#undef HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. */
#undef HAVE_STRING_H

/* Define to 1 if you have the `strncasecmp' function. */
#undef HAVE_STRNCASECMP

/* Define to 1 if you have the `strndup' function. */
#undef HAVE_STRNDUP

/* Define to 1 if you have the `strtoimax' function. */
#undef HAVE_STRTOIMAX

/* Define to 1 if you have the `strtol' function. */
#undef HAVE_STRTOL

/* Define to 1 if you have the `strtoll' function. */
#undef HAVE_STRTOLL

/* Define to 1 if you have the `strtoul' function. */
#undef HAVE_STRTOUL

/* Define to 1 if you have the `strtoull' function. */
#undef HAVE_STRTOULL

/* Define to 1 if you have the `strtoumax' function. */
#undef HAVE_STRTOUMAX

/* Define to 1 if `st_atimensec' is member of `struct stat'. */
#undef HAVE_STRUCT_STAT_ST_ATIMENSEC

/* Define to 1 if `st_atimespec.tv_nsec' is member of `struct stat'. */
#undef HAVE_STRUCT_STAT_ST_ATIMESPEC_TV_NSEC

/* Define to 1 if `st_atim.tv_nsec' is member of `struct stat'. */
#undef HAVE_STRUCT_STAT_ST_ATIM_TV_NSEC

/* Define to 1 if `st_blksize' is member of `struct stat'. */
#undef HAVE_STRUCT_STAT_ST_BLKSIZE

/* Define to 1 if `st_blocks' is member of `struct stat'. */
#undef HAVE_STRUCT_STAT_ST_BLOCKS

/* Define to 1 if `st_spare1' is member of `struct stat'. */
#undef HAVE_STRUCT_STAT_ST_SPARE1

/* Define if struct timespec is declared in <time.h>. */
#undef HAVE_STRUCT_TIMESPEC

/* Define to 1 if `tm_zone' is member of `struct tm'. */
#undef HAVE_STRUCT_TM_TM_ZONE

/* Define if struct utimbuf is declared -- usually in <utime.h>. Some systems
   have utime.h but don't declare the struct anywhere. */
#undef HAVE_STRUCT_UTIMBUF

/* Define to 1 if your `struct stat' has `st_blksize'. Deprecated, use
   `HAVE_STRUCT_STAT_ST_BLKSIZE' instead. */
#undef HAVE_ST_BLKSIZE

/* Define to 1 if your `struct stat' has `st_blocks'. Deprecated, use
   `HAVE_STRUCT_STAT_ST_BLOCKS' instead. */
#undef HAVE_ST_BLOCKS

/* Define if struct stat has a char st_fstype[] member. */
#undef HAVE_ST_FSTYPE_STRING

/* Define to 1 if you have the `symlink' function. */
#undef HAVE_SYMLINK

/* Define to 1 if you have the <sysexits.h> header file. */
#undef HAVE_SYSEXITS_H

/* Define to 1 if you have the <sys/buf.h> header file. */
#undef HAVE_SYS_BUF_H

/* Define to 1 if you have the <sys/device.h> header file. */
#undef HAVE_SYS_DEVICE_H

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
#undef HAVE_SYS_DIR_H

/* Define to 1 if you have the <sys/gentape.h> header file. */
#undef HAVE_SYS_GENTAPE_H

/* Define to 1 if you have the <sys/inet.h> header file. */
#undef HAVE_SYS_INET_H

/* Define to 1 if you have the <sys/io/trioctl.h> header file. */
#undef HAVE_SYS_IO_TRIOCTL_H

/* Define to 1 if you have the <sys/mtio.h> header file. */
#undef HAVE_SYS_MTIO_H

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
#undef HAVE_SYS_NDIR_H

/* Define to 1 if you have the <sys/param.h> header file. */
#undef HAVE_SYS_PARAM_H

/* Define to 1 if you have the <sys/stat.h> header file. */
#undef HAVE_SYS_STAT_H

/* Define to 1 if you have the <sys/tape.h> header file. */
#undef HAVE_SYS_TAPE_H

/* Define to 1 if you have the <sys/time.h> header file. */
#undef HAVE_SYS_TIME_H

/* Define to 1 if you have the <sys/tprintf.h> header file. */
#undef HAVE_SYS_TPRINTF_H

/* Define to 1 if you have the <sys/types.h> header file. */
#undef HAVE_SYS_TYPES_H

/* Define to 1 if you have <sys/wait.h> that is POSIX.1 compatible. */
#undef HAVE_SYS_WAIT_H

/* Define to 1 if localtime_r, etc. have the type signatures that POSIX
   requires. */
#undef HAVE_TIME_R_POSIX

/* Define if struct tm has the tm_gmtoff member. */
#undef HAVE_TM_GMTOFF

/* Define to 1 if your `struct tm' has `tm_zone'. Deprecated, use
   `HAVE_STRUCT_TM_TM_ZONE' instead. */
#undef HAVE_TM_ZONE

/* Define to 1 if you have the `tsearch' function. */
#undef HAVE_TSEARCH

/* Define to 1 if you don't have `tm_zone' but do have the external array
   `tzname'. */
#undef HAVE_TZNAME

/* Define if you have the 'uintmax_t' type in <stdint.h> or <inttypes.h>. */
#undef HAVE_UINTMAX_T

/* Define to 1 if you have the <unistd.h> header file. */
#undef HAVE_UNISTD_H

/* Define to 1 if you have the `unsetenv' function. */
#undef HAVE_UNSETENV

/* Define if you have the 'unsigned long long' type. */
#undef HAVE_UNSIGNED_LONG_LONG

/* Define to 1 if you have the `utimes' function. */
#undef HAVE_UTIMES

/* Define if utimes accepts a null argument */
#undef HAVE_UTIMES_NULL

/* Define to 1 if you have the <utime.h> header file. */
#undef HAVE_UTIME_H

/* Define to 1 if `utime(file, NULL)' sets file's timestamp to the present. */
#undef HAVE_UTIME_NULL

/* Define to 1 if you have the `waitpid' function. */
#undef HAVE_WAITPID

/* Define to 1 if you have the <wchar.h> header file. */
#undef HAVE_WCHAR_H

/* Define to 1 if you have the <wctype.h> header file. */
#undef HAVE_WCTYPE_H

/* Define to 1 if you have the `wmemchr' function. */
#undef HAVE_WMEMCHR

/* Define to 1 if you have the `wmemcpy' function. */
#undef HAVE_WMEMCPY

/* Define to 1 if you have the `wmempcpy' function. */
#undef HAVE_WMEMPCPY

/* Define to 1 if the system has the type `_Bool'. */
#undef HAVE__BOOL

/* Define as const if the declaration of iconv() needs const. */
#undef ICONV_CONST

#if FILE_SYSTEM_BACKSLASH_IS_FILE_NAME_SEPARATOR
# define ISSLASH(C) ((C) == '/' || (C) == '\\')
#else
# define ISSLASH(C) ((C) == '/')
#endif

/* Define to 1 if `major', `minor', and `makedev' are declared in <mkdev.h>.
   */
#undef MAJOR_IN_MKDEV

/* Define to 1 if `major', `minor', and `makedev' are declared in
   <sysmacros.h>. */
#undef MAJOR_IN_SYSMACROS

/* If malloc(0) is != NULL, define this to 1. Otherwise define this to 0. */
#undef MALLOC_0_IS_NONNULL

/* Define to mt_model (v.g., for DG/UX), else to mt_type. */
#undef MTIO_CHECK_FIELD

/* Name of package */
#undef PACKAGE

/* Define to the address where bug reports for this package should be sent. */
#undef PACKAGE_BUGREPORT

/* Define to the full name of this package. */
#undef PACKAGE_NAME

/* Define to the full name and version of this package. */
#undef PACKAGE_STRING

/* Define to the one symbol short name of this package. */
#undef PACKAGE_TARNAME

/* Define to the version of this package. */
#undef PACKAGE_VERSION

/* Define to the full path of your rsh, if any. */
#undef REMOTE_SHELL

/* Define as the return type of signal handlers (`int' or `void'). */
#undef RETSIGTYPE

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
	STACK_DIRECTION > 0 => grows toward higher addresses
	STACK_DIRECTION < 0 => grows toward lower addresses
	STACK_DIRECTION = 0 => direction of growth unknown */
#undef STACK_DIRECTION

/* Define to 1 if the `S_IS*' macros in <sys/stat.h> do not work properly. */
#undef STAT_MACROS_BROKEN

/* Define to 1 if you have the ANSI C header files. */
#undef STDC_HEADERS

/* Define to 1 if strerror_r returns char *. */
#undef STRERROR_R_CHAR_P

/* Define to be the nanoseconds member of struct stat's st_mtim, if it exists.
   */
#undef ST_MTIM_NSEC

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#undef TIME_WITH_SYS_TIME

/* Define to 1 if your <sys/time.h> declares `struct tm'. */
#undef TM_IN_SYS_TIME

/* Define to 1 if you want getc etc. to use unlocked I/O if available.
   Unlocked I/O can improve performance in unithreaded apps, but it is not
   safe for multithreaded apps. */
#undef USE_UNLOCKED_IO

/* Version number of package */
#undef VERSION

/* Define if unsetenv() returns void, not int. */
#undef VOID_UNSETENV

/* Define to 1 if on AIX 3.
   System headers sometimes define this.
   We just want to avoid a redefinition error message.  */
#ifndef _ALL_SOURCE
# undef _ALL_SOURCE
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
#undef _FILE_OFFSET_BITS

/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# undef _GNU_SOURCE
#endif

/* Define for large files, on AIX-style hosts. */
#undef _LARGE_FILES

/* Define to 1 if on MINIX. */
#undef _MINIX

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
#undef _POSIX_1_SOURCE

/* Define to 1 if you need to in order for `stat' and other things to work. */
#undef _POSIX_SOURCE

/* Enable extensions on Solaris.  */
#ifndef __EXTENSIONS__
# undef __EXTENSIONS__
#endif

/* Define to rpl_ if the getcwd replacement function should be used. */
#undef __GETCWD_PREFIX

/* Define to rpl_ if the getopt replacement functions and variables should be
   used. */
#undef __GETOPT_PREFIX

/* Define to rpl_chown if the replacement function should be used. */
#undef chown

/* Define to empty if `const' does not conform to ANSI C. */
#undef const

/* Define to `unsigned' if <sys/types.h> does not define. */
#undef dev_t

/* Define to a replacement function name for fnmatch(). */
#undef fnmatch

/* Define to a replacement function name for getline(). */
#undef getline

/* Define to rpl_gettimeofday if the replacement function should be used. */
#undef gettimeofday

/* Define to `int' if <sys/types.h> doesn't define. */
#undef gid_t

/* Define to rpl_gmtime if the replacement function should be used. */
#undef gmtime

/* Conversion descriptor type */
#undef iconv_t

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
#undef inline
#endif

/* Define to `unsigned' if <sys/types.h> does not define. */
#undef ino_t

/* Define to long or long long if <inttypes.h> and <stdint.h> don't define. */
#undef intmax_t

/* Define to rpl_localtime if the replacement function should be used. */
#undef localtime

/* Type of major device numbers. */
#undef major_t

/* Define to a type if <wchar.h> does not define. */
#undef mbstate_t

/* Type of minor device numbers. */
#undef minor_t

/* Define to rpl_mktime if the replacement function should be used. */
#undef mktime

/* Define to `int' if <sys/types.h> does not define. */
#undef mode_t

/* Define to `long' if <sys/types.h> does not define. */
#undef off_t

/* Define to `int' if <sys/types.h> does not define. */
#undef pid_t

/* Define to equivalent of C99 restrict keyword, or to nothing if this is not
   supported. Do not define if restrict is supported directly. */
#undef restrict

/* Define to `unsigned' if <sys/types.h> does not define. */
#undef size_t

/* Define as a signed type of the same size as size_t. */
#undef ssize_t

/* Define to rpl_strnlen if the replacement function should be used. */
#undef strnlen

/* Define to `int' if <sys/types.h> doesn't define. */
#undef uid_t

/* Define to unsigned long or unsigned long long if <stdint.h> and
   <inttypes.h> don't define. */
#undef uintmax_t

/* Define to rpl_utime if the replacement function should be used. */
#undef utime
