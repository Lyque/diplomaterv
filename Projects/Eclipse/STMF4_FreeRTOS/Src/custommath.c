/*
 * custommath.c
 *
 *  Created on: Sep 25, 2016
 *      Author: lyque
 */
#include "custommath.h"

int32_t ipower10(int pow)
{
	int i = 0;
	int32_t result = 1;

	for(i=0;i<pow;i++)
	{
		result = 10*result;
	}

	return result;
}
