-- Implements a simple Nios II system for the DE2 board.
-- Inputs: SW7°0 are parallel port inputs to the Nios II system.
-- CLOCK_50 is the system clock.
-- KEY0 is the active-low system reset.
-- Outputs: LEDG7°0 are parallel port outputs from the Nios II system.
-- SDRAM ports correspond to the signals in Figure 2; their names are those
-- used in the DE2 User Manual.
LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_arith.all;
USE ieee.std_logic_unsigned.all;
ENTITY SoW IS
PORT (
SW : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
KEY : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
CLOCK_50 : IN STD_LOGIC;
LEDG : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
DRAM_CLK, DRAM_CKE : OUT STD_LOGIC;
DRAM_ADDR : OUT STD_LOGIC_VECTOR(11 DOWNTO 0);
DRAM_BA_0, DRAM_BA_1 : BUFFER STD_LOGIC;
DRAM_CS_N, DRAM_CAS_N, DRAM_RAS_N, DRAM_WE_N : OUT STD_LOGIC;
DRAM_DQ : INOUT STD_LOGIC_VECTOR(15 DOWNTO 0);
DRAM_UDQM, DRAM_LDQM : BUFFER STD_LOGIC;
LCD_DATA    : inout std_logic_vector(7 downto 0); -- DATA
LCD_ON      : out   std_logic;                                        -- ON
LCD_BLON    : out   std_logic;                                        -- BLON
LCD_EN      : out   std_logic;                                        -- EN
LCD_RS      : out   std_logic;                                        -- RS
LCD_RW      : out   std_logic;
VGA_R:out	std_logic_vector(9	downto	0);	
VGA_G:out	std_logic_vector(9	downto	0);	
VGA_B:out	std_logic_vector(9	downto	0);	
VGA_CLK:	out	std_logic;
VGA_BLANK:	out	std_logic;	
VGA_HS:out	std_logic;	
VGA_VS:out	std_logic;	
VGA_SYNC:out	std_logic;
SRAM_DQ	:	INOUT	STD_LOGIC_VECTOR(15	downto	0);
SRAM_ADDR	:	OUT	STD_LOGIC_VECTOR(17	downto	0);
SRAM_LB_N	:	OUT	STD_LOGIC;
SRAM_UB_N	:	OUT	STD_LOGIC;
SRAM_CE_N	:	OUT	STD_LOGIC;
SRAM_OE_N	:	OUT	STD_LOGIC;
SRAM_WE_N	:	OUT	STD_LOGIC;
PS2_CLK : INOUT STD_LOGIC;
PS2_DAT : INOUT STD_LOGIC;
I2C_SDAT :	INOUT STD_LOGIC;
I2C_SCLK :  OUT	STD_LOGIC;
AUD_XCK	:	OUT	STD_LOGIC;
CLOCK_27	:	IN		STD_LOGIC;
AUD_ADCDAT :	IN	STD_LOGIC;
AUD_ADCLRCK	:	IN	STD_LOGIC;
AUD_BCLK	:	IN		STD_LOGIC;
AUD_DACDAT	:	OUT	STD_LOGIC;
AUD_DACLRCK	:	IN		STD_LOGIC;
UART_RXD :		IN 	STD_LOGIC;
UART_TXD :		OUT	STD_LOGIC;
SD_DAT, SD_DAT3, SD_CMD: INOUT STD_LOGIC;
SD_CLK: OUT STD_LOGIC;
GPIO_0 : INOUT STD_LOGIC_VECTOR(9 downto 0)
);
END SoW;
ARCHITECTURE Structure OF SoW IS
COMPONENT SoW_system
        port (
            clk_clk              : in    std_logic                    ;             -- clk
            reset_reset_n        : in    std_logic                    ;             -- reset_n
            leds_export          : out   std_logic_vector(7 downto 0);                     -- export
            switches_export      : in    std_logic_vector(7 downto 0)  ; -- export
            sdram_wire_addr      : out   std_logic_vector(11 downto 0);                    -- addr
            sdram_wire_ba        : out   std_logic_vector(1 downto 0);                     -- ba
            sdram_wire_cas_n     : out   std_logic;                                        -- cas_n
            sdram_wire_cke       : out   std_logic;                                        -- cke
            sdram_wire_cs_n      : out   std_logic;                                        -- cs_n
            sdram_wire_dq        : inout std_logic_vector(15 downto 0); -- dq
            sdram_wire_dqm       : out   std_logic_vector(1 downto 0);                     -- dqm
            sdram_wire_ras_n     : out   std_logic;                                        -- ras_n
            sdram_wire_we_n      : out   std_logic;                                        -- we_n
            sdram_clk_clk        : out   std_logic;                                        -- clk
            lcd_data_DATA        : inout std_logic_vector(7 downto 0)  ; -- DATA
            lcd_data_ON          : out   std_logic;                                        -- ON
            lcd_data_BLON        : out   std_logic;                                        -- BLON
            lcd_data_EN          : out   std_logic;                                        -- EN
            lcd_data_RS          : out   std_logic;                                        -- RS
            lcd_data_RW          : out   std_logic;                                        -- RW
            sram_DQ              : inout std_logic_vector(15 downto 0) ; -- DQ
            sram_ADDR            : out   std_logic_vector(17 downto 0);                    -- ADDR
            sram_LB_N            : out   std_logic;                                        -- LB_N
            sram_UB_N            : out   std_logic;                                        -- UB_N
            sram_CE_N            : out   std_logic;                                        -- CE_N
            sram_OE_N            : out   std_logic;                                        -- OE_N
            sram_WE_N            : out   std_logic;                                        -- WE_N
            vga_controller_CLK   : out   std_logic;                                        -- CLK
            vga_controller_HS    : out   std_logic;                                        -- HS
            vga_controller_VS    : out   std_logic;                                        -- VS
            vga_controller_BLANK : out   std_logic;                                        -- BLANK
            vga_controller_SYNC  : out   std_logic;                                        -- SYNC
            vga_controller_R     : out   std_logic_vector(9 downto 0);                     -- R
            vga_controller_G     : out   std_logic_vector(9 downto 0);                     -- G
            vga_controller_B     : out   std_logic_vector(9 downto 0);                     -- B
            keyboard_CLK         : inout std_logic                  ;             -- CLK
            keyboard_DAT         : inout std_logic                     ;             -- DAT
            audiovideocfg_SDAT   : inout std_logic                     ;             -- SDAT
            audiovideocfg_SCLK   : out   std_logic;                                        -- SCLK
            audio_ADCDAT         : in    std_logic                    ;             -- ADCDAT
            audio_ADCLRCK        : in    std_logic                   ;             -- ADCLRCK
            audio_BCLK           : in    std_logic                 ;             -- BCLK
            audio_DACDAT         : out   std_logic;                                        -- DACDAT
            audio_DACLRCK        : in    std_logic               ;                   -- DACLRCK
				audio_clk_clk        : out   std_logic;                                        -- clk
            clk2_clk             : in    std_logic;                             -- clk
				serial_RXD           : in    std_logic;           -- RXD
            serial_TXD           : out   std_logic;				-- TXD
				sdcard_b_SD_cmd: INOUT STD_LOGIC;
				sdcard_b_SD_dat: INOUT STD_LOGIC;
				sdcard_b_SD_dat3: INOUT STD_LOGIC;
				sdcard_o_SD_clock: OUT STD_LOGIC;
				gpio_0_export       : inout   std_logic_vector(9 downto 0)  
        );
