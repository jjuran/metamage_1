/*	=====================
 *	SavedCFragInitBlock.h
 *	=====================
 */

#ifndef SAVEDCFRAGINITBLOCK_H
#define SAVEDCFRAGINITBLOCK_H

#ifndef __MACTYPES__
#include "MacTypes.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

OSErr Initialize_SavedCFragInitBlock( const struct CFragSystem7InitBlock* initBlock );

const struct CFragSystem7InitBlock* SavedCFragInitBlock();

#ifdef __cplusplus
}
#endif

#endif

