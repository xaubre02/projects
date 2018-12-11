----------------------------
--  Project:  Lights Out  --
----------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.vga_controller_cfg.all;
use work.math_mask.all;
use work.math_fce.all;

architecture main of tlv_pc_ifc is
	-- signal pro praci s vga
	signal vga_mode  : std_logic_vector(60 downto 0);
	-- pomocne signaly pro barvy
	signal irgb, color1, color2, color3 : std_logic_vector(8 downto 0);
	-- signaly pro praci s radky a sloupci
	signal row : std_logic_vector(11 downto 0);
	signal col : std_logic_vector(11 downto 0);
	-- signaly pro praci s klavesnici
	signal kbrd_data_out 	: std_logic_vector(15 downto 0);
	signal kboard 			: std_logic_vector(4 downto 0);
	signal kbrd_data_vld 	: std_logic;
	-- signaly pro praci kurzoru s radky a sloupci
	signal cur_x : std_logic_vector(4 downto 0) := "00001";
	signal cur_y : std_logic_vector(4 downto 0) := "00001";
	-- signaly pro praci s bunkou
	signal active 			: std_logic_vector(24 downto 0);
	signal selected 		: std_logic_vector(24 downto 0);
	signal init_active 		: std_logic_vector(24 downto 0) := "0000000000000000000000000"; -- pocatecni inicializace
	signal init_selected 	: std_logic_vector(24 downto 0) := "0000000000000000000000001"; -- pocatecni inicializace
	signal invert_request 	: std_logic_vector(99 downto 0);
	signal select_request 	: std_logic_vector(99 downto 0);
	-- signaly pro praci s nastavenim obtiznosti
	signal init_reset 		: std_logic := '1';
	signal init_enter 		: std_logic := '0';
	signal zacatek 			: std_logic_vector(1 downto 0);
	-- signaly pro praci s citacem
	signal char_symbol 		: std_logic_vector(3 downto 0) := "1010";
	signal char_data 		: std_logic;
	-- citac
	signal bcd_clk, bcd_reset : std_logic;
	signal snumber1		 	: std_logic_vector(3 downto 0);
	signal snumber2		 	: std_logic_vector(3 downto 0);
	signal snumber3		 	: std_logic_vector(3 downto 0);
	
	-- indexy sousednich bunek
	constant IDX_TOP    : NATURAL := 0; -- nahore
    constant IDX_LEFT   : NATURAL := 1; -- vlevo
    constant IDX_RIGHT  : NATURAL := 2; -- vpravo
    constant IDX_BOTTOM : NATURAL := 3; -- dole
	constant IDX_ENTER  : NATURAL := 4; -- enter
	
	-- definovani a pripojeni bunky - cell.vhdl
	component cell
		generic(
			MASK : mask_t := (others => '1')
		);	
		port(
			KEYS 			: IN  	std_logic_vector(4 downto 0);
			INVERT_REQ_IN 	: IN  	std_logic_vector(3 downto 0);
			SELECT_REQ_IN 	: IN  	std_logic_vector(3 downto 0);
			INIT_ACTIVE 	: IN  	std_logic;
			INIT_SELECTED 	: IN  	std_logic;
			CLK 			: IN  	std_logic;
			RESET 			: IN  	std_logic;
			ACTIVE 			: OUT  	std_logic;
			SELECTED 		: OUT  	std_logic;
			INVERT_REQ_OUT 	: OUT  	std_logic_vector(3 downto 0);
			SELECT_REQ_OUT 	: OUT  	std_logic_vector(3 downto 0)
		);
    end component;
	
	-- Klavesnice
	component keyboard_controller
		port(
			CLK      : in 	std_logic;
			RST      : in 	std_logic;
			KB_KOUT  : in  	std_logic_vector(3 downto 0);
			KB_KIN   : out 	std_logic_vector(3 downto 0);
			DATA_OUT : out 	std_logic_vector(15 downto 0);
			DATA_VLD : out 	std_logic
		);
	end component;

