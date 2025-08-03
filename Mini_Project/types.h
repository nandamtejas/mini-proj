// types.h

#ifndef _TYPES_H
#define _TYPES_H

typedef unsigned char uint8;
typedef char int8;
typedef unsigned short int uint16;
typedef short int int16;
typedef unsigned int uint32;
typedef int int32;
/*
	********************************************
	**  long int has the same size of int     **
	**  No need for typedefinition for        **
	**  long int                              **
	********************************************

// typedef unsigned long int uint64;
*/

typedef long int int64;
typedef float real32;
typedef double real64;

/*
	**************************************************
	**  long double has the same size of double     **
	**  No need for typedefinition for              **
	**  long int                                    **
	**************************************************
// typedef long double real128;
*/

#endif
