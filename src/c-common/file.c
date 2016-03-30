#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <stdint.h>
#include <netinet/in.h>

#include "memory.h"
#include "file.h"

bool file_exists_p(const char *filename)
{
  struct stat buf;
  int err = stat(filename, &buf);
  return err == 0;
}

#define GENERATE_STAT_FIELD(type,name)			\
  type stat_##name(const char *filename)		\
  {							\
    struct stat buf;					\
    int err = stat(filename, &buf);			\
    if(err) {						\
      fprintf(stderr, "stat() failed: %s\n", filename);	\
      exit(EXIT_FAILURE);						\
    }							\
    return buf.st_##name;				\
  }

GENERATE_STAT_FIELD(time_t, mtime);
GENERATE_STAT_FIELD(size_t, size);

int xpipe(int filedes[2])
{
  int err = pipe(filedes);
  if(err) {
    perror("pipe() failed");
    exit(EXIT_FAILURE);
  }
  return err;
}

ssize_t xwrite(int filedes, const void *buffer, size_t size)
{
  ssize_t err = write(filedes, buffer, size);
  if(err == -1) {
    perror("write() failed");
    exit(EXIT_FAILURE);
  }
  return err;
}

ssize_t xread(int filedes, void *buffer, size_t size)
{
  ssize_t err = read(filedes, buffer, size);
  if(err == -1) {
    perror("read() failed");
    exit(EXIT_FAILURE);
  }
  return err;
}
