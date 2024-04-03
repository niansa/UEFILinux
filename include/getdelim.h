#ifndef _GETDELIM_H
#define _GETDELIM_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>



static __attribute__((unused))
ssize_t my_getdelim(char **out, size_t *out_size, int delim, int in)
{
	size_t out_idx = 0;
	// Allocate buffer if needed
	if (*out == NULL) {
		*out = malloc(2);
		*out_size = 1;
	}
	// Loop until delimiter
	for (;;) {
		// Read single character
		char c;
		if (read(in, &c, 1) <= 0) {
			return -1;
		}
		// Add it to buffer
		if (*out_size < out_idx + 1) {
			(*out_size)++;
			*out = realloc(*out, *out_size + 1);
		}
		(*out)[out_idx++] = c;
		// Break on delimiter
		if (c == (char)delim) {
			break;
		}
	}
	// Add null terminator
	(*out)[out_idx] = '\0';
	// Return success
	return out_idx + 1;
}

static inline __attribute__((unused))
ssize_t my_getline(char **out, size_t *out_size, int in)
{
	return my_getdelim(out, out_size, '\n', in);
}

#endif /* _GETDELIM_H */
