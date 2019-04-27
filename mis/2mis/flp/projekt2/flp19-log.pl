/** ************************
 *  Předmět: FLP 2018/2019
 *  Projekt: Turingův stroj
 *  Datum:   2019-04-24
 *  Autor:   Tomáš Aubrecht
 *  Login:   xaubre02
 ** ************************/

%%%%%%%%%%%%%%% Funkce převzaté z input2.pl %%%%%%%%%%%%%%%

read_line(L,C) :-
	get_char(C),
	(isEOFEOL(C), L = [], !;
		read_line(LL,_),% atom_codes(C,[Cd]),
		[C|LL] = L).


% testuje znak na EOF nebo LF
isEOFEOL(C) :-
	C == end_of_file;
	(char_code(C,Code), Code==10).


read_lines(Ls) :-
	read_line(L,C),
	( C == end_of_file, Ls = [] ;
	  read_lines(LLs), Ls = [L|LLs]
	).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%% Funkce pro zpracování vstupu %%%%%%%%%%%%%%

% odstranění posledního prvku seznamu
remove_last([_], []).
remove_last([H|T], [H|Rest]) :- remove_last(T, Rest).


% kontrola, zdali se jedná o platný stav TS (velké písmeno)
is_state(C) :-
	is_alpha(C),
	is_upper(C).
is_state(C) :-
	% neplatný stav Turingova stroje
	write('Neplatný stav Turingova stroje: '),
	writeln(C),
	halt(1).


% kontrola, zdali se jedná o platný symbol pásky TS (mezera nebo malé písmeno)
is_symbol(' ').
is_symbol(C) :-
	is_alpha(C),
  is_lower(C).
is_symbol(C) :-
	% neplatný symbol Turingova stroje
	write('Neplatný symbol Turingova stroje: '),
	writeln(C),
	halt(1).


% kontrola, zdali se jedná o platnou akci TS (posuv nebo symbol)
is_action('L').
is_action('R').
is_action(C) :- is_symbol(C).
is_action(C) :-
	% neplatná akce Turingova stroje
	write('Neplatná akce Turingova stroje: '),
	writeln(C),
	halt(1).


% kontrola, zdali se jedná o platnou vstupní pásku TS (pouze symboly)
is_valid_tape([]).
is_valid_tape([H|T]) :-
	is_symbol(H),
	is_valid_tape(T).


% parsování pravidel
parse_rules([], []).
parse_rules([[S, ' ', Z, ' ', N, ' ', A]|T], Rest) :- % Pravidla jsou zadána ve tvaru <stav> <znak na pásce> <nový stav> <nový znak na pásce nebo „L“, „R“>. Jednotlivé části jsou odděleny mezerou.
	is_state(S),  % musí být platný stav
	is_symbol(Z), % musí být platný symbol
	is_state(N),  % musí být platný stav
	is_action(A), % musí být platná akce
	Rule = [S, Z, N, A],
	parse_rules(T, R),
	Rest = [Rule|R].
parse_rules(_, _) :-
	% neplatný formát pravidla
	writeln('Neplatné pravidlo na vstupu!'),
	halt(1).


% parsování vstupu
parse_input(Vstup, Pravidla, Paska) :-
	% zpracování pravidel
	remove_last(Vstup, V),
	parse_rules(V, Pravidla),

	% získání pásky, její kontrola a inicializace počáteční konfigurace -> přidání počátečního stavu 'S' na začátek pásky
	last(Vstup, P),
  is_valid_tape(P),
	append(['S'], P, Paska).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%% Funkce pro simulaci TS %%%%%%%%%%%%%%%%%

% získání aktuálního symbolu z pásky
get_cur_symbol([], ' ').  % nekonečná posloupnost prázdných symbolů
get_cur_symbol([H|_], H).


% získání aktuální konfigurace
get_cur_config([H|T], Stav, Symbol) :-
	% stav je velké písmeno
	is_alpha(H),
	is_upper(H),
	(
		Stav=H,
		get_cur_symbol(T, Symbol)
	);

	% je potřeba hledat dále
	get_cur_config(T, Stav, Symbol).


% nalezni pravidlo pro přechod do koncového stavu z aktuální konfigurace
find_rule_final(_, _, [], _, Akce) :- Akce='X'. % nelze přejít do koncového stavu z aktuální konfigurace
find_rule_final(Stav, Symbol, [[S, Z, N, A]|Pravidla], NovyStav, Akce) :-
	N == 'F',
	Stav == S,
	Symbol == Z,
	% pravidlo nalezelo
	(
		NovyStav = N,
		Akce = A
	);
	% hledej dále
	find_rule_final(Stav, Symbol, Pravidla, NovyStav, Akce).


% nalezení pravidla, které lze aplikovat nebo nastav chybu
find_rule_any(_, _, [], _, Akce) :- Akce='X'. % abnormální zastavení (není definován přechod z aktuální konfigurace)
find_rule_any(Stav, Symbol, [[S, Z, N, A]|Pravidla], NovyStav, Akce) :-
	Stav == S,
	Symbol == Z,
	% pravidlo nalezelo
	(
		NovyStav = N,
		Akce = A
	);
	% hledej dále
	find_rule_any(Stav, Symbol, Pravidla, NovyStav, Akce).