begin	
	vga: entity work.vga_controller(arch_vga_controller) 
		generic map (REQ_DELAY => 1)
		port map (
			CLK    => CLK, 
			RST    => RESET,
			ENABLE => '1',
			MODE   => vga_mode,

			DATA_RED    => irgb(8 downto 6),
			DATA_GREEN  => irgb(5 downto 3),
			DATA_BLUE   => irgb(2 downto 0),
			ADDR_COLUMN => col,
			ADDR_ROW    => row,

			VGA_RED   => RED_V,
			VGA_BLUE  => BLUE_V,
			VGA_GREEN => GREEN_V,
			VGA_HSYNC => HSYNC_V,
			VGA_VSYNC => VSYNC_V
		);
		
	-- Nastaveni rozliseni a frekvence (640x480, 60Hz)
	setmode(r640x480x60, vga_mode);	
		
	-- Ovladani klavesnice
	kbrd_ctrl: keyboard_controller
		port map (
			CLK => SMCLK,
			RST => RESET,
			KB_KIN   => KIN,
			KB_KOUT  => KOUT,
			DATA_OUT => kbrd_data_out(15 downto 0),
			DATA_VLD => kbrd_data_vld
		);

	-- dekoder cislic 0-9
	chardec : entity work.char_rom
		port map (
			ADDRESS => char_symbol,
			ROW => row(3 downto 0),
			COLUMN => col(2 downto 0),
			DATA => char_data
		);

	-- citac akci
	bcd: entity work.bcd
		port map (
			CLK => bcd_clk,
			RESET => bcd_reset,

			NUMBER1 => snumber1,
			NUMBER2 => snumber2,
			NUMBER3 => snumber3
		);

	-- generovani pole o velikosti 5 x 5
	GEN: for y in 1 to 5 generate
	begin
		GEN1: for x in 1 to 5 generate
		begin
			CELL_INST: entity work.cell 
			generic map(
				MASK => getmask(x-1,y-1,5,5)
			)	
			port map(		
				CLK => CLK,
				RESET => init_reset,
				ACTIVE => active((y-1)*5+(x-1)),
				SELECTED => selected((y-1)*5+(x-1)),
				INIT_ACTIVE => init_active((y-1)*5+(x-1)),
				INIT_SELECTED => init_selected((y-1)*5+(x-1)),
				INVERT_REQ_IN => invert_request(vypocet(x,y,IDX_BOTTOM) downto vypocet(x,y,IDX_TOP)), 
				INVERT_REQ_OUT(IDX_TOP) => invert_request(vypocet(x,y-1,IDX_BOTTOM)), 
				INVERT_REQ_OUT(IDX_LEFT) => invert_request(vypocet(x-1,y,IDX_RIGHT)),
				INVERT_REQ_OUT(IDX_RIGHT) => invert_request(vypocet(x+1,y,IDX_LEFT)),
				INVERT_REQ_OUT(IDX_BOTTOM) => invert_request(vypocet(x,y+1,IDX_TOP)),
				SELECT_REQ_IN => select_request(vypocet(x,y,IDX_BOTTOM) downto vypocet(x,y,IDX_TOP)),
				SELECT_REQ_OUT(IDX_TOP) => select_request(vypocet(x,y-1,IDX_BOTTOM)),
				SELECT_REQ_OUT(IDX_LEFT) => select_request(vypocet(x-1,y,IDX_RIGHT)),
				SELECT_REQ_OUT(IDX_RIGHT) => select_request(vypocet(x+1,y,IDX_LEFT)),
				SELECT_REQ_OUT(IDX_BOTTOM) => select_request(vypocet(x,y+1,IDX_TOP)),
				KEYS => kboard
			);
		end generate GEN1;
    end generate GEN;	

	-- prace s klavesnici a kurzorem
	cursor: process
		variable in_access : std_logic := '0';
	begin
		if CLK'event and CLK='1' then     
			kboard <= "00000";
			init_reset <= '0';
			init_enter <= '0';
			
			if in_access='0' then
				if kbrd_data_vld='1' then 
					in_access:='1';
					-- klavesa '6' (pohyb doprava)
					if kbrd_data_out(9)='1' and cur_x(4)/='1' then 
						cur_x <= cur_x(3 downto 0) & cur_x(4);	
						kboard(2)<='1';
					-- klavesa '4' (pohyb doleva)		
					elsif kbrd_data_out(1)='1' and cur_x(0)/='1' then
						cur_x <= cur_x(0) & cur_x(4 downto 1); 
						kboard(1)<='1';
					-- klavesa '2' (pohyb nahoru)	
					elsif kbrd_data_out(4)='1' and cur_y(0)/='1' then
						cur_y <= cur_y(0) & cur_y(4 downto 1);
						kboard(0)<='1';
					-- klavesa '8' (pohyb dolu)		
					elsif kbrd_data_out(6)='1' and cur_y(4)/='1' then
						cur_y <= cur_y(3 downto 0) & cur_y(4);
						kboard(3)<='1';
					-- klavesa '5' (změna stavu)		
					elsif kbrd_data_out(5)='1' then
						init_enter <= '1';
						kboard(4)<='1';
					-- klavesa 'A' (nastaveni obtiznosti c. 1)		
					elsif kbrd_data_out(12)='1' then
						-- vyresetovani
						init_reset <= '1';
						-- volba startovniho rozmisteni
						zacatek <= "00";
					-- klavesa 'B' (nastaveni obtiznosti c. 2)		
					elsif kbrd_data_out(13)='1' then
						-- vyresetovani
						init_reset <= '1';
						-- volba startovniho rozmisteni
						zacatek <= "01";
					-- klavesa 'C' (nastaveni obtiznosti c. 3)		
					elsif kbrd_data_out(14)='1' then
						-- vyresetovani
						init_reset <= '1';
						-- volba startovniho rozmisteni
						zacatek <= "10";
					-- klavesa 'D' (nastaveni obtiznosti c. 4)		
					elsif kbrd_data_out(15)='1' then
						-- vyresetovani
						init_reset <= '1';
						-- volba startovniho rozmisteni
						zacatek <= "11";
					end if;
				end if;
			else
				if kbrd_data_vld='0' then 
					in_access:='0';
				end if;
			end if;
			-- nastaveni policek pro jednotlive moznosti zacatku hry
			if (init_reset = '1') then
				if (zacatek="00") then
					-- nastaveni kurzoru na stred
					cur_x <= "00100";
					cur_y <= "00100";	
					-- nastaveni vybraneho policka na stred 
					init_selected <= "0000000000001000000000000";
					-- nastaveni aktivnich poli
					init_active <= "1111111111111111111111111";
				elsif (zacatek="01") then
					-- nastaveni kurzoru na stred
					cur_x <= "00100";
					cur_y <= "00100";	
					-- nastaveni vybraneho policka na stred 
					init_selected <= "0000000000001000000000000";
					-- nastaveni aktivnich poli
					init_active <= "0100100100100100100101110";
				elsif (zacatek="10") then
					-- nastaveni kurzoru na stred
					cur_x <= "00100";
					cur_y <= "00100";	
					-- nastaveni vybraneho policka na stred 
					init_selected <= "0000000000001000000000000";
					-- nastaveni aktivnich poli
					init_active <= "1110001111010011101001001";
				elsif (zacatek="11") then
					-- nastaveni kurzoru na stred
					cur_x <= "00100";
					cur_y <= "00100";	
					-- nastaveni vybraneho policka na stred 
					init_selected <= "0000000000001000000000000";
					-- nastaveni aktivnich poli
					init_active <= "1100110000001010100100111";
				end if;
			end if;
		end if;
	end process;   	

		
	setgraph : process (row, col)
	begin
		if CLK'event and CLK='1' then
			-- nastaveni vychozi barvy (pozadi)
			irgb <= "000000000"; -- cerna
			-- roznuta svetla 
			color1 <= "111000000";
			-- zhasnuta svetla
			color2 <= "111110000";
			-- barva kurzoru
			color3 <= "000000000";
			
			-- nastaveni barev "sachovnice"
			-- prvni sloupec
			if col>155 and col<221 then
				-- prvni radek
				if row>75 and row<141 then
					if active(0)='1' then -- nastaveni barvy na zaklade aktivity bunky
						irgb <= color1;
					else
						irgb <= color2;
					end if;
				-- druhy radek
				elsif row>141 and row<207 then
					if active(5)='1' then -- nastaveni barvy na zaklade aktivity bunky
						irgb <= color1;
					else
						irgb <= color2;
					end if;
				-- treti radek
				elsif row>207 and row<273 then
					if active(10)='1' then -- nastaveni barvy na zaklade aktivity bunky
						irgb <= color1;
					else
						irgb <= color2;
					end if;
				-- ctvrty radek
				elsif row>273 and row<339 then
					if active(15)='1' then -- nastaveni barvy na zaklade aktivity bunky
						irgb <= color1;
					else
						irgb <= color2;
					end if;
				-- paty radek
				elsif row>339 and row<405 then
					if active(20)='1' then -- nastaveni barvy na zaklade aktivity bunky
						irgb <= color1;
					else
						irgb <= color2;
					end if;
				end if;
			-- druhy sloupec
			elsif col>221 and col<287 then
				-- prvni radek
				if row>75 and row<141 then
					if active(1)='1' then -- nastaveni barvy na zaklade aktivity bunky
						irgb <= color1;
					else
						irgb <= color2;
					end if;
				-- druhy radek
				elsif row>141 and row<207 then
					if active(6)='1' then -- nastaveni barvy na zaklade aktivity bunky
						irgb <= color1;
					else
						irgb <= color2;
					end if;
				-- treti radek
				elsif row>207 and row<273 then
					if active(11)='1' then -- nastaveni barvy na zaklade aktivity bunky
						irgb <= color1;
					else
						irgb <= color2;
					end if;
				-- ctvrty radek
				elsif row>273 and row<339 then
					if active(16)='1' then -- nastaveni barvy na zaklade aktivity bunky
						irgb <= color1;
					else
						irgb <= color2;
					end if;
				-- paty radek
				elsif row>339 and row<405 then
					if active(21)='1' then -- nastaveni barvy na zaklade aktivity bunky
						irgb <= color1;
					else
						irgb <= color2;
					end if;
				end if;
			-- treti sloupec
			elsif col>287 and col<353 then
				-- prvni radek
				if row>75 and row<141 then
					if active(2)='1' then -- nastaveni barvy na zaklade aktivity bunky
						irgb <= color1;
					else
						irgb <= color2;
					end if;
				-- druhy radek
				elsif row>141 and row<207 then
					if active(7)='1' then -- nastaveni barvy na zaklade aktivity bunky
						irgb <= color1;
					else
						irgb <= color2;
					end if;
				-- treti radek
				elsif row>207 and row<273 then
					if active(12)='1' then -- nastaveni barvy na zaklade aktivity bunky
						irgb <= color1;
					else
						irgb <= color2;
					end if;
				-- ctvrty radek
				elsif row>273 and row<339 then
					if active(17)='1' then -- nastaveni barvy na zaklade aktivity bunky
						irgb <= color1;
					else
						irgb <= color2;
					end if;
				-- paty radek
				elsif row>339 and row<405 then
					if active(22)='1' then -- nastaveni barvy na zaklade aktivity bunky
						irgb <= color1;
					else
						irgb <= color2;
					end if;
				end if;
			-- ctvrty sloupec
			elsif col>353 and col<419 then
				-- prvni radek
				if row>75 and row<141 then
					if active(3)='1' then -- nastaveni barvy na zaklade aktivity bunky
						irgb <= color1;
					else
						irgb <= color2;
					end if;
				-- druhy radek
				elsif row>141 and row<207 then
					if active(8)='1' then -- nastaveni barvy na zaklade aktivity bunky
						irgb <= color1;
					else
						irgb <= color2;
					end if;
				-- treti radek
				elsif row>207 and row<273 then
					if active(13)='1' then -- nastaveni barvy na zaklade aktivity bunky
						irgb <= color1;
					else
						irgb <= color2;
					end if;
				-- ctvrty radek
				elsif row>273 and row<339 then
					if active(18)='1' then -- nastaveni barvy na zaklade aktivity bunky
						irgb <= color1;
					else
						irgb <= color2;
					end if;
				-- paty radek
				elsif row>339 and row<405 then
					if active(23)='1' then -- nastaveni barvy na zaklade aktivity bunky
						irgb <= color1;
					else
						irgb <= color2;
					end if;
				end if;
			-- paty sloupec
			elsif col>419 and col<485 then
				-- prvni radek
				if row>75 and row<141 then
					if active(4)='1' then -- nastaveni barvy na zaklade aktivity bunky
						irgb <= color1;
					else
						irgb <= color2;
					end if;
				-- druhy radek
				elsif row>141 and row<207 then
					if active(9)='1' then -- nastaveni barvy na zaklade aktivity bunky
						irgb <= color1;
					else
						irgb <= color2;
					end if;
				-- treti radek
				elsif row>207 and row<273 then
					if active(14)='1' then -- nastaveni barvy na zaklade aktivity bunky
						irgb <= color1;
					else
						irgb <= color2;
					end if;
				-- ctvrty radek
				elsif row>273 and row<339 then
					if active(19)='1' then -- nastaveni barvy na zaklade aktivity bunky
						irgb <= color1;
					else
						irgb <= color2;
					end if;
				-- paty radek
				elsif row>339 and row<405 then
					if active(24)='1' then -- nastaveni barvy na zaklade aktivity bunky
						irgb <= color1;
					else
						irgb <= color2;
					end if;
				end if;
			end if;
			-- prace s kurzorem	
			case cur_x is 
				-- prvni sloupec
				when "00001" =>
					case cur_y is
						-- prvni radek
						when "00001" =>
							if col>171 and col<205 and row>91 and row<125 then -- kurzor ma polovicni velikost nez je bunka a je umisten ve stredu
								irgb <= color3; -- vymezení a nastaveni barvy kurzoru
							end if;
						-- druhy radek
						when "00010" =>
							if col>171 and col<205 and row>156 and row<190 then
								irgb <= color3; -- vymezení a nastaveni barvy kurzoru
							end if;
						-- treti radek
						when "00100" =>
							if col>171 and col<205 and row>222 and row<256 then
								irgb <= color3; -- vymezení a nastaveni barvy kurzoru
							end if;
						-- ctvrty radek
						when "01000" =>
							if col>171 and col<205 and row>288 and row<322 then
								irgb <= color3; -- vymezení a nastaveni barvy kurzoru
							end if;
						-- paty radek
						when "10000" =>
							if col>171 and col<205 and row>354 and row<388 then
								irgb <= color3; -- vymezení a nastaveni barvy kurzoru
							end if;
						when others =>
					end case;
				-- druhy sloupec
				when "00010" =>
					case cur_y is
						-- prvni radek
						when "00001" =>
							if col>237 and col<271 and row>91 and row<125 then
								irgb <= color3; -- vymezení a nastaveni barvy kurzoru
							end if;
						-- druhy radek
						when "00010" =>
							if col>237 and col<271 and row>156 and row<190 then
								irgb <= color3; -- vymezení a nastaveni barvy kurzoru
							end if;
						-- treti radek
						when "00100" =>
							if col>237 and col<271 and row>222 and row<256 then
								irgb <= color3; -- vymezení a nastaveni barvy kurzoru
							end if;
						-- ctvrty radek
						when "01000" =>
							if col>237 and col<271 and row>288 and row<322 then
								irgb <= color3; -- vymezení a nastaveni barvy kurzoru
							end if;
						-- paty radek
						when "10000" =>
							if col>237 and col<271 and row>354 and row<388 then
								irgb <= color3; -- vymezení a nastaveni barvy kurzoru
							end if;
						when others =>
					end case;
				-- treti sloupec
				when "00100" =>
					case cur_y is
						-- prvni radek
						when "00001" =>
							if col>303 and col<337 and row>91 and row<125 then
								irgb <= color3; -- vymezení a nastaveni barvy kurzoru
							end if;
						-- druhy radek
						when "00010" =>
							if col>303 and col<337 and row>156 and row<190 then
								irgb <= color3; -- vymezení a nastaveni barvy kurzoru
							end if;
						-- treti radek
						when "00100" =>
							if col>303 and col<337 and row>222 and row<256 then
								irgb <= color3; -- vymezení a nastaveni barvy kurzoru
							end if;
						-- ctvrty radek
						when "01000" =>
							if col>303 and col<337 and row>288 and row<322 then
								irgb <= color3; -- vymezení a nastaveni barvy kurzoru
							end if;
						-- paty radek
						when "10000" =>
							if col>303 and col<337 and row>354 and row<388 then
								irgb <= color3; -- vymezení a nastaveni barvy kurzoru
							end if;
						when others =>
					end case;
				-- ctvrty sloupec
				when "01000" =>
					case cur_y is
						-- prvni radek
						when "00001" =>
							if col>369 and col<403 and row>91 and row<125 then
								irgb <= color3; -- vymezení a nastaveni barvy kurzoru
							end if;
						-- druhy radek
						when "00010" =>
							if col>369 and col<403 and row>156 and row<190 then
								irgb <= color3; -- vymezení a nastaveni barvy kurzoru
							end if;
						-- treti radek
						when "00100" =>
							if col>369 and col<403 and row>222 and row<256 then
								irgb <= color3; -- vymezení a nastaveni barvy kurzoru
							end if;
						-- ctvrty radek
						when "01000" =>
							if col>369 and col<403 and row>288 and row<322 then
								irgb <= color3; -- vymezení a nastaveni barvy kurzoru
							end if;
						-- paty radek
						when "10000" =>
							if col>369 and col<403 and row>354 and row<388 then
								irgb <= color3; -- vymezení a nastaveni barvy kurzoru
							end if;
						when others =>
					end case;
				-- paty sloupec
				when "10000" =>
					case cur_y is
						-- prvni radek
						when "00001" =>
							if col>435 and col<469 and row>91 and row<125 then
								irgb <= color3; -- vymezení a nastaveni barvy kurzoru
							end if;
						-- druhy radek
						when "00010" =>
							if col>435 and col<469 and row>156 and row<190 then
								irgb <= color3; -- vymezení a nastaveni barvy kurzoru
							end if;
						-- treti radek
						when "00100" =>
							if col>435 and col<469 and row>222 and row<256 then
								irgb <= color3; -- vymezení a nastaveni barvy kurzoru
							end if;
						-- ctvrty radek
						when "01000" =>
							if col>435 and col<469 and row>288 and row<322 then
								irgb <= color3; -- vymezení a nastaveni barvy kurzoru
							end if;
						-- paty radek
						when "10000" =>
							if col>435 and col<469 and row>354 and row<388 then
								irgb <= color3; -- vymezení a nastaveni barvy kurzoru
							end if;
						when others =>
					end case;
				when others =>
			end case;
		end if;
    end process;
end main;