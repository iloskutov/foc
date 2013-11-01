// --------------------------------------------------------------------------
INTERFACE [arm && sun4i]:

#include "kmem.h"
#include "irq_chip.h"
#include "mmio_register_block.h"

EXTENSION class Timer : private Mmio_register_block
{
public:
  static unsigned irq() { return 22; }

private:
  enum {    
    TMR_IRQ_EN       = 0x00,
    TMR_IRQ_STA      = 0x04,
    TMR0_CTRL        = 0x10,
    TMR0_INTV        = 0x14,
    TMR0_CUR_VAL     = 0x18,

    TIMER_MODE       = (0 << 7),        /* continuous mode                  */
    TIMER_DIV        = (4 << 4),        /* pre scale 16                     */
    TIMER_SRC        = (1 << 2),        /* clock source osc24m              */
    TIMER_RELOAD     = (1 << 1),        /* reload interval value            */
    TIMER_EN         = (1 << 0),        /* enable timer                     */

    SYS_TIMER_SCAL   = (16),            /* timer clock source pre-division */
    SYS_TIMER_CLKSRC = (24000000),      /* timer clock source               */
    TMR_INTER_VAL    = (SYS_TIMER_CLKSRC/(SYS_TIMER_SCAL*1000)),
  };

  static Static_object<Timer> _timer;
};

// ----------------------------------------------------------------------
IMPLEMENTATION [arm && sun4i]:

#include "config.h"
#include "kip.h"
#include "io.h"
#include "kmem.h"
#include "mem_layout.h"

Static_object<Timer> Timer::_timer;

PUBLIC
Timer::Timer(Address base) : Mmio_register_block(base)
{
  write<Mword>(TMR_INTER_VAL, TMR0_INTV);

  /* set clock sourch to HOSC, 16 pre-division */
  Mword val = read<Mword>(TMR0_CTRL);
  val &= ~(0x07<<4);
  val &= ~(0x03<<2);
  val |= (4<<4) | (1<<2);
  write<Mword>(val, TMR0_CTRL);

  /* set mode to auto reload */
  val = read<Mword>(TMR0_CTRL);
  val &= ~(1<<7);    /* Continuous mode */  
  val |= (1<<1);
  write<Mword>(val, TMR0_CTRL);

  /* Enable time0 interrupt */
  val = read<Mword>(TMR_IRQ_EN);
  val |= (1<<0);
  write<Mword>(val, TMR_IRQ_EN);
}

IMPLEMENT
void Timer::init(Cpu_number)
{ _timer.construct(Kmem::mmio_remap(Mem_layout::Timer_phys_base)); }

static inline
Unsigned64
Timer::timer_to_us(Unsigned32 /*cr*/)
{ return 0; }

static inline
Unsigned64
Timer::us_to_timer(Unsigned64 us)
{ (void)us; return 0; }

PUBLIC static inline
void
Timer::acknowledge()
{
  _timer->write(1, TMR_IRQ_STA);
}

IMPLEMENT inline
void
Timer::update_one_shot(Unsigned64 wakeup)
{
  (void)wakeup;
}

IMPLEMENT inline NEEDS["kip.h"]
Unsigned64
Timer::system_clock()
{
  if (Config::Scheduler_one_shot)
    return 0;
  else
    return Kip::k()->clock;
}
