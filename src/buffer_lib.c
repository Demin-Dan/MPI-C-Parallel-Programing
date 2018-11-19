
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "lib/buffer_lib.h"

void buffer_print(int *buf, int len) {
	for (int i = 0; i < len; i ++) printf("%d ", buf[i]); printf("\n");
}

int buffer_distribute(int len, int *lens, int *inds, int dby) {
	int llen = len / dby >= 1 ? ceil(len / (1.0 * dby)) : len % dby;
	int rlen, off;
	for (int i = 0; i < dby; i ++) {
		rlen = len - i * llen;
		lens[i] = rlen >= llen ? llen : rlen;
		if (lens[i] <= 0) lens[i] = 0;
		off = buffer_sum(lens, 0, i);
		inds[i] = off < len ? off : 0;
	}
	if (len - dby == 1) lens[dby - 1] ++;
	return llen;
}

void buffer_fill(int *buf, int len, int st, int en) {
    for (int i = 0; i < len; i ++) buf[i] = (rand() % (en - st + 1)) + st;
}

void buffer_copy(int *buf, int *src, int slen, int bst) {
	for (int i = 0; i < slen; i ++) buf[bst + i] = src[i];
}

int *buffer_slice(int *buf, int len, int st, int tlen) {
	int *ot; ot = (int*) malloc(tlen * sizeof(int));
	for (int i = 0; i < tlen; i ++) ot[i] = buf[st + i];
	return ot;
}

int buffer_max(int *buf, int len) {
	int mx = buf[0];
	for (int i = 1; i < len; i ++) if (buf[i] > mx) mx = buf[i];
	return mx;
}

int buffer_min(int *buf, int len) {
	int mn = buf[0];
	for (int i = 1; i < len; i ++) if (buf[i] < mn) mn = buf[i];
	return mn;
}

int buffer_sum(int *buf, int st, int en) {
	int sm = 0;
	for (int i = st; i < en; i ++) sm += buf[i];
	return sm;
}
