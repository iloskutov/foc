// ---------------------------------------------------------------------
IMPLEMENTATION [arm && sun4i]:

#include "assert.h"
#include "initcalls.h"
#include "irq_chip_generic.h"
#include "irq_mgr.h"
#include "mmio_register_block.h"
#include "kmem.h"

class Irq_chip_arm_sun4i : public Irq_chip_gen, Mmio_register_block
{
private:
  enum
  {
    INT_VECTOR_REG       = 0x00,
    INT_BASE_ADR_REG     = 0x04,
    INT_PROTECTION_REG   = 0x08,
    INT_NMI_CTRL_REG     = 0x0c,
    INT_IRQ_PENDING_REG0 = 0x10,
    INT_IRQ_PENDING_REG1 = 0x14,
    INT_IRQ_PENDING_REG2 = 0x18,

    INT_FIQ_PENDING_REG0 = 0x20,
    INT_FIQ_PENDING_REG1 = 0x24,
    INT_FIQ_PENDING_REG2 = 0x28,

    INT_SELECT_REG0      = 0x30,
    INT_SELECT_REG1      = 0x34,
    INT_SELECT_REG2      = 0x38,

    INT_ENABLE_REG0      = 0x40,
    INT_ENABLE_REG1      = 0x44,
    INT_ENABLE_REG2      = 0x48,

    INT_MASK_REG0        = 0x50,
    INT_MASK_REG1        = 0x54,
    INT_MASK_REG2        = 0x58,

    INT_RESP_REG0        = 0x60,
    INT_RESP_REG1        = 0x64,
    INT_RESP_REG2        = 0x68,

    INT_FASTFORCE_REG0   = 0x70,
    INT_FASTFORCE_REG1   = 0x74,
    INT_FASTFORCE_REG2   = 0x78,

    INT_SRCPRIO_REG0     = 0x80,
    INT_SRCPRIO_REG1     = 0x84,
    INT_SRCPRIO_REG2     = 0x88,
    INT_SRCPRIO_REG3     = 0x8c,
    INT_SRCPRIO_REG4     = 0x90,
    
    INT_IRQNO_ENMI       = 0,
  };

public:
  int  set_mode(Mword, Mode) { return 0; }
  bool is_edge_triggered(Mword) const { return false; }
  void set_cpu(Mword, Cpu_number) {}
};

PUBLIC
Irq_chip_arm_sun4i::Irq_chip_arm_sun4i()
: Irq_chip_gen(96),
  Mmio_register_block(Kmem::mmio_remap(Mem_layout::Intc_phys_base))
{
  /* Disable & clear all interrupts */
  write<Mword>(0, INT_ENABLE_REG0);
  write<Mword>(0, INT_ENABLE_REG1);
  write<Mword>(0, INT_ENABLE_REG2);

  write<Mword>(0xffffffff, INT_MASK_REG0);
  write<Mword>(0xffffffff, INT_MASK_REG1);
  write<Mword>(0xffffffff, INT_MASK_REG2);

  write<Mword>(0xffffffff, INT_IRQ_PENDING_REG0);
  write<Mword>(0xffffffff, INT_IRQ_PENDING_REG1);
  write<Mword>(0xffffffff, INT_IRQ_PENDING_REG2);
  write<Mword>(0xffffffff, INT_FIQ_PENDING_REG0);
  write<Mword>(0xffffffff, INT_FIQ_PENDING_REG1);
  write<Mword>(0xffffffff, INT_FIQ_PENDING_REG2);

  /* enable protection mode */
  write<Mword>(0x01, INT_PROTECTION_REG);
  /* config the external interrupt source type */
  write<Mword>(0x00, INT_NMI_CTRL_REG);
}

PUBLIC
void
Irq_chip_arm_sun4i::mask(Mword irq)
{
  assert(cpu_lock.test());
  if(irq < 32){
    write<Mword>(read<Mword>(INT_ENABLE_REG0) & ~(1<<irq), INT_ENABLE_REG0);
    write<Mword>(read<Mword>(INT_MASK_REG0) | (1 << irq), INT_MASK_REG0);
  } else if(irq < 64){
    irq -= 32;
    write<Mword>(read<Mword>(INT_ENABLE_REG1) & ~(1<<irq), INT_ENABLE_REG1);
    write<Mword>(read<Mword>(INT_MASK_REG1) | (1 << irq), INT_MASK_REG1);
  } else if(irq < 96){
    irq -= 64;
    write<Mword>(read<Mword>(INT_ENABLE_REG2) & ~(1<<irq), INT_ENABLE_REG2);
    write<Mword>(read<Mword>(INT_MASK_REG2) | (1 << irq), INT_MASK_REG2);
  }
}

