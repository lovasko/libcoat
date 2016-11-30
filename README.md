# libcoat
The `libcoat` a client-side library for the
[GoatSwim](https://github.com/lovasko/goat) time series database.

## Features
* fully C89/POSIX2001 compatible
* API contains only 4 functions
* optional buffering to reduce the number of `write` calls
* optional custom memory management
* no loops used (guaranteed non-blocking flow)

## Example
The following [examples/load.c](example) will collect the system load over
the last minute and write it to the disk in a GoatSwim raw format:

```c
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
```

## Build & install
```
$ make
$ make install
```

## License
The `libcoat` library is licensed under the [2-clause BSD
license](LICENSE). In case, for any reason, you need a different license,
feel free to contact the author.

## Author
Daniel Lovasko <daniel.lovasko@gmail.com>
