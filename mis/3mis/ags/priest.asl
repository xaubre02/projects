{include("commonLogic.asl")}

/* Initial beliefs and rules */

isSpellApplicable(St, Wt, Wd, G) :- 
	(depot(stone,DSt) & DSt>=St) & 
	(depot(water,DWt) & DWt>=Wt) & 
	(depot(wood,DWd) & DWd>=Wd) & 
	(depot(gold,DG) & DG>=G).   

getTotalCountResource(R, MapSts + DSts) :-
	.count(anObject([R,_]), MapSts) & depot(R, DSts). 

isSpellPossibleInFeature(St, Wt, Wd, G) :-
	getTotalCountResource(stone, Sts) & Sts >= St &
	getTotalCountResource(wood, Wds) & Wds >= Wd &
	getTotalCountResource(gold, Gs) & Gs >= G.

isSpell5(A,B,C,D) :-
	(A+B+C+D == 5).
isSpell4(A,B,C,D) :-
	(A+B+C+D == 4).
isSpell3(A,B,C,D) :-
	(A+B+C+D == 3).

isSpell4OrBigger(A,B,C,D) :-
	(A+B+C+D >= 4).
isSpell3OrBigger(A,B,C,D) :-
	(A+B+C+D >= 3).
	
hasSpell5 :-
	spell(A,B,C,D) &
	isSpell5(A,B,C,D).
hasSpell4OrBigger :-
	spell(A,B,C,D) &
	isSpell4OrBigger(A,B,C,D).
hasSpell3OrBigger :-
	spell(A,B,C,D) &
	isSpell3OrBigger(A,B,C,D).
	
teamsMoney(Us,MoneyUs,T2,M2,T3,M3,T4,M4) :-
	team(Us) & money(Us,MoneyUs) & 
	money(T2,M2) & not(Us==T2) & 
	money(T3,M3) & not(Us==T3 | T2==T3) & 
	money(T4,M4) & not(Us==T4 | T2==T4 | T3==T4).

neededCoalition :-
	teamsMoney(Us,MoneyUs,T2,M2,T3,M3,T4,M4) &
	MoneyUs<92 &
	(M2+M3>100 | M2+M4>100 | M3+M4>100).

// M1 - our money, M2 - money of enemy, Third is good minimal limit(that we would accepted)
goodProposal(M1, M2, 54) :- 
	M1>M2 & M1-M2>20.
goodProposal(M1, M2, 53) :- 
	M1>M2 & M1-M2>15.
goodProposal(M1, M2, 52) :- 
	M1>M2 & M1-M2>10.
goodProposal(M1, M2, 51) :- 
	M1>M2 & M1-M2>5.
goodProposal(M1, M2, 46) :- 
	M2>M1 & M2-M1>20.
goodProposal(M1, M2, 47) :- 
	M2>M1 & M2-M1>15.
goodProposal(M1, M2, 48) :- 
	M2>M1 & M2-M1>10.
goodProposal(M1, M2, 49) :- 
	M2>M1 & M2-M1>5. 	
goodProposal(M1, M2, 50).


/* Initial goals */
!start.

+step(0) <- 
	.println("Our priest agent starts!!!");
	do(skip).
+step(X) <- 
	!step(X).


/* PLANS */
/* Pergamens logic after step 50 */
+!step(X): X>50 & depot(pergamen,Y) & Y>4 <-
	// always read 5 pergamens
	do(read,5).
+!step(X): X>50 & depot(pergamen,Y) & Y>3 & ((getTotalCountResource(pergamen, Ps) & not(Ps>4)) | X>210) <-
	// read 4 pergamens always after step 210(or when formula 5 cannot be created)
	do(read,4).
+!step(X): X>50 & depot(pergamen,Y) & Y>2 & ((getTotalCountResource(pergamen, Ps) & not(Ps>3)) | X>210) <-
	// read 3 pergamens always after step 210(or when formulas 5,4 cannot be created)
	do(read,3).

/* Resources logic */
+!step(X): X>140 <-
	!tellBroadThatNeeded;
	!tellLongThatNeeded;
	!tellSharpSightThatNeeded;
	!checkProposals;
	if(not(hasBeenProposed(_)) | (hasBeenProposed(_) & (X mod 5 == 0))) {
		!makeProposal;
	}
	!trySpells.