PUBLIC
void
Irq_chip_arm_sun4i::mask_and_ack(Mword irq)
{
  assert(cpu_lock.test());
  if(irq < 32){
    write<Mword>(read<Mword>(INT_ENABLE_REG0) & ~(1<<irq), INT_ENABLE_REG0);
    write<Mword>(read<Mword>(INT_MASK_REG0) | (1 << irq), INT_MASK_REG0);
    write<Mword>(read<Mword>(INT_IRQ_PENDING_REG0) | (1<<irq), INT_IRQ_PENDING_REG0);
  } else if(irq < 64){
    irq -= 32;
    write<Mword>(read<Mword>(INT_ENABLE_REG1) & ~(1<<irq), INT_ENABLE_REG1);
    write<Mword>(read<Mword>(INT_MASK_REG1) | (1 << irq), INT_MASK_REG1);
    write<Mword>(read<Mword>(INT_IRQ_PENDING_REG1) | (1<<irq), INT_IRQ_PENDING_REG1);
  } else if(irq < 96){
    irq -= 64;
    write<Mword>(read<Mword>(INT_ENABLE_REG2) & ~(1<<irq), INT_ENABLE_REG2);
    write<Mword>(read<Mword>(INT_MASK_REG2) | (1 << irq), INT_MASK_REG2);
    write<Mword>(read<Mword>(INT_IRQ_PENDING_REG2) | (1<<irq), INT_IRQ_PENDING_REG2);
  }
}

PUBLIC
void
Irq_chip_arm_sun4i::ack(Mword irq)
{
  if(irq < 32){
    write<Mword>(read<Mword>(INT_IRQ_PENDING_REG0) | (1<<irq), INT_IRQ_PENDING_REG0);
  } else if(irq < 64){
    irq -= 32;
    write<Mword>(read<Mword>(INT_IRQ_PENDING_REG1) | (1<<irq), INT_IRQ_PENDING_REG1);
  } else if(irq < 96){
    irq -= 64;
    write<Mword>(read<Mword>(INT_IRQ_PENDING_REG2) | (1<<irq), INT_IRQ_PENDING_REG2);
  }
}

PUBLIC
void
Irq_chip_arm_sun4i::unmask(Mword irq)
{
  assert (cpu_lock.test());
  if(irq < 32){
    write<Mword>(read<Mword>(INT_ENABLE_REG0) | (1<<irq), INT_ENABLE_REG0);
    write<Mword>(read<Mword>(INT_MASK_REG0) & ~(1 << irq), INT_MASK_REG0);
	/* must clear pending bit when enabled */
    if(irq == INT_IRQNO_ENMI)
      write<Mword>((1 << INT_IRQNO_ENMI), INT_IRQ_PENDING_REG0);
  } else if(irq < 64){
    irq -= 32;
    write<Mword>(read<Mword>(INT_ENABLE_REG1) | (1<<irq), INT_ENABLE_REG1);
    write<Mword>(read<Mword>(INT_MASK_REG1) & ~(1 << irq), INT_MASK_REG1);
    write<Mword>(read<Mword>(INT_IRQ_PENDING_REG1) | (1<<irq), INT_IRQ_PENDING_REG1);
  } else if(irq < 96){
    irq -= 64;
    write<Mword>(read<Mword>(INT_ENABLE_REG2) | (1<<irq), INT_ENABLE_REG2);
    write<Mword>(read<Mword>(INT_MASK_REG2) & ~(1 << irq), INT_MASK_REG2);
    write<Mword>(read<Mword>(INT_IRQ_PENDING_REG2) | (1<<irq), INT_IRQ_PENDING_REG2);
  }
}

static Static_object<Irq_mgr_single_chip<Irq_chip_arm_sun4i> > mgr;


IMPLEMENT FIASCO_INIT
void Pic::init()
{
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

PUBLIC inline
Unsigned32 Irq_chip_arm_sun4i::pending()
{
  for (int n = 0; n < 3; ++n)
    {
      unsigned long x = read<Mword>(INT_IRQ_PENDING_REG0 + 0x04 * n);
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
