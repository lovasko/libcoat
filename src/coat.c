#include <arpa/inet.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "coat.h"

/* GoatSwim expects timestamps to be in 2000 Epoch. */
#define COAT_EPOCH 946684800

/* Flush all buffered data points into the file storage. */
static int
coat_flush(coat* c)
{
  errno = 0;
  if (write(c->fd, c->buf_mem, c->cnt * 8) < c->cnt * 8)
    return COAT_E_IO;

  memset(c->buf_mem, '\0', c->buf_cnt * 8);
  c->cnt = 0;

  return COAT_OK;
}

/* Perform the common instance initialization. */
static int
coat_buf_init(coat* c, void* buf_mem, uint32_t buf_cnt)
{
  /* Buffer is irrelevant, if no buffering is selected. */
  if (buf_cnt == 0) {
    c->buf_mem = NULL;
    c->buf_cnt = 0;
    c->buf_own = 0;
    return COAT_OK;
  }

  if (buf_mem == NULL) {
    errno = 0;
    c->buf_mem = malloc(buf_cnt * 8);
    if (c->buf_mem == NULL)
      return COAT_E_IO;

    c->buf_cnt = buf_cnt;
    c->buf_own = 1;
  } else {
    c->buf_mem = buf_mem;
    c->buf_cnt = buf_cnt;
    c->buf_own = 0;
  }

  return COAT_OK;
}

/* Common data point storage procedure. */
static int
coat_store(coat* c, uint32_t ts, float val)
{
  uint32_t store_time;
  uint32_t store_value;
  ssize_t nbytes;
  int ret;

  /* convert data to big-endian */
  store_time  = htonl(ts);
  store_value = htonl(*(uint32_t*)&val);

  /* store the data point */
  if (c->buf_cnt == 0) {
    errno = 0;
    nbytes = write(c->fd, &store_time,  4);
    if (nbytes != 4)
      return COAT_E_IO;

    errno = 0;
    nbytes = write(c->fd, &store_value, 4);
    if (nbytes != 4)
      return COAT_E_IO;
  } else {
    c->buf_mem[c->cnt*2]   = store_time;
    c->buf_mem[c->cnt*2+1] = store_value;
    c->cnt++;

    if (c->cnt == c->buf_cnt) {
      ret = coat_flush(c);
      if (ret != COAT_OK)
        return ret;
    }
  }

  return COAT_OK;
}

int
coat_open_file(coat* c, char* path, void* buf_mem, uint32_t buf_cnt)
{
  int ret;
  int flags;
  mode_t mode;

  if (c == NULL || path == NULL)
    return COAT_E_NULL;

  ret = coat_buf_init(c, buf_mem, buf_cnt);
  if (ret != COAT_OK)
    return ret;

  errno = 0;
  mode = S_IRWXU | S_IRWXG;
  flags = O_APPEND | O_CREAT | O_WRONLY;
  c->fd = open(path, flags, mode);
  if (c->fd == -1)
    return COAT_E_IO;

  c->cnt = 0;
  c->fd_own = 1;

  return COAT_OK;
}

int
coat_open_fd(coat* c, int fd, void* buf_mem, uint32_t buf_cnt)
{
  if (c == NULL)
    return COAT_E_NULL;

  if (fd < 0)
    return COAT_E_IO;

  coat_buf_init(c, buf_mem, buf_cnt);

  c->cnt = 0;
  c->fd_own = 0;

  return COAT_OK;
}

int
coat_write(coat* c, float val)
{
  int ret;
  uint32_t ts;

  if (c == NULL)
    return COAT_E_NULL;

  ts = (uint32_t)(time(NULL) - COAT_EPOCH);
  ret = coat_store(c, ts, val);
  if (ret != COAT_OK)
    return ret;

  return COAT_OK;
}

int
coat_write_ex(coat* c, uint32_t ts, float val)
{
  int ret;

  if (c == NULL)
    return COAT_E_NULL;

  ret = coat_store(c, ts, val);
  if (ret != COAT_OK)
    return ret;

  return COAT_OK;
}

int
coat_close(coat* c)
{
  int ret;

  if (c == NULL)
    return COAT_E_NULL;

  ret = coat_flush(c);
  if (ret != COAT_OK)
    return ret;

  if (c->buf_own)
    free(c->buf_mem);

  if (c->fd_own) {
    errno = 0;
    if (close(c->fd) == -1)
      return COAT_E_IO;
  }

  return COAT_OK;
}
