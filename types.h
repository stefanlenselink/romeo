#ifndef	__TYPE_H__
#define	__TYPE_H__

/********************************************************************
 * Elementary data types
 ********************************************************************/
/*
 * This Cygwin patch is due to Isaac Salzman
 */
#ifdef __CYGWIN
#	include <sys/param.h>
#	define __LITTLE_ENDIAN LITTLE_ENDIAN
#	define __BIG_ENDIAN    BIG_ENDIAN
#	define __BYTE_ORDER    BYTE_ORDER
#elif sun
#	include <sys/isa_defs.h>
#else
#	include <endian.h>
#endif /* __CYGWIN */

#ifdef sun
#	include <sys/int_limits.h>
#else
#	include <stdint.h>
#endif

/*
 * This O_BINARY patch was inspired by Mark Contois
 */
#ifndef	O_BINARY
#	define	O_BINARY	0
#endif	/* O_BINARY */

/*Groan: some machines don't define these */
#ifndef __LITTLE_ENDIAN
#	define __LITTLE_ENDIAN 1234
#endif
#ifndef __BIG_ENDIAN
#	define __BIG_ENDIAN    4321
#endif
#ifndef __PDP_ENDIAN
#	define __PDP_ENDIAN    3412
#endif

#ifndef __BYTE_ORDER
#	ifdef _LITTLE_ENDIAN
#		define __BYTE_ORDER __LITTLE_ENDIAN
#	endif
#	ifdef _BIG_ENDIAN
#		define __BYTE_ORDER __BIG_ENDIAN
#	endif
#	ifdef _PDP_ENDIAN
#		define __BYTE_ORDER __PDP_ENDIAN
#	endif
#endif

#define CPU_ENDIAN_LITTLE	__LITTLE_ENDIAN
#define CPU_ENDIAN_BIG		__BIG_ENDIAN
#define CPU_ENDIAN_PDP		__PDP_ENDIAN
#define CPU_ENDIAN			__BYTE_ORDER

#ifndef	NULL
#  define	NULL	0
#endif	// NULL


// Fixed size data types
typedef signed char		Int8;
typedef signed short	Int16;	
typedef signed long		Int32;

typedef unsigned char	UInt8;
typedef unsigned short  UInt16;
typedef unsigned long   UInt32;


// Logical data types
typedef unsigned char	Boolean;

typedef char			Char;
typedef UInt16			WChar;		// 'wide' int'l character type.

typedef UInt16			Err;

typedef UInt32			LocalID;	// local (card relative) chunk ID

typedef Int16 			Coord;		// screen/window coordinate


typedef void *			MemPtr;		// global pointer
typedef struct _opaque*	MemHandle;	// global handle


#endif	// __TYPE_H__
