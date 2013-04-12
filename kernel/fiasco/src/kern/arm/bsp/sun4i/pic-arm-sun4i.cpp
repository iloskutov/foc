// ---------------------------------------------------------------------
INTERFACE [arm && sun4i]:

#include "kmem.h"

class Irq_base;

EXTENSION class Pic
{
public:
  enum
  {
    INT_VECTOR_REG       = (Kmem::Intc_map_base + 0x00),
    INT_BASE_ADR_REG     = (Kmem::Intc_map_base + 0x04),
    INT_PROTECTION_REG   = (Kmem::Intc_map_base + 0x08),
    INT_NMI_CTRL_REG     = (Kmem::Intc_map_base + 0x0c),
    INT_IRQ_PENDING_REG0 = (Kmem::Intc_map_base + 0x10),
    INT_IRQ_PENDING_REG1 = (Kmem::Intc_map_base + 0x14),
    INT_IRQ_PENDING_REG2 = (Kmem::Intc_map_base + 0x18),

    INT_FIQ_PENDING_REG0 = (Kmem::Intc_map_base + 0x20),
    INT_FIQ_PENDING_REG1 = (Kmem::Intc_map_base + 0x24),
    INT_FIQ_PENDING_REG2 = (Kmem::Intc_map_base + 0x28),

    INT_SELECT_REG0      = (Kmem::Intc_map_base + 0x30),
    INT_SELECT_REG1      = (Kmem::Intc_map_base + 0x34),
    INT_SELECT_REG2      = (Kmem::Intc_map_base + 0x38),

    INT_ENABLE_REG0      = (Kmem::Intc_map_base + 0x40),
    INT_ENABLE_REG1      = (Kmem::Intc_map_base + 0x44),
    INT_ENABLE_REG2      = (Kmem::Intc_map_base + 0x48),

    INT_MASK_REG0        = (Kmem::Intc_map_base + 0x50),
    INT_MASK_REG1        = (Kmem::Intc_map_base + 0x54),
    INT_MASK_REG2        = (Kmem::Intc_map_base + 0x58),

    INT_RESP_REG0        = (Kmem::Intc_map_base + 0x60),
    INT_RESP_REG1        = (Kmem::Intc_map_base + 0x64),
    INT_RESP_REG2        = (Kmem::Intc_map_base + 0x68),

    INT_FASTFORCE_REG0   = (Kmem::Intc_map_base + 0x70),
    INT_FASTFORCE_REG1   = (Kmem::Intc_map_base + 0x74),
    INT_FASTFORCE_REG2   = (Kmem::Intc_map_base + 0x78),

    INT_SRCPRIO_REG0     = (Kmem::Intc_map_base + 0x80),
    INT_SRCPRIO_REG1     = (Kmem::Intc_map_base + 0x84),
    INT_SRCPRIO_REG2     = (Kmem::Intc_map_base + 0x88),
    INT_SRCPRIO_REG3     = (Kmem::Intc_map_base + 0x8c),
    INT_SRCPRIO_REG4     = (Kmem::Intc_map_base + 0x90),
    
    INT_IRQNO_ENMI       = 0,
  };
};

// ---------------------------------------------------------------------
IMPLEMENTATION [arm && sun4i]:

#include <cstdio>
#include "io.h"
#include "irq_chip_generic.h"
#include "irq_mgr.h"

class Irq_chip_arm_sun4i : public Irq_chip_gen
{
public:
  Irq_chip_arm_sun4i() : Irq_chip_gen(96) {}
  unsigned set_mode(Mword, unsigned) { return Irq_base::Trigger_level; }
  void set_cpu(Mword, unsigned) {}
};

