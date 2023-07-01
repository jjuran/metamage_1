/*
	monitor_fdpass.cc
	-----------------
	
	This file has been modified.  See version control for details.
*/

/* $OpenBSD: monitor_fdpass.c,v 1.20 2015/02/25 23:05:47 djm Exp $ */
/*
 * Copyright 2001 Niels Provos <provos@citi.umich.edu>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#if defined(__MWERKS__)  &&  __MWERKS__ < 0x2400

/*
	This doesn't compile under CodeWarrior Pro 4, but it's not needed.
	(It's not needed under Pro 6 either, but at least it compiles.)
*/

#else  // #if defined(__MWERKS__)  &&  __MWERKS__ < 0x2400

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>

#if ! defined( __APPLE__ )  ||  defined( _SOCKLEN_T )
#include <errno.h>
#include <poll.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

//#include "log.h"
#include "monitor_fdpass.hh"

#ifdef __APPLE__
// Fix "array bound is not an integer constant" error on 10.5.
// (This code is not used on OS X at all.)
#undef CMSG_SPACE
#define CMSG_SPACE( x )  (x)
#endif


namespace openbsd
{

int
mm_send_fd(int sock, int fd)
{
	struct msghdr msg;
	union {
		struct cmsghdr hdr;
		char buf[CMSG_SPACE(sizeof(int))];
	} cmsgbuf;
	struct cmsghdr *cmsg;
	struct iovec vec;
	char ch = '\0';
	ssize_t n;
	struct pollfd pfd;

	memset(&msg, 0, sizeof(msg));
	memset(&cmsgbuf, 0, sizeof(cmsgbuf));
	msg.msg_control = (caddr_t)&cmsgbuf.buf;
	msg.msg_controllen = sizeof(cmsgbuf.buf);
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_len = CMSG_LEN(sizeof(int));
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	*(int *)CMSG_DATA(cmsg) = fd;

	vec.iov_base = &ch;
	vec.iov_len = 1;
	msg.msg_iov = &vec;
	msg.msg_iovlen = 1;

	pfd.fd = sock;
	pfd.events = POLLOUT;
	while ((n = sendmsg(sock, &msg, 0)) == -1 &&
	    (errno == EAGAIN || errno == EINTR)) {
		//debug3("%s: sendmsg(%d): %s", __func__, fd, strerror(errno));
		(void)poll(&pfd, 1, -1);
	}
	if (n == -1) {
		fprintf(stderr, "%s: sendmsg(%d): %s", __func__, fd,
		    strerror(errno));
		return -1;
	}

	if (n != 1) {
		fprintf(stderr, "%s: sendmsg: expected sent 1 got %ld",
		    __func__, (long)n);
		return -1;
	}
	return 0;
}

int
mm_receive_fd(int sock)
{
	struct msghdr msg;
	union {
		struct cmsghdr hdr;
		char buf[CMSG_SPACE(sizeof(int))];
	} cmsgbuf;
	struct cmsghdr *cmsg;
	struct iovec vec;
	ssize_t n;
	char ch;
	int fd;
	struct pollfd pfd;

	memset(&msg, 0, sizeof(msg));
	memset(&cmsgbuf, 0, sizeof(cmsgbuf));
	vec.iov_base = &ch;
	vec.iov_len = 1;
	msg.msg_iov = &vec;
	msg.msg_iovlen = 1;
	msg.msg_control = cmsgbuf.buf;
	msg.msg_controllen = sizeof(cmsgbuf.buf);

	pfd.fd = sock;
	pfd.events = POLLIN;
	while ((n = recvmsg(sock, &msg, 0)) == -1 &&
	    (errno == EAGAIN || errno == EINTR)) {
		//debug3("%s: recvmsg: %s", __func__, strerror(errno));
		(void)poll(&pfd, 1, -1);
	}
	if (n == -1) {
		fprintf(stderr, "%s: recvmsg: %s", __func__, strerror(errno));
		return -1;
	}

	if (n != 1) {
		fprintf(stderr, "%s: recvmsg: expected received 1 got %ld",
		    __func__, (long)n);
		return -1;
	}

	cmsg = CMSG_FIRSTHDR(&msg);
	if (cmsg == NULL) {
		fprintf(stderr, "%s: no message header", __func__);
		return -1;
	}

	if (cmsg->cmsg_type != SCM_RIGHTS) {
		fprintf(stderr, "%s: expected type %d got %d", __func__,
		    SCM_RIGHTS, cmsg->cmsg_type);
		return -1;
	}
	fd = (*(int *)CMSG_DATA(cmsg));
	return fd;
}

}  // namespace openbsd

#else  // #if ! defined( __APPLE__ )  ||  defined( _SOCKLEN_T )

int dummy;

#endif  // #if ! defined( __APPLE__ )  ||  defined( _SOCKLEN_T )

#endif  // #else  // #if defined(__MWERKS__)  &&  __MWERKS__ < 0x2400
