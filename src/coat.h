#ifndef COAT_H
#define COAT_H

#include <stdint.h>

/* status codes */
#define COAT_OK     0
#define COAT_E_NULL 1
#define COAT_E_IO   2

/*
 * libcoat is a C89 library that provides storage of time series data
 * into a file or a file descriptor in the GoatSwim raw format.
*/

/**
 * Coat instance handling storage and buffering.
 * This data type is intended to be treated as an opaque structure.
*/
typedef struct coat {
  uint32_t* buf_mem; /**< Buffer memory.              */
  uint32_t  buf_cnt; /**< Number of buffer entries.   */
  uint32_t  cnt;     /**< Current buffer entry count. */
  int       fd;      /**< Storage file descriptor.    */
  int       buf_own; /**< Buffer memory ownership.    */
  int       fd_own;  /**< File descriptor ownership.  */
  int       padding; /**< Dummy structure padding.    */
} coat;

/**
 * Initialize the file storage for data points.
 *
 * It is possible to use a custom buffer (perhaps allocated on the stack)
 * as the backing memory for the buffering. It is expected of such buffer
 * to be of necessary size, precisely >= buf_cnt * 8.
 *
 * @param[in] c       coat instance (not allocated here)
 * @param[in] path    file path (alternative to fd)
 * @param[in] fd      file descriptor (alternative to path)
 * @param[in] buf_mem pointer to custom buffer (allocated internally if NULL)
 * @param[in] buf_cnt custom buffer entry count (zero for no buffer usage)
 *
 * @return status code
 * @retval COAT_OK     success
 * @retval COAT_E_NULL c and/or path are NULL
 * @retval COAT_E_IO   error while opening the file (check errno)
*/
int coat_open_file(coat* c, char* path, void* buf_mem, uint32_t buf_cnt);
int coat_open_fd(coat* c, int fd, void* buf_mem, uint32_t buf_cnt);

/**
 * Store a data point.
 *
 * NOTE: In case that the coat instance is using buffering, such point
 * might not appear on the disk right away.
 *
 * @param[in] c   coat instance
 * @param[in] ts  data point timestamp in 2000 epoch (implicit: current time)
 * @param[in] val data point value
 *
 * @return status code
 * @retval COAT_OK     success
 * @retval COAT_E_NULL c is NULL
 * @retval COAT_E_IO   error while writing the data (check errno)
*/
int coat_write(coat* c, float val);
int coat_write_ex(coat* c, uint32_t ts, float val);

/**
 * Free all resources held by the coat instance. Custom memory used for
 * data point buffering is not being freed.
 *
 * @param[in] c coat instance
 *
 * @return status code
 * @retval COAT_OK     success
 * @retval COAT_E_NULL c is NULL
 * @retval COAT_E_IO   error while closing the file descriptor (check errno)
*/
int coat_close(coat* c);

#endif
