-- cpu.vhd: Simple 8-bit CPU (BrainLove interpreter)
-- Copyright (C) 2016 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Tomas Aubrecht (xaubre02)
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (1) / zapis (0)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0' 
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is
	-- prace s CODE_ADDR
	signal pc_reg : std_logic_vector(11 downto 0);
	signal pc_inc : std_logic;
	signal pc_dec : std_logic;
	-- prace s DATA_ADDR
	signal ptr_reg : std_logic_vector(9 downto 0);
	signal ptr_inc : std_logic;
	signal ptr_dec : std_logic;
	-- povolovaci signaly
	signal pc_allow : std_logic;
	signal ptr_allow : std_logic;
	-- pomocne signaly
	signal sel : std_logic_vector(2 downto 0);
	signal tmp : std_logic_vector(7 downto 0);
	signal tmp_allow : std_logic;
	-- prace s pocitadlem
	signal cnt_reg : std_logic_vector(7 downto 0);
	signal cnt_inc : std_logic;
	signal cnt_dec : std_logic;

--	Prace s instrukcemi
	type ins_type is 
	(
		iPtrInc, iPtrDec, iValInc, iValDec, iWhileS, iWhileE, iPrint, iGet, iSave, iLoad, iSkip, iHalt
	);
	
	signal ireg_dec : ins_type;
	signal ireg_reg : std_logic_vector(7 downto 0);
	signal ireg_ld  : std_logic;
	
 -- Stavy konecneho automatu
	type fsm_state is 
	(
		idle, fetch0, fetch1, decode,
		ptrInc, ptrInc2, ptrDec, valInc, valInc2, valDec, valDec2, 
		WhileS, WhileS2, WhileS3, WhileSFetch0, WhileSFetch1, WhileE, WhileE2, WhileE3, WhileEFetch0, WhileEFetch1,
		print, print2, get, get2, save, save2, load, halt, skip
	);
	
	signal pstate : fsm_state;
	signal nstate : fsm_state;

