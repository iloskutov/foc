IMPLEMENTATION [arm && sun4i]:

#include "io.h"
#include "kmem.h"

void __attribute__ ((noreturn))
platform_reset(void)
{
  enum {
    WDOG_MODE  = 0x94,
  };

  Io::write<Unsigned32>(3, Kmem::mmio_remap(Kmem::Timer_phys_base) + WDOG_MODE);

  for (;;)
    ;
}