+!step(X): X>50 <-
	!tellBroadThatNeeded;
	!trySpells.
+!step(X) <-
	do(skip).
	
/* Broad needed resource logic */
+!tellBroadThatNeeded: depot(stone,St) & depot(wood,Wd) & (Wd > 2*St) & (getTotalCountResource(stone, Sts) & Sts>0) <-                                                          
	!tellBroad(tell, needed(stone));
	!tellBroad(untell, needed(wood));
	!tellBroad(untell, needed(shore)).	
+!tellBroadThatNeeded: depot(stone,St) & depot(wood,Wd) & (St > 2*Wd) & (getTotalCountResource(wood, Wds) & Wds>0) <-
	!tellBroad(untell, needed(stone));
	!tellBroad(tell, needed(wood));
	!tellBroad(untell, needed(shore)).
+!tellBroadThatNeeded <-
	!tellBroad(untell, needed(stone));
	!tellBroad(untell, needed(wood));
	!tellBroad(untell, needed(shore)).	

/* Long needed resource logic */
+!tellLongThatNeeded: depot(wood,Wd) & Wd<5 & (getTotalCountResource(wood, Wds) & Wds>0) <-
	!tellLong(untell, needed(stone));
	!tellLong(tell, needed(wood));
	!tellLong(untell, needed(shore)).
+!tellLongThatNeeded: depot(water,Wt) & Wt<5 <-
	!tellLong(untell, needed(stone));
	!tellLong(untell, needed(wood));
	!tellLong(tell, needed(shore)).
+!tellLongThatNeeded <-
	!tellLong(untell, needed(stone));
	!tellLong(untell, needed(wood));
	!tellLong(untell, needed(shore)).
	
/* Fix - Long will not go for stone anytime
+!tellLongThatNeeded: depot(stone,St) & St<5 & (getTotalCountResource(stone, Sts) & Sts>0) <-
	!tellLong(tell, needed(stone));                                                                                              
	!tellLong(untell, needed(wood));
	!tellLong(untell, needed(shore)).*/

/* SharpSight needed resource logic */
+!tellSharpSightThatNeeded: depot(water,Wt) & Wt<8 <-
	!tellSharpSight(untell, needed(stone));
	!tellSharpSight(untell, needed(wood));
	!tellSharpSight(tell, needed(shore)).
+!tellSharpSightThatNeeded: depot(wood,Wd) & Wd<8 & (getTotalCountResource(wood, Wds) & Wds > 0) <-
	!tellSharpSight(untell, needed(stone));
	!tellSharpSight(tell, needed(wood));
	!tellSharpSight(untell, needed(shore)).
+!tellSharpSightThatNeeded: depot(stone,St) & St<8 & (getTotalCountResource(stone, Sts) & Sts > 0) <-
	!tellSharpSight(tell, needed(stone));
	!tellSharpSight(untell, needed(wood));
	!tellSharpSight(untell, needed(shore)).
+!tellSharpSightThatNeeded <-
	!tellSharpSight(untell, needed(stone));
	!tellSharpSight(untell, needed(wood));
	!tellSharpSight(untell, needed(shore)).

	
/* Spells logic */
+!trySpells: team(Us) & money(Us, M) & M>85 <-
	!trySpellsWinIsClose.
+!trySpells <-
	!trySpell.

+!trySpellsWinIsClose: spell(A,B,C,D) & isSpell5(A,B,C,D) & isSpellApplicable(A,B,C,D) <-
	do(create,A,B,C,D).
+!trySpellsWinIsClose: spell(A,B,C,D) & isSpell4(A,B,C,D) & isSpellApplicable(A,B,C,D) <-
	do(create,A,B,C,D).
+!trySpellsWinIsClose: spell(A,B,C,D) & isSpell3(A,B,C,D) & isSpellApplicable(A,B,C,D) <-
	do(create,A,B,C,D).
+!trySpellsWinIsClose: spell(A,B,C,D) & isSpellApplicable(A,B,C,D) <-
	do(create,A,B,C,D).
+!trySpellsWinIsClose <-
	do(skip).

+!trySpell: spell(A,B,C,D) & isSpell5(A,B,C,D) & isSpellApplicable(A,B,C,D) <- 
	do(create,A,B,C,D).
