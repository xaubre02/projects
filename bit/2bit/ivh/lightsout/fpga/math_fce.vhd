----------------------------
--  Project:  Lights Out  --
----------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.all;

package math_fce is
	function vypocet(col,row : natural; idx: natural) return natural;
end math_fce;

package body math_fce is
	function vypocet(col,row : natural; idx: natural) return natural is
	variable vysledek : natural; 
	begin
		if (col = 0) then
			vysledek := (idx+(((5*(row-1))+4)*4));
		elsif (col = 6) then
			vysledek := (idx+((5*(row-1))*4));
		elsif (row = 0) then
			vysledek := (idx+((20+col-1)*4));
		elsif (row = 6) then
			vysledek := (idx+((col-1)*4));
		else
			vysledek := (idx+((((row-1)*5)+(col-1))*4));
		end if;
		return vysledek;
	end;
end math_fce;