PUBLIC
void
Irq_chip_arm_sun4i::mask(Mword irq)
{
  assert(cpu_lock.test());
  if(irq < 32){
    Io::write<Mword>(Io::read<Mword>(Pic::INT_ENABLE_REG0) & ~(1<<irq), Pic::INT_ENABLE_REG0);
    Io::write<Mword>(Io::read<Mword>(Pic::INT_MASK_REG0) | (1 << irq), Pic::INT_MASK_REG0);
  } else if(irq < 64){
    irq -= 32;
    Io::write<Mword>(Io::read<Mword>(Pic::INT_ENABLE_REG1) & ~(1<<irq), Pic::INT_ENABLE_REG1);
    Io::write<Mword>(Io::read<Mword>(Pic::INT_MASK_REG1) | (1 << irq), Pic::INT_MASK_REG1);
  } else if(irq < 96){
    irq -= 64;
    Io::write<Mword>(Io::read<Mword>(Pic::INT_ENABLE_REG2) & ~(1<<irq), Pic::INT_ENABLE_REG2);
    Io::write<Mword>(Io::read<Mword>(Pic::INT_MASK_REG2) | (1 << irq), Pic::INT_MASK_REG2);
  }
}

PUBLIC
void
Irq_chip_arm_sun4i::mask_and_ack(Mword irq)
{
  assert(cpu_lock.test());
  if(irq < 32){
    Io::write<Mword>(Io::read<Mword>(Pic::INT_ENABLE_REG0) & ~(1<<irq), Pic::INT_ENABLE_REG0);
    Io::write<Mword>(Io::read<Mword>(Pic::INT_MASK_REG0) | (1 << irq), Pic::INT_MASK_REG0);
    Io::write<Mword>(Io::read<Mword>(Pic::INT_IRQ_PENDING_REG0) | (1<<irq), Pic::INT_IRQ_PENDING_REG0);
  } else if(irq < 64){
    irq -= 32;
    Io::write<Mword>(Io::read<Mword>(Pic::INT_ENABLE_REG1) & ~(1<<irq), Pic::INT_ENABLE_REG1);
    Io::write<Mword>(Io::read<Mword>(Pic::INT_MASK_REG1) | (1 << irq), Pic::INT_MASK_REG1);
    Io::write<Mword>(Io::read<Mword>(Pic::INT_IRQ_PENDING_REG1) | (1<<irq), Pic::INT_IRQ_PENDING_REG1);
  } else if(irq < 96){
    irq -= 64;
    Io::write<Mword>(Io::read<Mword>(Pic::INT_ENABLE_REG2) & ~(1<<irq), Pic::INT_ENABLE_REG2);
    Io::write<Mword>(Io::read<Mword>(Pic::INT_MASK_REG2) | (1 << irq), Pic::INT_MASK_REG2);
    Io::write<Mword>(Io::read<Mword>(Pic::INT_IRQ_PENDING_REG2) | (1<<irq), Pic::INT_IRQ_PENDING_REG2);
  }
}

PUBLIC
void
Irq_chip_arm_sun4i::ack(Mword irq)
{
  if(irq < 32){
    Io::write<Mword>(Io::read<Mword>(Pic::INT_IRQ_PENDING_REG0) | (1<<irq), Pic::INT_IRQ_PENDING_REG0);
  } else if(irq < 64){
    irq -= 32;
    Io::write<Mword>(Io::read<Mword>(Pic::INT_IRQ_PENDING_REG1) | (1<<irq), Pic::INT_IRQ_PENDING_REG1);
  } else if(irq < 96){
    irq -= 64;
    Io::write<Mword>(Io::read<Mword>(Pic::INT_IRQ_PENDING_REG2) | (1<<irq), Pic::INT_IRQ_PENDING_REG2);
  }
}

