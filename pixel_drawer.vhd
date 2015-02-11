library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity pixel_drawer is
generic(
    pixel_buffer_base : std_logic_vector := x"00080000"
	 );
port (
    clk: in std_logic;
    reset_n: in std_logic;
    slave_addr: in std_logic_vector(7 downto 0);
    slave_rd_en: in std_logic;
    slave_wr_en: in std_logic;
    slave_readdata: out std_logic_vector(31 downto 0);
    slave_writedata: in std_logic_vector(31 downto 0);
    master_addr : out std_logic_vector(31 downto 0);
    master_rd_en : out std_logic;
    master_wr_en : out std_logic;
    master_be : out std_logic_vector(1 downto 0);
    master_readdata : in std_logic_vector(15 downto 0);
    master_writedata: out  std_logic_vector(15 downto 0);
    master_waitrequest : in std_logic);
end pixel_drawer;

architecture rtl of pixel_drawer is
    signal x1,x2 : std_logic_vector(8 downto 0);
    signal y1,y2 : std_logic_vector(7 downto 0);
    signal colour : std_logic_vector(15 downto 0);
	 signal tile_type : std_logic_vector(9 downto 0);
    signal done : std_logic := '0';
	 type tile is array(255 downto 0) of std_logic_vector(15 downto 0);
	 type mem is array(39 downto 0) of tile;
	 signal memory: mem;
	 signal ram_wr: std_logic;
	 signal ram_address: std_logic_vector(15 downto 0);
	 signal ram_data: std_logic_vector(15 downto 0);
--	 signal ram_rd: std_logic;
--	 signal ram_out: std_logic_vector(15 downto 0);
--	 constant pixel_buffer_base : std_logic_vector(31 downto 0) :=  x"00080000";
	 
