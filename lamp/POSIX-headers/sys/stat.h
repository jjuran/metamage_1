/*-
 * Copyright (c) 1982, 1986, 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * [3 Deleted as of 22Jul99, see
 *     ftp://ftp.cs.berkeley.edu/pub/4bsd/README.Impt.License.Change
 *	   for details]
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)stat.h	8.6 (Berkeley) 3/8/94
 */

/* Adapted for GUSI by Matthias Neeracher <neeri@iis.ee.ethz.ch> */

#ifndef _SYS_STAT_H_
#define	_SYS_STAT_H_

#include <sys/types.h>
#include <time.h>

struct stat
{
	dev_t            st_dev;      /* inode's device */
	ino_t            st_ino;      /* inode's number */
	mode_t           st_mode;     /* inode protection mode */
	nlink_t          st_nlink;    /* number of hard links */
	uid_t            st_uid;      /* user ID of the file's owner */
	gid_t            st_gid;      /* group ID of the file's group */
	dev_t            st_rdev;     /* device type */
	struct timespec  st_atim;     /* time of last access */
	struct timespec  st_mtim;     /* time of last data modification */
	struct timespec  st_ctim;     /* time of last file status change */
	struct timespec  st_birthtim; /* time of creation */
	struct timespec  st_checktim; /* time of last backup/archive/checkpoint */
	off_t            st_size;     /* file size, in bytes */
	long             st_blocks;   /* blocks allocated for file */
	unsigned long    st_blksize;  /* optimal blocksize for I/O */
	unsigned long    st_flags;    /* user defined flags for file */
	unsigned char    st_name[32]; /* Mac File Manager name of file */
};

#define st_atime  st_atim.tv_sec
#define st_mtime  st_mtim.tv_sec
#define st_ctime  st_ctim.tv_sec

#define st_birthtime  st_birthtim.tv_sec
#define st_checktime  st_checktim.tv_sec

#define	S_ISUID	0004000			/* set user id on execution */
#define	S_ISGID	0002000			/* set group id on execution */

#define	S_IRWXU	0000700			/* RWX mask for owner */
#define	S_IRUSR	0000400			/* R for owner */
#define	S_IWUSR	0000200			/* W for owner */
#define	S_IXUSR	0000100			/* X for owner */

#define	S_IRWXG	0000070			/* RWX mask for group */
#define	S_IRGRP	0000040			/* R for group */
#define	S_IWGRP	0000020			/* W for group */
#define	S_IXGRP	0000010			/* X for group */

#define	S_IRWXO	0000007			/* RWX mask for other */
#define	S_IROTH	0000004			/* R for other */
#define	S_IWOTH	0000002			/* W for other */
#define	S_IXOTH	0000001			/* X for other */

#define S_IRUGO  0000444
#define S_IWUGO  0000222
#define S_IXUGO  0000111

#define S_ISUID  0004000
#define S_ISGID  0002000
#define S_ISVTX  0001000

#define	S_IFMT	 0170000		/* type of file mask */
#define	S_IFIFO	 0010000		/* named pipe (fifo) */
#define	S_IFCHR	 0020000		/* character special */
#define	S_IFDIR	 0040000		/* directory */
#define	S_IFBLK	 0060000		/* block special */
#define	S_IFREG	 0100000		/* regular */
#define	S_IFLNK	 0120000		/* symbolic link */
#define	S_IFSOCK 0140000		/* socket */

#define	S_ISDIR(m)	(((m) & 0170000) == 0040000)	/* directory */
#define	S_ISCHR(m)	(((m) & 0170000) == 0020000)	/* char special */
#define	S_ISBLK(m)	(((m) & 0170000) == 0060000)	/* block special */
#define	S_ISREG(m)	(((m) & 0170000) == 0100000)	/* regular file */
#define	S_ISFIFO(m)	(((m) & 0170000) == 0010000)	/* fifo */
#define	S_ISLNK(m)	(((m) & 0170000) == 0120000)	/* symbolic link */
#define	S_ISSOCK(m)	(((m) & 0170000) == 0140000)	/* socket */

#define AT_FDCWD  (-100)
#define AT_SYMLINK_FOLLOW       2
#define AT_SYMLINK_NOFOLLOW  4096

#define AT_LINK_ALIAS  0x80000000

#define UTIME_ARCHIVE  0x40000000

#define UTIME_NOW   ((1 << 30) - 1)
#define UTIME_OMIT  ((1 << 30) - 2)

#include <sys/cdefs.h>

__BEGIN_DECLS
int futimens( int fd, const struct timespec times[2] );
int utimensat( int fd, const char* path, const struct timespec times[2], int flags );

int fstatat( int dirfd, const char* path, struct stat* sb, int flags );
int mkdirat( int dirfd, const char* path, mode_t mode );
int mkfifoat( int dirfd, const char* path, mode_t mode );
int mknodat( int dirfd, const char* path, mode_t mode, dev_t dev );

int	chmod(const char *, mode_t);
int	fstat(int, struct stat *);
int	mknod(const char *, mode_t, dev_t);
int	mkdir(const char *, mode_t);
int	mkfifo(const char *, mode_t);
int	stat(const char *, struct stat *);
mode_t	umask(mode_t);
#if __BSD_VISIBLE
int	chflags(const char *, unsigned int);
int	fchflags(int, unsigned int);
int	fchmod(int, mode_t);
int	lstat(const char *, struct stat *);
int	isfdtype(int, int);
#endif
__END_DECLS

#endif /* !_SYS_STAT_H_ */
