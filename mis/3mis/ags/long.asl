{include("commonLogic.asl")}
{include("Astar.asl")}
{include("commonMinersLogic.asl")}

gathering([gold,pergamen,wood,shore]).

agentType(long).

needed(gold).
needed(pergamen).

/******************** PLANS ********************/

// Go for the gloves if don't have them.
+!setGoal: moves_per_round(3) & findNearest(shoes, [X, Y]) & X\==999 & Y\==999 <-
	.println("Set goal: reach the shoes ", [X, Y]);
	+goal([X, Y]).
// The bag is full go to depot.
+!setGoal: bag_full & depot(X,Y) <-
	.println("Set goal: reach the depot ", [X, Y]);
	+goal([X, Y]).
// Go for the nearest gathering needed resource.
+!setGoal: not(bag_full) & needed(T) & gathering(GR) & .member(T, GR) & findNearest(T, [TX,TY]) & TX\==999 & TY\==999 <-
	.findall(R, (needed(R) & .member(R, GR)), Rs); ?findNearestTypes(Rs, [X,Y]);
	.println("Set goal: reach the needed resource (choices are ", Rs, ") ", [X, Y]);
	if (anObject([shore, [X,Y]]) & not(bag([null, null]))) {
		.println("Set goal: can not go for water because the bag is not empty go to depot");
		?depot(DX,DY); +goal([DX, DY]);
	}
	else {
		+goal([X, Y]);
	}.
// Go for the nearest gathering resource.
+!setGoal: not(bag_full) & gathering(GR) & findNearestTypes(GR, [X,Y]) & X\==999 & Y\==999 <-
	.println("Set goal: reach the resource ", [X, Y]);
	if (anObject([shore, [X,Y]]) & not(bag([null, null]))) {
		.println("Set goal: can not go for water because the bag is not empty go to depot");
		?depot(DX,DY); +goal([DX, DY]);
	}
	else {
		+goal([X, Y]);
	}.
// Default no goal going to depot.
+!setGoal: depot(X,Y) <-
	.println("Set goal: no goal going to depot");
	+goal([X, Y]).
// Default no goal.
+!setGoal <-
	.println("Set goal: no goal").


// The goal is reached process it.
+!reachGoal: pos(X,Y) & goal([X,Y]) <-
	.println("Reach goal: process the goal");
	-goal(_); -path(_); !processGoal.
// If the path for the goal exits and the next position of the move is accessible do the move.
+!reachGoal: pos(X,Y) & goal(Goal) & path(Path) & last(Path, Goal) & head(Path, NextPos) & isAccessible(NextPos) <-
	.println("Reach goal: move to position ", NextPos, " from ", [X,Y], " reaching the goal ", Goal);
	!move.
// The goal is defined but there is no path, so create it.
+!reachGoal: goal(Goal) <-
	.println("Reach goal: planning path to the goal ", Goal);
	!planRoute;
	if(path(Path)){
		.println("Reach goal: path found ", Path);
		!reachGoal;
	}
	else {
		.println("Reach goal: path not found");
		!planDelObject([_, Goal]); 
		!setAndReachGoal; // !!!
	}.
+!reachGoal <-
	.println("Reach goal: no goal"); do(skip).
	

+!processGoal: pos(X,Y) & depot(X,Y) <-
	+picked;  drop(all).
// Pick up the gloves.
+!processGoal: pos(X,Y) & shoes(X,Y) & not(moves_per_round(6)) <-
	.println("Picking shoes on ", [X,Y]);
	!planDelObject([shoes, [X,Y]]);
	.abolish(shoes(X,Y));
	+picked; do(pick).
// Pick up resources.
+!processGoal: pos(X,Y) & water(X,Y) & not(bag_full) <-
	.println("Picking water on ", [X,Y]);
	+picked; do(pick).
+!processGoal <- !processResourceGoal.

