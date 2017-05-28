LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;
USE work.math_pack.ALL;
 
ENTITY cell_tb IS
END cell_tb;
 
ARCHITECTURE behavior OF cell_tb IS 
 
    COMPONENT cell
    PORT(
         INVERT_REQ_IN : IN  std_logic_vector(3 downto 0);
         INVERT_REQ_OUT : OUT  std_logic_vector(3 downto 0);
         KEYS : IN  std_logic_vector(4 downto 0);
         SELECT_REQ_IN : IN  std_logic_vector(3 downto 0);
         SELECT_REQ_OUT : OUT  std_logic_vector(3 downto 0);
         INIT_ACTIVE : IN  std_logic;
         ACTIVE : OUT  std_logic;
         INIT_SELECTED : IN  std_logic;
         SELECTED : OUT  std_logic;
         CLK : IN  std_logic;
			RESET : IN  std_logic
			
        );
    END COMPONENT;
	 
   --Constants
	constant IDX_TOP    : NATURAL := 0;
	constant IDX_LEFT   : NATURAL := 1;
	constant IDX_RIGHT  : NATURAL := 2;
	constant IDX_BOTTOM : NATURAL := 3;
	constant IDX_ENTER  : NATURAL := 4;

	
   --Inputs
   signal INVERT_REQ_IN : std_logic_vector(3 downto 0) := (others => '0');
   signal KEYS : std_logic_vector(4 downto 0) := (others => '0');
   signal SELECT_REQ_IN : std_logic_vector(3 downto 0) := (others => '0');
   signal INIT_ACTIVE : std_logic := '0';
   signal INIT_SELECTED : std_logic := '0';
   signal CLK : std_logic := '0';
   signal RESET : std_logic := '0';
	
 	--Outputs
   signal INVERT_REQ_OUT : std_logic_vector(3 downto 0);
   signal SELECT_REQ_OUT : std_logic_vector(3 downto 0);
   signal ACTIVE : std_logic;
   signal SELECTED : std_logic;

   -- Clock period definitions
   constant CLK_period : time := 10 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: cell PORT MAP (
          INVERT_REQ_IN => INVERT_REQ_IN,
          INVERT_REQ_OUT => INVERT_REQ_OUT,
          KEYS => KEYS,
          SELECT_REQ_IN => SELECT_REQ_IN,
          SELECT_REQ_OUT => SELECT_REQ_OUT,
          INIT_ACTIVE => INIT_ACTIVE,
          ACTIVE => ACTIVE,
          INIT_SELECTED => INIT_SELECTED,
          SELECTED => SELECTED,
          CLK => CLK,
          RESET => RESET
        );

   -- Clock process definitions
   CLK_process :process
   begin
		CLK <= '0';
		wait for CLK_period/2;
		CLK <= '1';
		wait for CLK_period/2;
   end process;
 

   stim_proc: process
	begin
	
		RESET <= '0';
		wait for 20 ns;
		
		INIT_ACTIVE <= '1';
		INIT_SELECTED <= '1';
		RESET <= '1';
		
		wait for CLK_period;
		assert ACTIVE = '1' and SELECTED = '1' report "Chyba reset!";
		
		RESET <= '0';
		wait for 20 ns;

		INIT_ACTIVE <= '0';
		INIT_SELECTED <= '0';
		RESET <= '1';
		
		wait for CLK_period;
		assert ACTIVE = '0' and SELECTED = '0' report "Chyba reset #2!";
		
		RESET <= '0';
		wait for 20 ns;
		
		SELECT_REQ_IN(IDX_TOP) <= '1';
		wait for CLK_period;
		assert SELECTED = '1' report "Chyba u hodnoty SELECTED!";
		wait for 10 ns;
		SELECT_REQ_IN(IDX_TOP) <= '0';
		
		SELECT_REQ_IN(IDX_LEFT) <= '1';
		wait for CLK_period;
		assert SELECTED = '1' report "Chyba u hodnoty SELECTED!";
		wait for 10 ns;
		SELECT_REQ_IN(IDX_LEFT) <= '0';
		
		SELECT_REQ_IN(IDX_RIGHT) <= '1';
		wait for CLK_period;
		assert SELECTED = '1' report "Chyba u hodnoty SELECTED!";
		wait for 10 ns;
		SELECT_REQ_IN(IDX_RIGHT) <= '0';
		
		SELECT_REQ_IN(IDX_BOTTOM) <= '1';
		wait for CLK_period;
		assert SELECTED = '1' report "Chyba u hodnoty SELECTED!";
		
		INVERT_REQ_IN(IDX_TOP) <= '1';
		wait for CLK_period;
		assert ACTIVE = '1' report "Chyba u hodnoty ACTIVE!";
		wait for 10 ns;
		INVERT_REQ_IN(IDX_TOP) <= '0';

		INVERT_REQ_IN(IDX_LEFT) <= '1';
		wait for CLK_period;
		assert ACTIVE = '1' report "Chyba u hodnoty ACTIVE!";
		wait for 10 ns;
		INVERT_REQ_IN(IDX_LEFT) <= '0';

		INVERT_REQ_IN(IDX_RIGHT) <= '1';
		wait for CLK_period;
		assert ACTIVE = '1' report "Chyba u hodnoty ACTIVE!";
		wait for 10 ns;
		INVERT_REQ_IN(IDX_RIGHT) <= '0';
		
		INVERT_REQ_IN(IDX_BOTTOM) <= '1';
		wait for CLK_period;
		assert ACTIVE = '1' report "Chyba u hodnoty ACTIVE!";
		wait for 10 ns;
		
		wait;
	end process;
END;
