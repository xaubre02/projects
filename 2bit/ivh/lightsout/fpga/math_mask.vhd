----------------------------
--  Project:  Lights Out  --
----------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.all;

package math_mask is

	type mask_t is 
	record
		top, left, right, bottom : std_logic;
	end record;

	function getmask(x,y: natural; COLUMNS, ROWS: natural) return mask_t;

end math_mask;

package body math_mask is

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

end math_mask;