begin

    -- This synchronous process is triggered on a rising clock edge.
    -- There are two things we might do on a rising clock edge.  We might
    -- respond to write operations on the slave bus, or we might step through
    -- the state machine to draw something to the pixel buffer.  We could
    -- have separated these into two processes.

    process(clk, reset_n)
    variable processing : bit := '0';  -- Used to indicate whether we are drawing
    variable state : integer;          -- Current state.  We could use enumerated types.

    -- The following are local copies of the coordinates and colour.  When the user
    -- starts a drawing operation, we immediately copy the coordinates here, so that
    -- if the user tries to change the coordinates while the draw operation is running,
    -- the draw operation completes with the old value of the coordinates.  This is
    -- not strictly required, but perhaps provides a more Ã¢â‚¬Å“naturalÃ¢â‚¬Â operation for
    -- whoever is writing the C code.

    variable x1_local,x2_local : std_logic_vector(8 downto 0);
    variable y1_local,y2_local : std_logic_vector(7 downto 0);
    variable colour_local : std_logic_vector(15 downto 0);
	 variable count : integer := 0;

    -- This is used to remember the left-most x point as we draw the box.
    variable savedx : std_logic_vector(8 downto 0);
	 variable savedy : std_logic_vector(7 downto 0);
	 	 
	 
    begin
       if (reset_n = '0') then
          master_wr_en<= '0';
          master_rd_en<= '0';
          processing := '0';
          state := 0;
          done <= '0';
			 ram_wr <= '0';
			 count := 0;

        elsif rising_edge(clk) then

           -- on a rising clock edge, if we are currently in the middle of a
           -- drawing operation, step through the drawing state machine.

           if processing = '1' then

               -- Initiate a write operation on the master bus.  The address of
               -- of the write operation points to the pixel buffer plus an offset
               -- that is computed from the x1_local and y1_local.  The final Ã¢â‚¬Ëœ0Ã¢â‚¬â„¢
               -- is because each pixel takes 16 bits in memory.  The data of the
               -- write operation is the colour value (16 bits).

               if state = 0 then						  
					 --elsif state = 3 then 
					  master_addr <= std_logic_vector(unsigned(pixel_buffer_base) + unsigned( y1_local & x1_local & '0'));
					  ram_address <= std_logic_vector(unsigned(tile_type(7 downto 0))) & std_logic_vector(to_unsigned(count, 8));
					  state := 1;
						
					elsif state = 1 then
						colour_local := colour;
						if(colour_local = x"F81F") then
							colour_local := x"0000";
							master_wr_en  <= '0';
							state := 0;
							if (x1_local = x2_local) then
								if (y1_local = y2_local) then 
									done <= '1';   -- box is done
									processing := '0';
									count := 0;
								else
									x1_local := savedx;
									y1_local := std_logic_vector(unsigned(y1_local)+1);
									count := count + 1;
								end if;
							else 
                        x1_local := std_logic_vector(unsigned(x1_local)+1);
								count := count + 1;
							end if;
						else
							case tile_type(9 downto 8) is
								when "01" => colour_local := colour_local OR x"001F";
								when "10" => colour_local:= colour_local OR x"F800";
								when others => null;
							end case;
							master_writedata <= colour_local;
							master_be <= "11";  -- byte enable
							master_wr_en <= '1';
							master_rd_en <= '0';
							state := 2; -- on the next rising clock edge, do state 2 operations
						end if;

               -- After starting a write operation, we need to wait until
               -- master_waitrequest is 0.  If it is 1, stay in state 1.

               elsif state = 2 and master_waitrequest = '0' then
                  master_wr_en  <= '0';
                  state := 0;
                  if (x1_local = x2_local) then
                     if (y1_local = y2_local) then 
                        done <= '1';   -- box is done
                        processing := '0';
								count := 0;
                     else
                        x1_local := savedx;
                        y1_local := std_logic_vector(unsigned(y1_local)+1);
								count := count + 1;
                     end if;
                  else 
                        x1_local := std_logic_vector(unsigned(x1_local)+1);
								count := count + 1;	
                  end if;
               end if;
				end if;
             -- We should also check if there is a write on the slave bus.  If so, copy the
             -- written value into one of our internal registers.
				if(state = 3) then
					ram_wr <= '0';
					state := 0;
					count := count + 1;
					done <= '1';
				end if;
					
             if (slave_wr_en = '1') then
					if(count >= 256) then
						count := 0;
					end if;
                case slave_addr is

                    -- These four should be self-explantory
                    when x"00" => x1 <= slave_writedata(8 downto 0);
											x2 <= std_logic_vector(unsigned(slave_writedata(8 downto 0)) + 15);
											count := 0;
											ram_wr <= '0';
                    when x"01" => y1 <= slave_writedata(7 downto 0);
											y2 <= std_logic_vector(unsigned(slave_writedata(7 downto 0)) + 15);
											count := 0;
											ram_wr <= '0';
                    when x"02" => tile_type <= slave_writedata(9 downto 0);
											 count := 0;
											 ram_wr <= '0';
						  when x"04" => ram_address <= tile_type(7 downto 0) & std_logic_vector(to_unsigned(count, 8));
											 ram_data <= slave_writedata(15 downto 0);
											 ram_wr <= '1';
											 done <= '0';
											 --count := count + 1;
											 state := 3;

                    -- If the user tries to write to offset 3, we are to start drawing
                    when x"03" =>
								ram_wr <= '0';
                       if processing = '0' then
                          processing := '1';  -- start drawing on next rising clk edge
                          state := 0;
                          done <= '0';
								  count := 0;

                          -- The above drawing code assumes x1<x2 and y1<y2, however the
                          -- user may give us points with x1>x2 or y1>y2.  If so, swap
                          -- the x and y values.  In any case, copy to our internal _local
                          -- variables.  This ensures that if the user changes a coordinate
                          -- while a drawing is occurring, it continues to draw the box
                          -- as originally requested.

                          if (x1 < x2) then
                             x1_local := x1;
                             savedx := x1;
                             x2_local := x2;
                          else
                             x2_local := x1;
                             savedx := x2;
                             x1_local := x2;
                          end if;
								
                          if (y1 < y2) then
                             y1_local := y1;
									  savedy := y1;
                             y2_local := y2;
                          else
                             y2_local := y1;
									  savedy := y2;
                             y1_local := y2;
                          end if;									
                        end if;
								colour_local := x"FFFF";
                   when others => null;
                end case;
            end if;
         end if;
   end process;


	process(clk)
	begin
		if rising_edge(clk) then
			if(ram_wr = '1') then
				memory(to_integer(unsigned(ram_address(15 downto 8))))(to_integer(unsigned(ram_address(7 downto 0))) - 1) <= ram_data;
			end if;
				colour <= memory(to_integer(unsigned(ram_address(15 downto 8))))(to_integer(unsigned(ram_address(7 downto 0))));
		end if;
	end process;
	
   -- This process is used to describe what to do when a Ã¢â‚¬Å“readÃ¢â‚¬Â operation occurs on the
   -- slave interface (this is because the C program does a memory read).  Depending
   -- on the address read, we return x1, x2, y1, y2, the colour, or the done flag.

   process (slave_rd_en, slave_addr, x1,x2,y1,y2,colour,tile_type,done)
   begin	       
      slave_readdata <= (others => '-');
      if (slave_rd_en = '1') then
          case slave_addr is
              when "00000000" => slave_readdata <= "00000000000000000000000" & x1;
              when "00000001" => slave_readdata <= "000000000000000000000000" & y1;
              when "00000010" => slave_readdata <= "00000000000000000000000" & x2;
              when "00000011" => slave_readdata <= "000000000000000000000000" & y2;
              when "00000100" => slave_readdata <= "0000000000000000" & colour;
				  when "00000101" => slave_readdata <= "0000000000000000000000" & tile_type;
              when "00000110" => slave_readdata <= (0=>done, others=>'0');
              when others => null;
            end case;
         end if;
    end process;						
				
end rtl;