begin
-- ----------------------------------------------------------------------------
--                      Program Counter
-- ----------------------------------------------------------------------------
	pc_cntr: process (RESET, CLK)
	begin
		if (RESET = '1') then
			pc_reg <= (others => '0');
		elsif (CLK'event) and (CLK = '1') then
			-- inkrementace obsahu PC registru
			if (pc_inc = '1') then
				pc_reg <= pc_reg + 1;
			-- dekrementace obsahu PC registru
			elsif (pc_dec = '1') then 
				pc_reg <= pc_reg - 1;
			end if;
		end if;
	end process;
	-- změna adresy v pc registru, pokud má povolení
	CODE_ADDR <= pc_reg when pc_allow = '1' else
		(others => 'Z');

-- ----------------------------------------------------------------------------
--                      Instruction Register
-- ----------------------------------------------------------------------------
	ireg: process (RESET, CLK)
	begin
		if (RESET = '1') then
			ireg_reg <= (others => '0');
		elsif (CLK'event) and (CLK = '1') then
			-- nacteni instrukce do pomocneho signalu
			if (ireg_ld = '1') then
				ireg_reg <= CODE_DATA;
			end if;
		end if;
	end process;

-- ----------------------------------------------------------------------------
--                      Instruction Decoder
-- ----------------------------------------------------------------------------
	process (ireg_reg)
	begin
		case ireg_reg is
			when X"3E" => ireg_dec <= iPtrInc;  -- >
			when X"3C" => ireg_dec <= iPtrDec;  -- <
			when X"2B" => ireg_dec <= iValInc;  -- +
			when X"2D" => ireg_dec <= iValDec;  -- -
			when X"5B" => ireg_dec <= iWhileS;  -- [
			when X"5D" => ireg_dec <= iWhileE;  -- ]
			when X"2E" => ireg_dec <= iPrint;   -- .
			when X"2C" => ireg_dec <= iGet;     -- ,
			when X"24" => ireg_dec <= iSave;    -- $
			when X"21" => ireg_dec <= iLoad;    -- !
			when X"00" => ireg_dec <= iHalt;    -- null
			when others => ireg_dec <= iSkip;   -- Komentare
		end case;
	end process;
	
-- ----------------------------------------------------------------------------
--                      Pointer Processing
-- ----------------------------------------------------------------------------
	pointer: process(RESET, CLK)
	begin
		if (RESET = '1') then
			ptr_reg <= (others => '0');
		elsif (CLK'event) and (CLK = '1') then
			-- inkrementace ukazatele do pameti
			if (ptr_inc = '1') then
				ptr_reg <= ptr_reg + 1;
			-- dekrementace ukazatele do pameti
			elsif (ptr_dec = '1') then 
				ptr_reg <= ptr_reg - 1;
			end if;
		end if;
	end process;
	-- změna adresy v paměti, pokud má povolení
	DATA_ADDR <= ptr_reg when ptr_allow = '1' else
		(others => 'Z');
	
-- ----------------------------------------------------------------------------
--                      Counter Register
-- ----------------------------------------------------------------------------
	counter: process (RESET, CLK)
	begin
		if (RESET = '1') then
			cnt_reg <= (others => '0');
		elsif (CLK'event) and (CLK = '1') then
			-- inkrementace pocitadla
			if (cnt_inc = '1') then
				cnt_reg <= cnt_reg + 1;
			end if;
			-- dekrementace pocitadla
			if (cnt_dec = '1') then 
				cnt_reg <= cnt_reg - 1;
			end if;
		end if;
	end process;

-- ----------------------------------------------------------------------------
--                      TMP Register
-- ----------------------------------------------------------------------------
	tmp_reg: process (RESET, CLK)
	begin
		if (RESET = '1') then
			tmp <= (others => '0');
		elsif (CLK'event) and (CLK = '1') then
			if (tmp_allow = '1') then 
				tmp <= DATA_RDATA;
			end if;
		end if;
	end process;

-- ----------------------------------------------------------------------------
--                      Multiplexor for WDATA
-- ----------------------------------------------------------------------------
	process_mx: process (sel, tmp, DATA_RDATA, IN_DATA)
	begin
		case sel is
			when "000" => DATA_WDATA <= DATA_RDATA;
			when "001" => DATA_WDATA <= DATA_RDATA + 1 ;
			when "010" => DATA_WDATA <= DATA_RDATA - 1;
			when "011" => DATA_WDATA <= IN_DATA;
			when "100" => DATA_WDATA <= tmp;
			when others => DATA_WDATA <= (others => 'X');
		end case;
	end process;
	
-- ----------------------------------------------------------------------------
--                      FSM - Present State
-- ----------------------------------------------------------------------------
	fsm_pstate: process (CLK, RESET, EN)
	begin
		if (RESET = '1') then
			pstate <= idle;
		elsif (CLK'event) and (CLK = '1') then
			if (EN = '1') then
				pstate <= nstate;
			end if;
		end if;
	end process;
	
-- ----------------------------------------------------------------------------
--                      Finite State Machine
-- ----------------------------------------------------------------------------
	next_state_logic: process (pstate, ireg_dec, cnt_reg, CODE_DATA, DATA_RDATA, OUT_BUSY, IN_VLD)
	begin
		-- Inicializace automatu
		nstate <= idle;
		
		DATA_RDWR <= '1';
		DATA_EN <= '0';
		CODE_EN <= '0';
		IN_REQ <= '0';
		OUT_WE  <= '0';
		OUT_DATA <= (others => 'Z');
		
		pc_allow <= '0';
		ptr_allow <= '0';
		tmp_allow <= '0';
		ireg_ld <= '0';
		pc_inc <= '0';
		pc_dec <= '0';
		ptr_inc <= '0';
		ptr_dec <= '0';
		cnt_inc <= '0';
		cnt_dec <= '0';

		sel <= "000";

		
		case pstate is

			-- Necinny stav
			when idle =>
				nstate <= fetch0;
				
			-- Nacitani instrukce
			when fetch0 =>
				nstate <= fetch1;
				pc_allow <= '1';
				CODE_EN <= '1'; 
			
			-- Cteni dalsi instrukce
			when fetch1 =>
				nstate <= decode;
				ireg_ld <= '1';
				
			-- Dekodovani instrukce
			when decode =>
				case ireg_dec is 
					when iPtrInc => nstate <= ptrInc;
					when iPtrDec => nstate <= ptrDec;
					when iValInc => nstate <= valInc;
					when iValDec => nstate <= valDec;
					when iWhileS => nstate <= WhileS;
					when iWhileE => nstate <= WhileE;
					when iPrint => nstate <= print;
					when iGet => nstate <= get;
					when iSave => nstate <= save;
					when iLoad => nstate <= load;
					when iHalt => nstate <= halt;
					when others => nstate <= skip;
				end case;
			
			-- Zastaveni cinnosti procesoru
			when halt =>
				nstate <= halt;
			
			-- Preskoceni instrukce -> zvyseni hodnoty pc registru
			when skip =>
				nstate <= fetch0;
				pc_inc <= '1';
				
			-- Inkrementace ukazatele
			when ptrInc =>
				nstate <= fetch0;
				ptr_inc <= '1';
				pc_inc <= '1';
				
			-- Dekrementace ukazatele
			when ptrDec =>
				nstate <= fetch0;
				ptr_dec <= '1';
				pc_inc <= '1';
			
			-- Inkrementace hodnoty buňky
			when valInc =>
				nstate <= valInc2;
				ptr_allow <= '1';
				DATA_EN <= '1';
				DATA_RDWR <= '1'; -- cteni hodnoty bunky
				sel <= "001";
				
			when valInc2 =>
				nstate <= fetch0;
				ptr_allow <= '1';
				DATA_EN <= '1';
				DATA_RDWR <= '0'; -- zapis
				sel <= "001"; -- inkrementace hodnoty bunky
				pc_inc <= '1';
			
			-- Dekrementace hodnoty buňky
			when valDec =>
				nstate <= valDec2;
				ptr_allow <= '1';
				DATA_EN <= '1';
				DATA_RDWR <= '1'; -- cteni hodnoty bunky
				sel <= "010";
				
			when valDec2 =>
				nstate <= fetch0;
				ptr_allow <= '1';
				DATA_EN <= '1';
				DATA_RDWR <= '0'; -- zapis
				sel <= "010"; -- dekrementace hodnoty bunky
				pc_inc <= '1';
				
			-- Vytisteni hodnoty bunky na displej
			when print =>
				nstate <= print2;
				ptr_allow <= '1';
				DATA_EN <= '1';
				DATA_RDWR <= '1'; -- cteni hodntoy
				
			when print2 =>
				nstate <= print2;
				if (OUT_BUSY = '0') then -- cekani na uvolneni displeje
					nstate <= fetch0;
					OUT_WE <= '1';
					OUT_DATA <= DATA_RDATA; -- zobrazeni na displej
					pc_inc <= '1';
				end if;
				
			-- Ziskani a ulozeni hodnoty
			when get =>
				nstate <= get;
				IN_REQ <= '1';
				if (IN_VLD = '1') then -- cekani na povoleni
					nstate <= get2;
				end if;
			
			when get2 =>
				nstate <= fetch0;
				ptr_allow <= '1';
				DATA_EN <= '1';
				DATA_RDWR <= '0'; -- zapsani zadane hodnoty
				sel <= "011";
				pc_inc <= '1';
				
			-- Ulozeni hodnoty aktualni bunky do pomocne promenne
			when save =>
				nstate <= save2;
				ptr_allow <= '1';
				DATA_EN <= '1';
				DATA_RDWR <= '1';
			
			when save2 =>
				nstate <= fetch0;
				tmp_allow <= '1';
				pc_inc <= '1';

			-- Zapsani hodnoty z pomocne promenne do aktualni bunky
			when load =>
				nstate <= fetch0;
				ptr_allow <= '1';
				DATA_EN <= '1';
				DATA_RDWR <= '0';
				sel <= "100";
				pc_inc <= '1';
			
			-- Zacatek cyklu
			when WhileS =>
				nstate <= WhileS2;
				ptr_allow <= '1';
				DATA_EN <= '1';
				DATA_RDWR <= '1'; -- cteni hodnoty bunky
			
			when WhileS2 =>
				if (DATA_RDATA = "00000000") then -- je-li nulova, bude se ignorovat az po prislusnou ]
					nstate <= WhileSFetch0;
					cnt_inc <= '1';
				else
					nstate <= fetch0; -- jinak se bude vykonavat dana instrukce
				end if;
				pc_inc <= '1'; -- nacteni dalsi instrukce behem dalsich dvou stavu
				
			when WhileSFetch0 =>
				nstate <= WhileSFetch1;
				pc_allow <= '1';
				CODE_EN <= '1'; 

			when WhileSFetch1 =>
				nstate <= WhileS3;
				ireg_ld <= '1';
			
			when WhileS3 =>
				if (cnt_reg /= "00000000") then -- prace s pocitadle, kdy pri nalezeni vnorene [ se pocitadlo zvysuje a pri ] se snizuje
					if (ireg_dec = iWhileS) then -- [
						cnt_inc <= '1';
					elsif (ireg_dec = iWhileE) then -- ]
						cnt_dec <= '1';
					end if;
					nstate <= WhileSFetch0;
					pc_inc <= '1';
				else
					nstate <= decode; -- tato instrukce se jiz nachazi za posledni ]
				end if;
			
			-- Konec cyklu - obdobne jako zacatek cyklu
			when WhileE =>
				nstate <= WhileE2;
				ptr_allow <= '1';
				DATA_EN <= '1';
				DATA_RDWR <= '1';
				
			when WhileE2 =>
				if (DATA_RDATA = "00000000") then
					nstate <= fetch0;
					pc_inc <= '1';
				else
					nstate <= WhileEFetch0;
					cnt_inc <= '1';
					pc_dec <= '1';
				end if;
			
			when WhileEFetch0 =>
				nstate <= WhileEFetch1;
				pc_allow <= '1';
				CODE_EN <= '1'; 

			when WhileEFetch1 =>
				nstate <= WhileE3;
				ireg_ld <= '1';
			
			when WhileE3 =>
				if (cnt_reg /= "00000000") then
					if (ireg_dec = iWhileS) then -- [
						cnt_dec <= '1';
					elsif (ireg_dec = iWhileE) then -- ]
						cnt_inc <= '1';
					end if;
					nstate <= WhileEFetch0;
					pc_dec <= '1';
				else
					nstate <= fetch0;
					pc_inc <= '1';
				end if;
				
			when others => null;
		end case;
	end process;
end behavioral;