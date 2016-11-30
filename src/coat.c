#include <arpa/inet.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "coat.h"

/* Flush all buffered data points into the file storage. */
static void
coat_flush(coat* c)
{
  write(c->fd, c->buf_mem, c->cnt * 8);
  memset(c->buf_mem, '\0', c->buf_cnt * 8);
  c->cnt = 0;
}

int
coat_open(coat* c, char* path, void* buf_mem, uint32_t buf_cnt)
{
	mode_t mode;

  if (c == NULL || path == NULL)
    return COAT_E_NULL;

  /* open story file for appending */
  errno = 0;
	mode = S_IRWXU | S_IRWXG;
	c->fd = open(path, O_APPEND | O_CREAT | O_WRONLY, mode);
  if (c->fd == -1)
    return COAT_E_NULL;

  /* create buffer for metrics */
  if (buf_mem == NULL && buf_cnt > 0) {
    c->buf_mem = malloc(buf_cnt * 8);
    c->buf_own = 1;
  } else {
    c->buf_mem = buf_mem;
    c->buf_own = 0;
  }

  c->buf_cnt = buf_cnt;
  c->cnt = 0;
  return COAT_OK;
}

int
coat_write(coat* c, float val)
{
  uint32_t store_time;
  uint32_t store_value;

  if (c == NULL)
    return COAT_E_NULL;

  /* convert data to big-endian */
  store_time  = htonl((uint32_t)time(NULL));
  store_value = htonl(*(uint32_t*)&val);

  /* store the data point */
  if (c->buf_cnt == 0) {
    write(c->fd, &store_time,  4);
    write(c->fd, &store_value, 4);
  } else {
    c->buf_mem[c->cnt]   = store_time;
    c->buf_mem[c->cnt+1] = store_value;
    c->cnt++;
    
    if (c->cnt == c->buf_cnt)
      coat_flush(c);
  }

  return COAT_OK;
}

int
coat_close(coat* c)
{
  if (c == NULL)
    return COAT_E_NULL;

  coat_flush(c);  

  if (c->buf_own)
    free(c->buf_mem);

  close(c->fd);
  return COAT_OK;
}
