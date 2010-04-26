#include <sys/mman.h>

#undef MAP_ANON

#define HAVE_MORECORE 0

#include "malloc.c.h"
