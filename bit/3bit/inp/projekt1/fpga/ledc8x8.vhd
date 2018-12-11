library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity ledc8x8 is
	port ( -- Sem doplnte popis rozhrani obvodu.
			RESET : in std_logic;
			SMCLK : in std_logic;
			ROW : out std_logic_vector(7 downto 0);
			LED : out std_logic_vector(7 downto 0)
	);
end ledc8x8;

architecture main of ledc8x8 is
	
	signal citac : std_logic_vector(22 downto 0);
	signal radek : std_logic_vector(7 downto 0) := "10000000";
	signal ledka : std_logic_vector(7 downto 0) := "11111111";
	signal clock, pismeno : std_logic := '0';

begin
	-- vypocitani potrebnych frekvenci
	frekvence : process (RESET, SMCLK)
	begin
		if RESET = '1' then
			citac <= (others => '0');
		elsif SMCLK'event and SMCLK = '1' then
			if citac(7 downto 0) = "11111111" then 
				clock <= '1';
			else
				clock <= '0';
			end if;

			citac <= citac + 1;
			pismeno <= citac(22);
		end if;
	end process frekvence;
	
	posouvani : process (SMCLK, RESET, radek)
	begin
		if RESET = '1' then
			radek <= "10000000";
			
		elsif SMCLK'event and SMCLK = '1' and clock = '1' then
			case radek is
				when "10000000" => radek <= "01000000";
				when "00000001" => radek <= "10000000";
				when "00000010" => radek <= "00000001";
				when "00000100" => radek <= "00000010";
				when "00001000" => radek <= "00000100";
				when "00010000" => radek <= "00001000";
				when "00100000" => radek <= "00010000";
				when "01000000" => radek <= "00100000";
				when others => null;
			end case;
		end if;
		ROW <= radek;
	end process posouvani;
	
	dekodovani : process (RESET, SMCLK, pismeno, radek)
	begin
		if RESET = '1' then
			ledka <= "11111111";
		elsif SMCLK'event and SMCLK = '1' then
			if pismeno = '0' then -- Tcko
				case radek is
					when "00000001" => ledka <= "00000000";
					when "00000010" => ledka <= "00000000";
					when "00000100" => ledka <= "11100111";
					when "00001000" => ledka <= "11100111";
					when "00010000" => ledka <= "11100111";
					when "00100000" => ledka <= "11100111";
					when "01000000" => ledka <= "11100111";
					when "10000000" => ledka <= "11100111";
					when others => ledka <= "11111111";
				end case;
			else	
				case radek is -- Acko
					when "00000001" => ledka <= "11000011";
					when "00000010" => ledka <= "10000001";
					when "00000100" => ledka <= "10011001";
					when "00001000" => ledka <= "10011001";
					when "00010000" => ledka <= "10000001";
					when "00100000" => ledka <= "10000001";
					when "01000000" => ledka <= "10011001";
					when "10000000" => ledka <= "10011001";
					when others => ledka <= "11111111";
				end case;
			end if;
		end if;
	end process dekodovani;
	ROW <= radek;
	LED <= ledka;
end main;