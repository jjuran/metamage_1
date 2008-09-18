/*	=====================
 *	SavedCFragInitBlock.h
 *	=====================
 */

#ifndef SAVEDCFRAGINITBLOCK_H
#define SAVEDCFRAGINITBLOCK_H

#if PRAGMA_ONCE
#pragma once
#endif

#include <MacTypes.h>


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

