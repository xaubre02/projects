LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
 
ENTITY bcd_tb IS
END bcd_tb;
 
ARCHITECTURE behavior OF bcd_tb IS 
 
    COMPONENT bcd
    PORT(
         CLK : IN  std_logic;
         RESET : IN  std_logic;
         NUMBER1 : OUT  std_logic_vector(3 downto 0);
         NUMBER2 : OUT  std_logic_vector(3 downto 0);
         NUMBER3 : OUT  std_logic_vector(3 downto 0)
        );
    END COMPONENT;

   --Inputs
   signal CLK : std_logic := '0';
   signal RESET : std_logic := '0';

 	--Outputs
   signal NUMBER1 : std_logic_vector(3 downto 0);
   signal NUMBER2 : std_logic_vector(3 downto 0);
   signal NUMBER3 : std_logic_vector(3 downto 0);
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: bcd PORT MAP (
          CLK => CLK,
          RESET => RESET,
          NUMBER1 => NUMBER1,
          NUMBER2 => NUMBER2,
          NUMBER3 => NUMBER3
        );
 
    -- Stimulus process
	test: process
	begin		
		for c in 1 to 110 loop
			CLK <= '1';
			wait for 1 ns;
			
			CLK <= '0';
			wait for 1 ns;
			
			if (c=5) then	-- Cislo: 5
				assert NUMBER1="0101" and NUMBER2="0000" and NUMBER3="0000" report "Chyba, ocekavano 0000 0000 0101 (5)" severity error;
			end if;
			
			if (c=10) then	-- Cislo: 10
				assert NUMBER1="0000" and NUMBER2="0001" and NUMBER3="0000" report "Chyba, ocekavano 0000 0001 0000 (10)" severity error;
			end if;
			
			if (c=25) then	-- Cislo: 25
				assert NUMBER1="0101" and NUMBER2="0010" and NUMBER3="0000" report "Chyba, ocekavano 0000 0010 0101 (25)" severity error;
			end if;
			
			if (c=50) then	-- Cislo: 50
				assert NUMBER1="0000" and NUMBER2="0101" and NUMBER3="0000" report "Chyba, ocekavano 0000 0101 0000 (50)" severity error;
			end if;
			
			if (c=75) then -- Cislo: 75
				assert NUMBER1="0101" and NUMBER2="0111" and NUMBER3="0000" report "Chyba, ocekavano 0000 0111 0101 (75)" severity error;
			end if;
			
			if (c=80) then	-- Cislo: 80	
				assert NUMBER1="0000" and NUMBER2="1000" and NUMBER3="0000" report "Chyba, ocekavano 0000 1000 0000 (80)" severity error;
			end if;
			
			if (c=99) then	-- Cislo: 99	
				assert NUMBER1="1001" and NUMBER2="1001" and NUMBER3="0000" report "Chyba, ocekavano 0000 1001 1001 (99)" severity error;
			end if;
			
			if (c=100) then	-- Cislo: 100		
				assert NUMBER1="0000" and NUMBER2="0000" and NUMBER3="0001" report "Chyba, ocekavano 0001 0000 0000 (100)" severity error;
			end if;
			
			if (c=101) then	-- Cislo: 0
				assert NUMBER1="0000" and NUMBER2="0000" and NUMBER3="0000" report "Chyba, ocekavano 0000 0000 0000 (0)" severity error;
			end if;
			
			if (c=102) then -- Cislo: 1		
				assert NUMBER1="0001" and NUMBER2="0000" and NUMBER3="0000" report "Chyba, ocekavano 0000 0000 0001 (1)" severity error;
			end if;
			
			if (c=106) then -- Cislo: 5		
				assert NUMBER1="0101" and NUMBER2="0000" and NUMBER3="0000" report "Chyba, ocekavano 0000 0000 0101 (5)" severity error;
			end if;
			
			if (c=111) then -- Cislo: 10
				assert NUMBER1="0000" and NUMBER2="0001" and NUMBER3="0000" report "Chyba, ocekavano 0000 0001 0000 (10)" severity error;
			end if;
			
		end loop;
		
		RESET <= '1';
		wait for 1 ns;
		
		assert NUMBER1="0000" and NUMBER2="0000" and NUMBER3="0000" report "Chyba, ocekavano 0000 0000 0000 (0)" severity error;
		
		wait for 10 ns;
		
		assert NUMBER1="0000" and NUMBER2="0000" and NUMBER3="0000" report "Chyba, ocekavano 0000 0000 0000 (0)" severity error;
		
		wait;
   end process test;

END ARCHITECTURE behavior;
