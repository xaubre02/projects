library IEEE;
use IEEE.std_logic_1164.ALL;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;


entity bcd is
	port(
		CLK : in std_logic;
		RESET : in std_logic;
		NUMBER1 : out std_logic_vector(3 downto 0);
		NUMBER2 : out std_logic_vector(3 downto 0);
		NUMBER3 : out std_logic_vector(3 downto 0)
		);
end bcd;

architecture behavioral of bcd is
	signal snumber1,snumber2,snumber3 : std_logic_vector(3 downto 0):= "0000";
begin
	process (CLK,RESET)
	begin
		if (RESET='1') then
			snumber1 <= (others => '0');
			snumber2 <= (others => '0');
			snumber3 <= (others => '0');
		elsif CLK'event and CLK='1' then
			if 	(snumber1 = 9) then
                snumber1 <= "0000";
				
				if (snumber2 = 9) then
					snumber2 <= "0000";
					
					if (snumber3 = 9) then
						snumber3 <= "0000";
					else
						snumber3 <= snumber3 + 1;
					end if;
				else
					snumber2 <= snumber2 + 1;
				end if;
				
			elsif (snumber3=1) then
				snumber1 <= "0000"; 
				snumber2 <= "0000"; 
				snumber3 <= "0000";
			
			else
                snumber1 <= snumber1 + 1;
			end if;
		end if;
	end process;
    
	NUMBER1 <= snumber1;
	NUMBER2 <= snumber2;
	NUMBER3 <= snumber3;
 end;