PUBLIC
void
Irq_chip_arm_sun4i::unmask(Mword irq)
{
  assert (cpu_lock.test());
  if(irq < 32){
    Io::write<Mword>(Io::read<Mword>(Pic::INT_ENABLE_REG0) | (1<<irq), Pic::INT_ENABLE_REG0);
    Io::write<Mword>(Io::read<Mword>(Pic::INT_MASK_REG0) & ~(1 << irq), Pic::INT_MASK_REG0);
	/* must clear pending bit when enabled */
    if(irq == Pic::INT_IRQNO_ENMI)
      Io::write<Mword>((1 << Pic::INT_IRQNO_ENMI), Pic::INT_IRQ_PENDING_REG0);
  } else if(irq < 64){
    irq -= 32;
    Io::write<Mword>(Io::read<Mword>(Pic::INT_ENABLE_REG1) | (1<<irq), Pic::INT_ENABLE_REG1);
    Io::write<Mword>(Io::read<Mword>(Pic::INT_MASK_REG1) & ~(1 << irq), Pic::INT_MASK_REG1);
    Io::write<Mword>(Io::read<Mword>(Pic::INT_IRQ_PENDING_REG1) | (1<<irq), Pic::INT_IRQ_PENDING_REG1);
  } else if(irq < 96){
    irq -= 64;
    Io::write<Mword>(Io::read<Mword>(Pic::INT_ENABLE_REG2) | (1<<irq), Pic::INT_ENABLE_REG2);
    Io::write<Mword>(Io::read<Mword>(Pic::INT_MASK_REG2) & ~(1 << irq), Pic::INT_MASK_REG2);
    Io::write<Mword>(Io::read<Mword>(Pic::INT_IRQ_PENDING_REG2) | (1<<irq), Pic::INT_IRQ_PENDING_REG2);
  }
}

static Static_object<Irq_mgr_single_chip<Irq_chip_arm_sun4i> > mgr;


IMPLEMENT FIASCO_INIT
void Pic::init()
{
  /* Disable & clear all interrupts */
  Io::write<Mword>(0, Pic::INT_ENABLE_REG0);
  Io::write<Mword>(0, Pic::INT_ENABLE_REG1);
  Io::write<Mword>(0, Pic::INT_ENABLE_REG2);

  Io::write<Mword>(0xffffffff, Pic::INT_MASK_REG0);
  Io::write<Mword>(0xffffffff, Pic::INT_MASK_REG1);
  Io::write<Mword>(0xffffffff, Pic::INT_MASK_REG2);

  Io::write<Mword>(0xffffffff, Pic::INT_IRQ_PENDING_REG0);
  Io::write<Mword>(0xffffffff, Pic::INT_IRQ_PENDING_REG1);
  Io::write<Mword>(0xffffffff, Pic::INT_IRQ_PENDING_REG2);
  Io::write<Mword>(0xffffffff, Pic::INT_FIQ_PENDING_REG0);
  Io::write<Mword>(0xffffffff, Pic::INT_FIQ_PENDING_REG1);
  Io::write<Mword>(0xffffffff, Pic::INT_FIQ_PENDING_REG2);

  /* enable protection mode */
  Io::write<Mword>(0x01, Pic::INT_PROTECTION_REG);
  /* config the external interrupt source type */
  Io::write<Mword>(0x00, Pic::INT_NMI_CTRL_REG);

  Irq_mgr::mgr = mgr.construct();
}

IMPLEMENT inline
Pic::Status Pic::disable_all_save()
{
  Status s = 0;
  return s;
}

IMPLEMENT inline
void Pic::restore_all(Status)
{}

PUBLIC static inline NEEDS["io.h"]
Unsigned32 Irq_chip_arm_sun4i::pending()
{
  for (int n = 0; n < 3; ++n)
    {
      unsigned long x = Io::read<Mword>(Pic::INT_IRQ_PENDING_REG0 + 0x04 * n);
      for (int i = 0; i < 32; ++i)
        if (x & (1 << i))
          return i + n * 32;
    }
  return 0;
}

PUBLIC inline NEEDS[Irq_chip_arm_sun4i::pending]
void
Irq_chip_arm_sun4i::irq_handler()
{
  Unsigned32 p = pending();
  if (EXPECT_TRUE(p != 0xffff))
    handle_irq<Irq_chip_arm_sun4i>(p, 0);
}

extern "C"
void irq_handler()
{ mgr->c.irq_handler(); }

//---------------------------------------------------------------------------
IMPLEMENTATION [debug && sun4i]:

PUBLIC
char const *
Irq_chip_arm_sun4i::chip_type() const
{ return "HW SUN4I IRQ"; }
