#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "lib/buffer_lib.h"

int buffer_fill(int *buffer, int buffer_length, int from, int to) {
    srand(time(NULL));
    
    for (int i = 0; i < buffer_length; i ++) {
        buffer[i] = (rand() % (to - from + 1)) + from;
    }
    
    return (EXIT_SUCCESS);
}

int buffer_copy(int *buffer, int *source, int source_length, int to) {
	for (int i = 0; i < source_length; i ++) {
		buffer[to + i] = source[i];
	}

	return (EXIT_SUCCESS);
}

int *buffer_slice(int *buffer, int buffer_length, int from, int target_length) {
	int *output;
	output = (int*) malloc(target_length * sizeof(int));

	for (int i = 0; i < target_length; i ++) {
		output[i] = buffer[from + i];
	}

	return output;
}

int buffer_max(int *buffer, int buffer_length, int *maximum) {
	*maximum = buffer[0];
	for (int i = 1; i < buffer_length; i ++) {
		if (buffer[i] > *maximum) {
			*maximum = buffer[i];
		}
	}

	return (EXIT_SUCCESS);
}

int buffer_sum(int *buffer, int from, int to) {
	int summary = 0;
	for (int i = from; i < to; i ++) summary += buffer[i];
	return summary;
}