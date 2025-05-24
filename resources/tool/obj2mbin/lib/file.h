#ifndef _FILE_H_
#define _FILE_H_

#include <stdio.h>
#include <stdlib.h>

typedef void (*file_read_line_prefix_callback)(const char *, void *);

#define FILE_OPEN_FAIL 1

void namefile_from_path(const char *, char *, size_t);
void file_read_line_prefix(FILE *, const char *, file_read_line_prefix_callback,
                           void *);


#endif
