{include("commonLogic.asl")}
{include("Astar.asl")}
{include("commonMinersLogic.asl")}


isNeighbor([X,Y]):- pos(X-1,Y).
isNeighbor([X,Y]):- pos(X+1,Y).
isNeighbor([X,Y]):- pos(X,Y-1).
isNeighbor([X,Y]):- pos(X,Y+1).


// Defines the direction to dig the stone.
stoneInScope(-1,0,e).
stoneInScope(1,0,w).
stoneInScope(0,1,n).
stoneInScope(0,-1,s).

gathering([stone,wood]).

agentType(broad).

/******************** PLANS ********************/

// Go for the gloves if don't have them.
+!setGoal: carrying_capacity(4) & findNearest(gloves, [X, Y]) & X\==999 & Y\==999 <-
	.println("Set goal: reach the gloves ", [X, Y]);
	+goal([X, Y]).
// The bag is full go to depot.
+!setGoal: bag_full & depot(X,Y) <-
	.println("Set goal: reach the depot ", [X, Y]);
	+goal([X, Y]).
// Go for the nearest gathering needed resource.
+!setGoal: not(bag_full) & needed(T) & gathering(GR) & .member(T, GR) & findNearest(T, [TX,TY]) & TX\==999 & TY\==999 <-
	.findall(R, (needed(R) & .member(R, GR)), Rs); ?findNearestTypes(Rs, [X,Y]);
	.println("Set goal: reach the needed resource (choices are ", Rs, ") ", [X, Y]);
	+goal([X, Y]).
// Go for the nearest gathering resource.
+!setGoal: not(bag_full) & gathering(GR) & findNearestTypes(GR, [X,Y]) & X\==999 & Y\==999 <-
	.println("Set goal: reach the resource ", [X, Y]);
	+goal([X, Y]).
// Default no goal going to depot.
+!setGoal: depot(X,Y) <-
	.println("Set goal: no goal going to depot");
	+goal([X, Y]).
// Default no goal.
+!setGoal <-
	.println("Set goal: no goal").


// The goal is stone and we are next to it so process it.
+!reachGoal: goal([X,Y]) & stone(X,Y) & isNeighbor([X,Y]) <-
	.println("Reach goal: process the goal (stone)");
	-goal(_); -path(_); !processGoal.
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
	if (not(isAccessible(Goal))) {
		!getNeighbors(Goal); 
		if (neighbor(NextToGoal)) {
			.abolish(neighbor(_)); -goal(Goal); +goal(NextToGoal);
		}
		else {
			.println("Reach goal: path not found");
			!planDelObject([_, Goal]);
			!setAndReachGoal; // !!!
		}
	}
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
	+picked; drop(all).
// Pick up the gloves.
+!processGoal: pos(X,Y) & gloves(X,Y) & not(carrying_capacity(8)) <-
	.println("Picking gloves on ", [X,Y]);
	!planDelObject([gloves, [X,Y]]);
	.abolish(gloves(X,Y));
	+picked; do(pick).
// Pick up resources.
+!processGoal: pos(X,Y) & stone(SX, SY) & stoneInScope(X-SX, Y-SY, Direction) & not(bag_full) <-
	.println("Picking stone on ", [SX,SY]);
	!planDelObject([stone, [SX,SY]]);
	.abolish(stone(SX,SY));
	+picked; do(dig, Direction).
+!processGoal <- !processResourceGoal.
	
