#ifndef _buffer_lib_h
#define _buffer_lib_h

void buffer_print(int*, int);

int buffer_distribute(int, int*, int*, int);

void buffer_fill(int*, int, int, int);

void buffer_copy(int*, int*, int, int);

int *buffer_slice(int*, int, int, int);

int buffer_max(int*, int);

int buffer_sum(int*, int, int);

#endif