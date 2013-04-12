// --------------------------------------------------------------------------
INTERFACE [arm && sun4i]:

#include "kmem.h"
#include "irq_chip.h"

EXTENSION class Timer
{
public:
  static unsigned irq() { return 22; }

private:
  enum {    
    TMR_IRQ_EN       = Kmem::Timer_map_base + 0x00,
    TMR_IRQ_STA      = Kmem::Timer_map_base + 0x04,
    TMR0_CTRL        = Kmem::Timer_map_base + 0x10,
    TMR0_INTV        = Kmem::Timer_map_base + 0x14,
    TMR0_CUR_VAL     = Kmem::Timer_map_base + 0x18,

    TIMER_MODE       = (0 << 7),        /* continuous mode                  */
    TIMER_DIV        = (4 << 4),        /* pre scale 16                     */
    TIMER_SRC        = (1 << 2),        /* clock source osc24m              */
    TIMER_RELOAD     = (1 << 1),        /* reload interval value            */
    TIMER_EN         = (1 << 0),        /* enable timer                     */

    SYS_TIMER_SCAL   = (16),            /* timer clock source pre-division */
    SYS_TIMER_CLKSRC = (24000000),      /* timer clock source               */
    TMR_INTER_VAL    = (SYS_TIMER_CLKSRC/(SYS_TIMER_SCAL*1000)),
  };
};

// ----------------------------------------------------------------------
IMPLEMENTATION [arm && sun4i]:

#include "config.h"
#include "kip.h"
#include "io.h"

IMPLEMENT
void Timer::init(unsigned)
{
  Io::write<Mword>(TMR_INTER_VAL, TMR0_INTV);

  /* set clock sourch to HOSC, 16 pre-division */
  Mword val = Io::read<Mword>(TMR0_CTRL);
  val &= ~(0x07<<4);
  val &= ~(0x03<<2);
  val |= (4<<4) | (1<<2);
  Io::write<Mword>(val, TMR0_CTRL);

  /* set mode to auto reload */
  val = Io::read<Mword>(TMR0_CTRL);
  val &= ~(1<<7);    /* Continuous mode */  
  val |= (1<<1);
  Io::write<Mword>(val, TMR0_CTRL);

  /* Enable time0 interrupt */
  val = Io::read<Mword>(TMR_IRQ_EN);
  val |= (1<<0);
  Io::write<Mword>(val, TMR_IRQ_EN);
}

static inline
Unsigned64
Timer::timer_to_us(Unsigned32 /*cr*/)
{ return 0; }

static inline
Unsigned64
Timer::us_to_timer(Unsigned64 us)
{ (void)us; return 0; }

PUBLIC static inline NEEDS["io.h"]
void
Timer::acknowledge()
{
  Io::write<Mword>(1, TMR_IRQ_STA);
}

IMPLEMENT inline NEEDS["kip.h", "io.h", Timer::timer_to_us, Timer::us_to_timer]
void
Timer::update_one_shot(Unsigned64 /*wakeup*/)
{
}

IMPLEMENT inline NEEDS["config.h", "kip.h", "io.h", Timer::timer_to_us]
Unsigned64
Timer::system_clock()
{
  if (Config::Scheduler_one_shot)
    return 0;
  else
    return Kip::k()->clock;
}
