#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <coat.h>

#define WAIT_TIME 60

static void
err(int ret, char* name)
{
  if (ret == COAT_E_NULL)
    fprintf(stderr, "%s: null error", name);

  if (ret == COAT_E_IO)
    perror(name);
}

/**
  * Query the OS for the system load average over the last minute and
  * store the values in a GoatSwim raw format.
**/
int
main(void)
{
  coat c;
  double avg;
  int ret;

  ret = coat_open_file(&c, "load.gs", NULL, 0);
  if (ret != COAT_OK) {
    err(ret, "open_file");
    return EXIT_FAILURE;
  }

  while (1) {
    getloadavg(&avg, 1);
    printf("%f\n", (float)avg);
    ret = coat_write(&c, (float)avg);
    if (ret != COAT_OK) {
      err(ret, "write");
      break;
    }

    sleep(WAIT_TIME);
  }

  ret = coat_close(&c);
  if (ret != COAT_OK) {
    err(ret, "close");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
