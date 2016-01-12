/* These are normally defined in the Amiga's types library, but are
   defined here instead to ensure portability with minimal changes to the
   original Amiga source-code
*/

#ifdef __MORPHOS__

#ifndef MOS_EXEC_TYPES_H
#define MOS_EXEC_TYPES_H

#include <exec/types.h>

typedef signed short            int16;
typedef unsigned char           uint8;
typedef signed char             int8;
typedef unsigned long           uint32;
typedef signed long             int32;
typedef unsigned short          uint16;
typedef double                  float64;

#ifndef CONST
#define CONST const
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#endif

#else

#ifndef EXEC_TYPES_H
#define EXEC_TYPES_H

typedef unsigned short		uint16;
typedef unsigned char		uint8;
typedef signed short		int16;
typedef signed char		int8;

#ifndef __SKYOS__
typedef unsigned long		uint32;
typedef signed long		int32;
#endif

typedef double			float64;
typedef char			TEXT;
typedef short			BOOL;
typedef long			LONG;
typedef unsigned long		ULONG;
#define FALSE 0
#define TRUE 1
#define CONST const

#endif
#endif

#ifdef __SURFACE32__
#define PIXEL Uint32
#define BYTESPERPIXEL 4
#define BITSPERPIXEL 32 
#else
#define PIXEL Uint16
#define BYTESPERPIXEL 2
#define BITSPERPIXEL 16
#endif