% hledání aplikovatelných pravidel
find_rule(Stav, Symbol, Pravidla, NovyStav, Akce) :-
	% zjisti, zdali se dá přejít do koncového stavu
	find_rule_final(Stav, Symbol, Pravidla, NovyStav, Akce),
	% lze přejít do koncového stavu
	Akce \= 'X';

	% nelze přejít do koncového stavu -> nalezni jakékoliv pravidlo, které se dá aplikovat
	find_rule_any(Stav, Symbol, Pravidla, NovyStav, Akce).


% aktuální posun čtecí hlavy vlevo
shift_left_aux([H|[HT|TT]], Stav, NovyStav, NovaPaska) :-
	Stav == HT,
	(
		% vymění se nový stav s aktuálním znakem v H (začátek pásky) a zkopíruje se zbytek pásky
		append([NovyStav], [H], NP),
		append(NP, TT, NovaPaska)
	);

	% hledej dál
	shift_left_aux([HT|TT], Stav, NovyStav, NP2),
	NovaPaska = [H|NP2].


% abnormální zastavení nebo zahájení posunu čtecí hlavy vlevo
shif_left([H|T], Stav, NovyStav, NovaPaska) :-
	% dojde k přepadnutí čtecí hlavy - abnormální zastavení
	H == Stav, NovaPaska='X' ;

	% proveď posun
	shift_left_aux([H|T], Stav, NovyStav, NovaPaska).


% posun čtecí hlavy vpravo
shif_right([H|T], Stav, NovyStav, NovaPaska) :-
	Stav == H,
	(
		% konec pásky (následují prázdné symboly)
		T == [],
		(
    	% přidání prázdného symbolu
    	append([' '], [NovyStav], NovaPaska)
		);

		% uloží se hlava zbytku, následně nový stav a zkopíruje se zbytek pásky
		[HT|TT] = T,
		append([HT], [NovyStav], NP),
		append(NP, TT, NovaPaska)
	);
  
	% hledej dál
	shif_right(T, Stav, NovyStav, NP2),
	NovaPaska = [H|NP2].


% přepsání symbolu aktuálně se nacházejícího pod čtecí hlavou
write_symbol([H|T], Stav, NovyStav, NovySymbol, NovaPaska) :-
	Stav == H,
	(
		% konec pásky (následují prázdné symboly)
		T == [],
		(
    	% přidání symbolu za "konec" aktuální pásky
    	append([NovyStav], [NovySymbol], NovaPaska)
		);

		% vymění se nový stav s aktuálním znakem v H a zkopíruje se zbytek pásky
		[_|TT] = T,
		append([NovyStav], [NovySymbol], NP),
		append(NP, TT, NovaPaska)
	);

	% hledej dál
	write_symbol(T, Stav, NovyStav, NovySymbol, NP2),
	NovaPaska = [H|NP2].


% provede simulaci Turingova stroje
simulate(Paska, Pravidla, Konfigurace) :-
	% získání aktuálního stavu TS a symbolu pod čtecí hlavou
	get_cur_config(Paska, Stav, Symbol),
	(
		% koncový stav
		Stav == 'F';

		% nalezení pravidla, které lze aplikovat
		find_rule(Stav, Symbol, Pravidla, NovyStav, Akce),
		(
			Akce == 'X'; % abnormální zastavení
			Akce == 'L', shif_left(Paska, Stav, NovyStav, NovaPaska) ; % posun vlevo
			Akce == 'R', shif_right(Paska, Stav, NovyStav, NovaPaska) ; % posun vpravo
			write_symbol(Paska, Stav, NovyStav, Akce, NovaPaska) % zapsání symbolu
		),
		(
			% došlo k abnormálnímu zastavení
			Akce == 'X', writeln('Abnormální zastavení! Nebyl nalezen přechod.');
			NovaPaska == 'X', writeln('Abnormální zastavení! Přepadla čtecí hlava.');

			% pokračování v simulaci
			simulate(NovaPaska, Pravidla, NoveKonfigurace),

			% uložení nové konfigurace do seznamu konfigurací
			Konfigurace = [NovaPaska|NoveKonfigurace]
		)
	).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%% Funkce pro výpis konfigurací TS %%%%%%%%%%%%%

% vypíše jednu konfiguraci TS
write_config([]) :- writeln('').
write_config([H|T]) :-
	write(H),
	write_config(T).


% vypíše seznam konfigurací TS
write_configs([]).
write_configs([H|T]) :-
	write_config(H),
	write_configs(T).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%% Vstupní bod programu %%%%%%%%%%%%%%%%%%

start :-
	prompt(_, ''),

	% načtení vstupu
	read_lines(Vstup),

	% parsování vstupu
	parse_input(Vstup, Pravidla, Paska),

	% spuštění simulace
	simulate(Paska, Pravidla, Konfigurace),

	% vypíše počáteční konfiguraci NTS a následně posloupnost konfigurací
	write_config(Paska),
	write_configs(Konfigurace),

	% ukončení programu
	halt.
