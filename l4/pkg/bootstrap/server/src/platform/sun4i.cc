/*!
 * \file   sun4i.cc
 * \brief  Allwinner A10 (sun4i) support
 *
 * \date   2013-03-24
 * \author Ivan Loskutov <loskutov.ivan@gmail.com>
 *
 */
/*
 * (c) 2013 Ivan Loskutov <loskutov.ivan@gmail.com>
 * (c) 2009 Author(s)
 *     economic rights: Technische Universität Dresden (Germany)
 *
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU General Public License 2.
 * Please see the COPYING-GPL-2 file for details.
 */

#include "support.h"
#include <l4/drivers/uart_sun4i.h>

namespace {
class Platform_arm_sun4i : public Platform_single_region_ram
{
  bool probe() { return true; }

  void init()
  {
    static L4::Uart_sun4i _uart;
    static L4::Io_register_block_mmio r(0x01c28000);
    _uart.startup(&r);
    set_stdio_uart(&_uart);
  }
};
}

REGISTER_PLATFORM(Platform_arm_sun4i);