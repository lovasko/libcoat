#include <stdlib.h>
#include <unistd.h>
#include <coat.h>

#define WAIT_TIME 60

/**
  * Query the OS for the system load average over the last minute and
  * store the values in a GoatSwim raw format.
**/
int
main(void)
{
  coat c;
  double avg;

  coat_open_file(&c, "load.gs", NULL, 0);

  while (1) {
    getloadavg(&avg, 1);
    coat_write(&c, (float)avg);
    sleep(WAIT_TIME);
  }

  return EXIT_SUCCESS;
}
