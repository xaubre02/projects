{include("commonLogic.asl")}
{include("Astar.asl")}
{include("commonMinersLogic.asl")}

gathering([gold,pergamen,wood,shore]).

agentType(sharpSight).

spectaclesneeded.
scout.

/******************** PLANS ********************/

// Go for the spectacles if don't have them.
+!setGoal: spectaclesneeded & findNearest(spectacles, [X, Y]) & X\==999 & Y\==999 & not(unaccessable([X, Y])) <-
	.println("Set goal: reach the spectacles ", [X, Y]);
	+goal([X, Y]).
// Explorgin the map.
+!setGoal: scout <-
	!nearestNextUnseenCell(N);
	if (N \== [999,999]) {
		.println("Set goal: reach the unseen cell ", N);
		+goal(N);
	}
	else { 
		-scout; +gather; !setGoal;
	}.
// The bag is full go to depot.
+!setGoal: gather & bag_full & depot(X,Y) <-
	.println("Set goal: reach the depot ", [X, Y]);
	+goal([X, Y]).
// Go for the nearest gathering needed resource.
+!setGoal: gather & not(bag_full) & needed(T) & gathering(GR) & .member(T, GR) & findNearest(T, [TX,TY]) & TX\==999 & TY\==999 <-
	.findall(R, (needed(R) & .member(R, GR)), Rs); ?findNearestTypes(Rs, [X,Y]);
	.println("Set goal: reach the needed resource (choices are ", Rs, ") ", [X, Y]);
	+goal([X, Y]).
// Go for the nearest gathering resource.
+!setGoal: gather & not(bag_full) & gathering(GR) & findNearestTypes(GR, [X,Y]) & X\==999 & Y\==999 <-
	.println("Set goal: reach the resource ", [X, Y]);
	+goal([X, Y]).
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
		if (scout) {
			-aUnseen(Goal); !tellFriends(achieve, removeUnseen(Goal));
			!setAndReachGoal; // !!!
		}
		else {
			!planDelObject([_, Goal]); +unaccessable(Goal);
			!setAndReachGoal; // !!!
		}
	}.
+!reachGoal <-
	.println("Reach goal: no goal"); do(skip).
	

+!processGoal: pos(X,Y) & depot(X,Y) <-
	+picked;  drop(all).
// Already have spectacles.
+!processGoal: spectaclesneeded & visibility(6) <- -spectaclesneeded; !processGoal.
// Pick up the spectacles.
+!processGoal: spectaclesneeded & pos(X,Y) & spectacles(X,Y) <-
	.println("Picking spectacles on ", [X,Y]);
	!planDelObject([spectacles, [X,Y]]);
	.abolish(spectacles(X,Y));
	+picked; do(pick).
// Pick up resources.
+!processGoal: pos(X,Y) & water(X,Y) & not(bag_full) <-
	.println("Picking water on ", [X,Y]);
	+picked; do(pick).
+!processGoal <- !processResourceGoal.

