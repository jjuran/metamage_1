/* crypt.h (dummy version) -- do not perform encryption
 * Hardly worth copyrighting :-)
 */

#ifndef GNU_GZIP_CRYPT_H
#define GNU_GZIP_CRYPT_H

#ifdef CRYPT
#  undef CRYPT      /* dummy version */
#endif

#define RAND_HEAD_LEN  12  /* length of encryption random header */

#define zencode
#define zdecode

#endif

