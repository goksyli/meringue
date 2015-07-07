#include "util.h"


void simple_va_fun(int i, ...)
{
	va_list arg_ptr;
	int j = 0;

	va_start(arg_ptr,i);
	j = va_arg(arg_ptr,int);
	va_end(arg_ptr);
	printf("%d %d\n",i,j);
	return;
}