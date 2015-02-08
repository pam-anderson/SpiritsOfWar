library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity gpio_com is
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
end gpio_com;

architecture rtl of gpio_com is
    signal saved_value : std_logic_vector(7 downto 0);
	 signal done, ready : std_logic_vector(0 downto 0);
begin
    serialdata(0 downto 0) <= ready;
	 serialdata(9 downto 2) <= saved_value;
	 done <= serialdata(1 downto 1);
	 
    --saved_value
    process (clk)
    begin
        if rising_edge(clk) then
            if (reset_n = '0') then
                saved_value <= (others => '0');
					-- done <= "0";
					 ready <= "0";
            --elsif (wr_en = '1' and addr = "00") then
					-- Writing to done flag
               -- done <= writedata(0 downto 0);
            elsif (wr_en = '1' and addr = "01") then
					-- Writing to ready flag
                ready <= writedata(0 downto 0);
				elsif (wr_en = '1' and addr = "10") then
					-- Writing data to transfer
					--if done = "1" then -- Don't overwrite data if not done reading
						saved_value <= writedata(7 downto 0);
				--	end if;
            end if;
        end if;
    end process;
    
    --readdata
    process (rd_en, addr, saved_value)
    begin
        readdata <= (others => '-');
        if (rd_en = '1') then
           -- if (addr = "00") then
                -- Read ready flag
				--	 readdata <= "0000000000000000000000000000000" & ready;
            if (addr = "01") then
					 -- Read done flag
                readdata <= "0000000000000000000000000000000" & done;
          --  elsif (addr = "10") then
					 -- Read 
			--		if ready = "1" and done = "0" then
			--			readdata <= "000000000000000000000000" & saved_value;
			--		end if;
            end if;
        end if;
    end process;
end rtl;