END COMPONENT;
SIGNAL DQM : STD_LOGIC_VECTOR(1 DOWNTO 0);
SIGNAL BA : STD_LOGIC_VECTOR(1 DOWNTO 0);
BEGIN
DRAM_BA_0 <= BA(0);
DRAM_BA_1 <= BA(1);
DRAM_UDQM <= DQM(1);
DRAM_LDQM <= DQM(0);
-- Instantiate the Nios II system entity generated by the Qsys tool.
NiosII: SoW_system
PORT MAP (
clk_clk => CLOCK_50,
reset_reset_n => KEY(0),
sdram_clk_clk => DRAM_CLK,
leds_export => LEDG,
switches_export => SW,
sdram_wire_addr => DRAM_ADDR,
sdram_wire_ba => BA,
sdram_wire_cas_n => DRAM_CAS_N,
sdram_wire_cke => DRAM_CKE,
sdram_wire_cs_n => DRAM_CS_N,
sdram_wire_dq => DRAM_DQ,
sdram_wire_dqm => DQM,
sdram_wire_ras_n => DRAM_RAS_N,
sdram_wire_we_n => DRAM_WE_N,
lcd_data_DATA    => LCD_DATA,    --   lcd_data.DATA
lcd_data_ON      => LCD_ON,      --           .ON
lcd_data_BLON    => LCD_BLON,    --           .BLON
lcd_data_EN      => LCD_EN,      --           .EN
lcd_data_RS      => LCD_RS,      --           .RS
lcd_data_RW      => LCD_RW,       --           .R 
sram_DQ              => SRAM_DQ,              --           sram.DQ
sram_ADDR            => SRAM_ADDR,            --               .ADDR
sram_LB_N            => SRAM_LB_N,            --               .LB_N
sram_UB_N            => SRAM_UB_N,            --               .UB_N
sram_CE_N            => SRAM_CE_N,            --               .CE_N
sram_OE_N            => SRAM_OE_N,            --               .OE_N
sram_WE_N            => SRAM_WE_N,            --               .WE_N
vga_controller_CLK   => VGA_CLK,   -- vga_controller.CLK
vga_controller_HS    => VGA_HS,    --               .HS
vga_controller_VS    => VGA_VS,    --               .VS
vga_controller_BLANK => VGA_BLANK, --               .BLANK
vga_controller_SYNC  => VGA_SYNC,  --               .SYNC
vga_controller_R     => VGA_R,     --               .R
vga_controller_G     => VGA_G,     --               .G
vga_controller_B     => VGA_B,
keyboard_CLK         => PS2_CLK,         --       keyboard.CLK
keyboard_DAT         => PS2_DAT,          --               .DAT 
audiovideocfg_SDAT   => I2C_SDAT,       -- SDAT
audiovideocfg_SCLK   => I2C_SCLK,                                      -- SCLK
audio_ADCDAT         => AUD_ADCDAT,       -- ADCDAT
audio_ADCLRCK        => AUD_ADCLRCK,                            -- ADCLRCK
audio_BCLK           => AUD_BCLK,                       -- BCLK
audio_DACDAT         => AUD_DACDAT,                                  -- DACDAT
audio_DACLRCK        => AUD_DACLRCK,
audio_clk_clk        => AUD_XCK,                                    -- clk
clk2_clk             => CLOCK_27,  
serial_RXD           => UART_RXD,          -- RXD
serial_TXD           => UART_TXD,
sdcard_b_SD_cmd => SD_CMD,
sdcard_b_SD_dat => SD_DAT,
sdcard_b_SD_dat3 => SD_DAT3,
sdcard_o_SD_clock => SD_CLK ,
gpio_0_export        => GPIO_0 
);
END Structure;
