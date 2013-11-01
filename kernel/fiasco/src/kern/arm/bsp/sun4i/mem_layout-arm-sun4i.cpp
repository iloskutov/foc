INTERFACE [arm && sun4i]: //----------------------------------------------

EXTENSION class Mem_layout
{
public:
  enum Phys_layout_sun4i : Address {
    Devices1_phys_base   = 0x01c00000,
    Timer_phys_base      = Devices1_phys_base + 0x020C00,
    Intc_phys_base       = Devices1_phys_base + 0x020400,
    Uart_phys_base       = Devices1_phys_base + 0x028000,
    Watchdog_phys_base   = Timer_phys_base    + 0x90,
  };
};

