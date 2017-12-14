
/* 
 * The keys defined in XENV, more can be added.
 */

#ifndef __XENV_KEYS_H__
#define __XENV_KEYS_H__

#define XENV_KEY_BOARD_ID           "a.board_id"
#define XENV_KEY_CHIP_REV           "a.chip_rev"

#define XENV_KEY_PREMUX             "a.premux"
#define XENV_KEY_AVCLK_MUX          "a.avclk_mux"
#define XENV_KEY_HOSTCLK_MUX        "a.hostclk_mux"
#define XENV_KEY_IRQ_RISE_EDGE_LO   "a.irq_rise_edge_lo"
#define XENV_KEY_IRQ_FALL_EDGE_LO   "a.irq_fall_edge_lo"
#define XENV_KEY_GPIO_IRQ_MAP       "a.gpio_irq_map"

#define XENV_KEY_DEF_BAUDRATE       "a.baudrate"
#define XENV_KEY_CONSOLE_UART_PORT  "a.uart_console_port"
#define XENV_KEY_UART_USED_PORTS    "a.uart_used_ports"

#define XENV_KEY_PB_CS_CONFIG       "a.pb_cs_config"
#define XENV_KEY_DEF_TIMING         "a.pb_def_timing"
#define XENV_KEY_PB_TIMING0         "a.pb_timing0"
#define XENV_KEY_PB_USE_TIMING0     "a.pb_use_timing0"
#define XENV_KEY_PB_TIMING1         "a.pb_timing1"
#define XENV_KEY_PB_USE_TIMING1     "a.pb_use_timing1"
#define XENV_KEY_PB_TIMING2         "a.pb_timing2"
#define XENV_KEY_PB_USE_TIMING2     "a.pb_use_timing2"
#define XENV_KEY_PB_TIMING3         "a.pb_timing3"
#define XENV_KEY_PB_USE_TIMING3     "a.pb_use_timing3"
#define XENV_KEY_PB_TIMING4         "a.pb_timing4"
#define XENV_KEY_PB_USE_TIMING4     "a.pb_use_timing4"
#define XENV_KEY_PB_TIMING5         "a.pb_timing5"
#define XENV_KEY_PB_USE_TIMING5     "a.pb_use_timing5"

#if (defined(CONFIG_TANGO2) && (EM86XX_REVISION > 3)) || defined(CONFIG_TANGO3)
#define XENV_KEY_IRQ_RISE_EDGE_HI   "a.irq_rise_edge_hi"
#define XENV_KEY_IRQ_FALL_EDGE_HI   "a.irq_fall_edge_hi"
#endif

#define XENV_KEY_ENABLED_DEVICES    "a.enable_devices"

#define XENV_KEY_ETH_MAC            "a.eth_mac"

#define XENV_KEY_SCARD_OFF          "a.scard_off_pin"
#define XENV_KEY_SCARD_5V           "a.scard_5v_pin"
#define XENV_KEY_SCARD_CMD          "a.scard_cmd_pin"

#define XENV_KEY_ISAIDE_IRQ_ROUTE   "a.isaide_irq_route"
#define XENV_KEY_ISAIDE_TIMING_SLOT "a.isaide_timing_slot"

#define XENV_KEY_GPIO_DIR           "a.gpio_dir"
#define XENV_KEY_GPIO_DATA          "a.gpio_data"

#define XENV_KEY_LINUX_CMD          "a.linux_cmd"

#define XENV_KEY_Z_BOOT_DEF         "z.default_boot"

#define XENV_KEY_Z_PROD_TEST        "z.dt"
#define XENV_KEY_Z_PROD_LOOPS       "z.prod_loops"
#define XENV_KEY_Z_PROD_FTEST       "z.prod_ftest"
#define XENV_KEY_Z_PROD_FSTART      "z.prod_fstart"
#define XENV_KEY_Z_PROD_FEND        "z.prod_fend"
#define XENV_KEY_Z_PROD_FSTEP       "z.prod_fstep"
#define XENV_KEY_Z_PROD_RWIN        "z.prod_rwindow"
#define XENV_KEY_Z_PROD_WWIN        "z.prod_wwindow"
#define XENV_KEY_Z_PROD_CL          "z.prod_cl"
#define XENV_KEY_Z_PROD_NBLOCKS     "z.prod_nblocks"

#define XENV_KEY_YAMON_ENV          "y.env"
#define XENV_KEY_YAMON_IPADDR       "y.ipaddr"
#define XENV_KEY_YAMON_SUBNET       "y.subnetmask"
#define XENV_KEY_YAMON_GATEWAY      "y.gateway"
#define XENV_KEY_YAMON_START        "y.start"
#define XENV_KEY_YAMON_STARTDELAY   "y.startdelay"

#define XENV_KEYS_PCI_IRQ_ROUTE     "a.pcidev%d_irq_route"
#define XENV_KEYS_CD_FREQUENCY      "a.cd%d_freq"
#define XENV_KEYS_CD_DIV            "a.cd%d_div"
#define XENV_KEYS_UART_GPIO_MODE    "a.uart%d_gpio_mode"
#define XENV_KEYS_UART_GPIO_DIR     "a.uart%d_gpio_dir"
#define XENV_KEYS_UART_GPIO_DATA    "a.uart%d_gpio_data"
#define XENV_KEYS_UART_BAUDRATE     "a.uart%d_baudrate"
#define XENV_KEYS_GPIO_PULSE        "a.gpio%d_pulse"

#define XENV_KEYS_Z_BOOT_LOCATION   "z.boot%d"

#define XENV_KEYS_CS_SIZE           "l.cs%d_size"
#define XENV_KEYS_CS_PARTS          "l.cs%d_parts"

#define XENV_KEYS_CS_PART_SIZE      "l.cs%d_part%d_size"
#define XENV_KEYS_CS_PART_OFFSET    "l.cs%d_part%d_offset" 

#define XENV_KEY_PS_PLL3            "a.ps.pll3"
#define XENV_KEY_PS_PLL2            "a.ps.pll2"
#define XENV_KEY_PS_PLL1            "a.ps.pll1"
#define XENV_KEY_PS_PLL0            "a.ps.pll0"
#define XENV_KEY_PS_MUX             "a.ps.mux"
#define XENV_KEY_PS_D0CFG           "a.ps.d0cfg"
#define XENV_KEY_PS_D1CFG           "a.ps.d1cfg"
#define XENV_KEY_PS_D0DELAY         "a.ps.d0delay"
#define XENV_KEY_PS_D1DELAY         "a.ps.d1delay"

#ifndef CONFIG_SIGBLOCK_SUPPORT
#define ISAIDE_SHIFT		0
#define BMIDE_SHIFT		1
#define PCIHOST_SHIFT		2
#define ETHERNET_SHIFT		3
#define IR_SHIFT		4
#define FIP_SHIFT		5	
#define I2CM_SHIFT		6
#define I2CS_SHIFT		7
#define SDIO_SHIFT		8
#define USB_SHIFT		9
#define PCI1_SHIFT		10
#define PCI2_SHIFT		11
#define PCI3_SHIFT		12
#define PCI4_SHIFT		13
#define PCI5_SHIFT		14
#define PCI6_SHIFT		15
#define SATA_SHIFT		16
#define SCARD_SHIFT		17
#define GNET_SHIFT		18
#endif

#endif

