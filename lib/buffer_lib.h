#ifndef _buffer_lib_h
#define _buffer_lib_h

int buffer_fill(int*, int, int, int);

int buffer_copy(int*, int*, int, int);

int *buffer_slice(int*, int, int, int);

int buffer_max(int*, int, int*);

int buffer_sum(int*, int, int);

#endif