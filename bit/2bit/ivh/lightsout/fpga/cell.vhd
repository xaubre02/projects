----------------------------
--  Project:  Lights Out  --
----------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use work.math_mask.all;


entity cell is
	generic (
	MASK				: mask_t := (others => '1')
	);
	port (
		-- signaly pro praci s pozadavky na inverzi
		INVERT_REQ_IN  	: in   STD_LOGIC_VECTOR (3 downto 0);
		INVERT_REQ_OUT 	: out  STD_LOGIC_VECTOR (3 downto 0);
		-- klavesy
		KEYS            : in   STD_LOGIC_VECTOR (4 downto 0);
		-- signaly pro praci s pozadavky na vyber
		SELECT_REQ_IN   : in   STD_LOGIC_VECTOR (3 downto 0);
		SELECT_REQ_OUT  : out  STD_LOGIC_VECTOR (3 downto 0);
		-- signaly pro praci s aktivitou
		INIT_ACTIVE     : in   STD_LOGIC;
		ACTIVE          : out  STD_LOGIC;
		-- signaly pro praci s vyberem
		INIT_SELECTED   : in   STD_LOGIC;
		SELECTED        : out  STD_LOGIC;
		-- hodinovy signal a reset
		CLK             : in   STD_LOGIC;
		RESET           : in   STD_LOGIC
	);
end cell;

architecture Behavioral of cell is
	-- indexy sousednich bunek
	constant IDX_TOP    : NATURAL := 0; -- nahore
    constant IDX_LEFT   : NATURAL := 1; -- vlevo
    constant IDX_RIGHT  : NATURAL := 2; -- vpravo
    constant IDX_BOTTOM : NATURAL := 3; -- dole 
	constant IDX_ENTER  : NATURAL := 4; -- enter
	-- pomocne signaly pro praci v prosecu
	signal aktivita, vyber : std_logic;
	
begin
	process (CLK,RESET)
	begin
		if (RESET='1') then
			-- inicializace pri resetu
			aktivita <= INIT_ACTIVE;
			vyber <= INIT_SELECTED;
		elsif (CLK'event) and (CLK='1') then
			-- pocatecni inicializace
			SELECT_REQ_OUT<="0000";
			INVERT_REQ_OUT<="0000";
			-- pokud je vybrana klavesa a:
			if (vyber = '1') then
				if (KEYS(IDX_TOP) = '1') and (MASK.top = '1') then  -- pozadavek na posun nahoru, pokud tam je bunka
						SELECT_REQ_OUT(IDX_TOP) <= '1';
						vyber <= '0';
				elsif (KEYS(IDX_RIGHT) = '1') and (MASK.right = '1') then -- pozadavek na posun doprava, pokud tam je bunka
						SELECT_REQ_OUT(IDX_RIGHT) <= '1';
						vyber <= '0';
				elsif (KEYS(IDX_BOTTOM) = '1') and (MASK.bottom = '1') then -- pozadavek na posun dolu, pokud tam je bunka
						SELECT_REQ_OUT(IDX_BOTTOM) <= '1';
						vyber <= '0';
				elsif (KEYS(IDX_LEFT) = '1') and (MASK.left = '1') then  -- pozadavek na posun doleva, pokud tam je bunka
						SELECT_REQ_OUT(IDX_LEFT) <= '1';
						vyber <= '0';
				end if;   
				------------------------------------------------
				if (KEYS(IDX_ENTER) = '1') then
					-- pozadavek na inverzi horniho souseda
					if (MASK.top = '1') then
						INVERT_REQ_OUT(IDX_TOP) <= '1';
					end if;
					-- pozadavek na inverzi praveho souseda
					if (MASK.right = '1') then
						INVERT_REQ_OUT(IDX_RIGHT) <= '1';
					end if;
					-- pozadavek na inverzi spodniho souseda
					if (MASK.bottom = '1') then
						INVERT_REQ_OUT(IDX_BOTTOM) <= '1';
					end if;
					-- pozadavek na inverzi leveho souseda
					if (MASK.left = '1') then
						INVERT_REQ_OUT(IDX_LEFT) <= '1';
					end if;
					-- inverze vlastniho stavu
					aktivita <= not aktivita;
				end if;
			-----------------------------------------------------------------------------------
			else
				-- inverze horniho souseda, pokud existuje
				if (INVERT_REQ_IN(IDX_TOP) = '1') and (MASK.top = '1') then 
						aktivita <= not aktivita;
				-- inverze praveho souseda, pokud existuje
				elsif (INVERT_REQ_IN(IDX_RIGHT) = '1') and (MASK.right = '1') then
						aktivita <= not aktivita;
				-- inverze spodniho souseda, pokud existuje
				elsif (INVERT_REQ_IN(IDX_BOTTOM) = '1') and (MASK.bottom = '1') then
						aktivita <= not aktivita;
				-- inverze leveho souseda, pokud existuje
				elsif (INVERT_REQ_IN(IDX_LEFT) = '1') and (MASK.left = '1') then
						aktivita <= not aktivita;
				end if;			  
				------------------------------------------------------------------------------
				-- vyber horniho souseda, pokud existuje
				if (SELECT_REQ_IN(IDX_TOP) = '1') and (MASK.top = '1') then
						vyber <= '1';
				-- vyber praveho souseda, pokud existuje
				elsif (SELECT_REQ_IN(IDX_RIGHT) = '1') and (MASK.right = '1') then
						vyber <= '1';
				-- vyber spodnihko souseda, pokud existuje
				elsif (SELECT_REQ_IN(IDX_BOTTOM) = '1') and (MASK.bottom = '1') then
						vyber <= '1';
				-- vyber leveho souseda, pokud existuje
				elsif (SELECT_REQ_IN(IDX_LEFT) = '1') and (MASK.left = '1') then
						vyber <= '1';
				end if;
			end if;
		end if;
		-- prirazeni hodnot
		ACTIVE <= aktivita;
		SELECTED <= vyber;
	end process;
end Behavioral;