+!trySpell: spell(A,B,C,D) & isSpell4(A,B,C,D) & isSpellApplicable(A,B,C,D) & (not(hasSpell5) | (spell(E,F,G,H) & isSpell5(E,F,G,H) & not(isSpellPossibleInFeature(E,F,G,H)))) <- 
	do(create,A,B,C,D).
+!trySpell: spell(A,B,C,D) & isSpell3(A,B,C,D) & isSpellApplicable(A,B,C,D) & (not(hasSpell4OrBigger) | (spell(E,F,G,H) & isSpell4OrBigger(E,F,G,H) & not(isSpellPossibleInFeature(E,F,G,H)))) <- 
	do(create,A,B,C,D).
+!trySpell: spell(A,B,C,D) & isSpellApplicable(A,B,C,D) & (not(hasSpell3OrBigger) | (spell(E,F,G,H) & isSpell3OrBigger(E,F,G,H) & not(isSpellPossibleInFeature(E,F,G,H)))) <-
	do(create,A,B,C,D).
+!trySpell <-
	do(skip).

	
+!makeProposal: neededCoalition & team(Us) & money(Us, M1) & money(T, M2) & not(Us==T) & not(hasBeenProposed(T)) & M1+M2>100 & goodProposal(M1,M2,Limit) <-
	if(hasBeenProposed(_)) {
		-hasBeenProposed(_);	// remove previous proposal
	}
	
	if(Us==a) {
		if(T==b) {
			+hasBeenProposed(b);
			do(propose,Limit,100-Limit,0,0);
		}
		if(T==c) {
			+hasBeenProposed(c);
			do(propose,Limit,0,100-Limit,0);
		}
		if(T==d) {
			+hasBeenProposed(d);
			do(propose,Limit,0,0,100-Limit);
		}
	}
	if(Us==b) {
		if(T==a) {
			+hasBeenProposed(a);
			do(propose,100-Limit,Limit,0,0);
		}
		if(T==c) {
			+hasBeenProposed(c);
			do(propose,0,Limit,100-Limit,0);
		}
		if(T==d) {
			+hasBeenProposed(d);
			do(propose,0,Limit,0,100-Limit);
		}
	}
	if(Us==c) {
		if(T==a) {
			+hasBeenProposed(a);
			do(propose,100-Limit,0,Limit,0);
		}
		if(T==b) {
			+hasBeenProposed(b);
			do(propose,0,100-Limit,Limit,0);
		}
		if(T==d) {
			+hasBeenProposed(d);
			do(propose,0,0,Limit,100-Limit);
		}
	};
	if(Us==d) {
		if(T==a) {
			+hasBeenProposed(a);
			do(propose,100-Limit,0,0,Limit);
		}
		if(T==b) {
			+hasBeenProposed(b);
			do(propose,0,100-Limit,0,Limit);
		}
		if(T==c) {
			+hasBeenProposed(d);
			do(propose,0,0,100-Limit,Limit);
		}
	}.
	/* D is not proposing TODO */
+!makeProposal <-
	.println("We dont propose any coallition.").

	
+!checkProposals: neededCoalition & team(Us) & money(Us, M1) & money(T, M2) & not(Us==T) & M1+M2>100 & coalitionProposal(T,a,S1,b,S2,c,S3,d,S4) & goodProposal(M1,M2,Limit) <-
	if(Us==a) {
		if(S1>=Limit) {
			.println("We are gonna win!!! We accepted coalition with team: "); 
			.println(T);
			do(accept,T);
		}
	}
	if(Us==b) {
		if(S2>=Limit) { 
			.println("We are gonna win!!! We accepted coalition with team: "); 
			.println(T);
			do(accept,T); 
		}
	}
	if(Us==c) { 
		if(S3>=Limit) { 
			.println("We are gonna win!!! We accepted coalition with team: "); 
			.println(T);
			do(accept,T); 
		}
	};
	if(Us==d) { 
		if(S4>=Limit) { 
			.println("We are gonna win!!! We accepted coalition with team: "); 
			.println(T);
			do(accept,T); 
		}
	}.
	/* TODO d is not accepting */
+!checkProposals <-
	.println("We dont accept any coallition.").

	
	
+!start.


