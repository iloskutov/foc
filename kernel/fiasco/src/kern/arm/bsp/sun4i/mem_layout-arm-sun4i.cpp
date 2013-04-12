INTERFACE [arm && sun4i]: //----------------------------------------------

EXTENSION class Mem_layout
{
public:
  enum Virt_layout_sun4i : Address {
    Timer_map_base       = Devices1_map_base + 0x020C00,
    Intc_map_base        = Devices1_map_base + 0x020400,
    Uart_base            = Devices1_map_base + 0x028000,

    Watchdog_map_base    = Timer_map_base    + 0x90,
  };

  enum Phys_layout_sun4i : Address {
    Devices1_phys_base   = 0x01c00000,

    Sdram_phys_base      = 0x40000000,
    Flush_area_phys_base = 0xe0000000,
  };
};
