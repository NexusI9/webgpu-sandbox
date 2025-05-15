#include "file.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>


void namefile_from_path(const char *path, char *dest, size_t max_length) {

  const char *base = strrchr(path, '/');
  base = base ? base + 1 : path;

  // check if name contain "." (.obj)
  const char *dot = strchr(base, '.');

  // get length on filename before the dot
  size_t length = dot ? (size_t)(dot - base) : strlen(base);

  // trim name
  if (length > max_length)
    length = max_length - 1;

  strncpy(dest, base, length);
  dest[length] = '\0';
}

/**
   Traverse file lines a push values to the given list based on the list prefix
   attribute
 */
void file_read_line_prefix(FILE *file, const char *prefix,
                           file_read_line_prefix_callback callback,
                           void *callback_data) {

  // get length of attribute based on list prefix
  char line[512];
  size_t prefix_len = strlen(prefix);
  while (fgets(line, sizeof(line), file)) {

    // check if prefix match line start
    int match_prefix = true;
    for (size_t l = 0; l < prefix_len; l++) {
      if (line[l] != prefix[l]) {
        match_prefix = false;
        break;
      }
    }

    // skip to next line if doesn't match prefix
    if (!match_prefix)
      continue;

    callback(line, callback_data);
  }
  rewind(file);
}



