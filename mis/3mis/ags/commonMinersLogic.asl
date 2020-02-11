
/****** GOALS TO TAKE THE STEP ******/
+step(0): moves_per_round(MPR) & agentType(AgentType) <- 
	!introduceMyself(AgentType);
	!denoteAllCellsUnseen;
	+movesLeft(MPR); !act.
+step(X): moves_per_round(MPR) <- 
	.wait(done, 100);
	for (deleteObject(O)) { -deleteObject(O); !tellFriends(achieve, removeObject(O)); }
	-done; -picked; if (X mod 30 == 0) {.abolish(unaccessable(_));}
	.abolish(movesLeft(_)); +movesLeft(MPR); !act.


+!act: picked | movesLeft(0) <- 
	.print("No more moves end of step."); +done.
+!act: movesLeft(Moves) & Moves > 0 & not(picked) <-
	!perceive; 
	!setAndReachGoal;
	.abolish(movesLeft(_)); +movesLeft(Moves-1); !act.
	

// Try again set and reach a goa.
+!setAndReachGoal <-
	-goal(_); !setGoal; !reachGoal.
	

// Deletes localy the object and plan it to delete in friend agents in next step.
+!planDelObject(O) <-
	.abolish(anObject(O)); +deleteObject(O).
	

/****** GOALS TO MOVE AND PLAN ROUTE ******/
+!move: pos(X,Y) & path([[X,Y]|T]) <- -path(_); +path(T); !move.
+!move: pos(X,Y) & path(P) & head(P,PH) & getMove([X,Y],PH,M) <-
  if (M \== skip) {
    .abolish(path(_));
    if (tail(P,PT) & not .empty(PT)) {
      +path(PT);
    }
  } do(M).
+!move <- do(skip).


// Plan a new route to the goal	
+!planRoute: goal(G) & pos(X,Y) <- !astar([X,Y], G).


/******* GOALS TO PICK UP COMMON RESOURCES *******/
// Pick up resources.
+!processResourceGoal: pos(X,Y) & wood(X,Y) & not(bag_full) <-
	.println("Picking wood on ", [X,Y]);
	!planDelObject([wood, [X,Y]]);
	.abolish(wood(X,Y));
	+picked; do(pick).
+!processResourceGoal: pos(X,Y) & pergamen(X,Y) & not(bag_full) <-
	.println("Picking pergamen on ", [X,Y]);
	!planDelObject([pergamen, [X,Y]]);
	.abolish(pergamen(X,Y));
	+picked; do(pick).
+!processResourceGoal: pos(X,Y) & gold(X,Y) & not(bag_full) <-
	.println("Picking gold on ", [X,Y]);
	!planDelObject([gold, [X,Y]]);
	.abolish(gold(X,Y));
	+picked; do(pick).	
// Remove the object when he is not there anymore.
+!processResourceGoal: pos(X,Y) & anObject([R, [X,Y]])<-
	.println("Picking (removing) ", R, " on ", [X,Y]);
	!planDelObject([R, [X,Y]]);
	!setAndReachGoal.
// Default process of goal.
+!processResourceGoal <-
	!setAndReachGoal.
	
