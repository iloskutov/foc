IMPLEMENTATION [arm && sun4i]:

#include "io.h"
#include "kmem.h"

void __attribute__ ((noreturn))
platform_reset(void)
{
  enum {
    WDOG_MODE  = Kmem::Watchdog_map_base + 4,
  };

  Io::write<Unsigned32>(3, WDOG_MODE);

  for (;;)
    ;
}