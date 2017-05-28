--
--	Project:	Lights Out
--	Author:	Tomáš Aubrecht 
--	Description:	Implementation of a mask finding neighbours of a cell.
--

library IEEE;
use IEEE.STD_LOGIC_1164.all;

package math_pack is

	type mask_t is 
	record
		top, left, right, bottom : std_logic;
	end record;

	function getmask(x,y: natural; COLUMNS, ROWS: natural) return mask_t;

end math_pack;

package body math_pack is

	function getmask(x,y: natural; COLUMNS, ROWS: natural) return mask_t is
	variable mask : mask_t;
	begin
		if (y = 0) then 
			mask.top := '0';
		else
			mask.top := '1';
		end if;

		if (x = 0) then 
			mask.left := '0';
		else
			mask.left := '1';
		end if;

		if (x = COLUMNS - 1) then 
			mask.right := '0';
		else
			mask.right := '1';
		end if;

		if (y = ROWS - 1) then 
			mask.bottom := '0';
		else
			mask.bottom := '1';
		end if;
		return mask;
	end;

end math_pack;
