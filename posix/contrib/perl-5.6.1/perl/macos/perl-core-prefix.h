// Defined for perl itself, undefined for host (extensions?)
#define PERL_CORE 1

// Equivalent to being 'in' every source file simultaneously.
// Needed to provide a static context for precompiling.
#define PERL_DECL_PROT

// Another dynamic context issue.
#define PERLIO_NOT_STDIO 0 

#include "EXTERN.h"
#include "perl.h"

