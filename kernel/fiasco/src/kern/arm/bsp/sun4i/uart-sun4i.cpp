IMPLEMENTATION [sun4i]:

#include "uart_sun4i.h"

IMPLEMENT int Uart::irq() const { return 1; }

IMPLEMENT L4::Uart *Uart::uart()
{
  static L4::Uart_sun4i uart;
  return &uart;
}

#include "mem_layout.h"

IMPLEMENT Address Uart::base() const { return Mem_layout::Uart_base; }
