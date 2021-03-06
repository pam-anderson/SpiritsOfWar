library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity gpio_master is
port (
    clk: in std_logic;
    reset_n: in std_logic;
    addr: in std_logic_vector(1 downto 0);
    rd_en: in std_logic;
    wr_en: in std_logic;
    readdata: out std_logic_vector(31 downto 0);
    writedata: in std_logic_vector(31 downto 0);
	 serialdata: inout std_logic_vector(9 downto 0)
);
end gpio_master;

architecture rtl of gpio_master is
    signal saved_value : std_logic_vector(7 downto 0);
	 signal done, ready : std_logic_vector(0 downto 0);
begin
	ready <= serialdata(0 downto 0);
	saved_value <= serialdata(9 downto 2);
	serialdata(1 downto 1) <= done;
		   --saved_value
    process (clk)
    begin
        if rising_edge(clk) then
            if (reset_n = '0') then
					 done <= "0";
            elsif (wr_en = '1' and addr = "00") then
					-- Writing to done flag
                done <= writedata(0 downto 0);
            end if;
        end if;
    end process;
    
    --readdata
    process (rd_en, addr, saved_value)
    begin
        readdata <= (others => '-');
        if (rd_en = '1') then
            if (addr = "00") then
                -- Read ready flag
					 readdata <= "0000000000000000000000000000000" & ready;
            elsif (addr = "01") then
					 -- Read 
						readdata <= "000000000000000000000000" & saved_value;
            end if;
        end if;
    end process;
end rtl;
