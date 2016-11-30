#include <stdlib.h>
#include <unistd.h>
#include <coat.h>

int
main(void)
{
  coat c;
	double avg;

  coat_open(&c, "load.gs", NULL, 0);

  while (1) {
	  getloadavg(&avg, 1);
    coat_write(&c, (float)avg);
		sleep(60);
  }

	return EXIT_SUCCESS;
}
