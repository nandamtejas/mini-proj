// types_test.c

/* This file is for types.h test purpose only */

#include "../types.h"

// a variable which can store the size of each type
unsigned long int size;

/*
Output can be viewed in the Watch window
*/

int main()
{
	// unsigned char -> uint8
	size = sizeof(uint8); // 1
	// char -> int8
	size = sizeof(int8); // 1
	// unsigned short int -> uint16
	size = sizeof(uint16); // 2
	// short int -> int16
	size = sizeof(int16); // 2
	// unsigned int -> uint32
	size = sizeof(uint32); // 4
	// int -> int32
	size = sizeof(int32); // 4
	/*
	********************************************
	**  long int has the same size of int     **
	**  No need for typedefinition for        **
	**  long int                              **
	********************************************
	*/
	
	/*
	// unsigned long int -> uint64
	size = sizeof(uint64); // 4
	// long int -> int64
	size = sizeof(int64); // 4
	*/
	
	// float -> real32
	size = sizeof(real32); // 4
	// double -> real64
	size = sizeof(real64); // 8
	
	/*
	**************************************************
	**  long double has the same size of double     **
	**  No need for typedefinition for              **
	**  long int                                    **
	**************************************************
	*/
	
	/*
	// long double -> real128
	size = sizeof(real128); // 8
	*/
	
}
