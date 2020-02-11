/*
 * A* algorithm for searching the shortest path
 */

getFScore(C, S):- fScore(C, S).
getFScore(_, 999).
getGScore(C, S):- gScore(C, S).
getGScore(_, 999).

lowestOpenSetFScoreCell([], _, L, L).
lowestOpenSetFScoreCell([H|T], CM, M, L):- getFScore(H, HS) & HS < CM & lowestOpenSetFScoreCell(T, HS, H, L).
lowestOpenSetFScoreCell([_|T], CM, M, L):- lowestOpenSetFScoreCell(T, CM, M, L).
lowestOpenSetFScoreCell([H|T], L):- getFScore(H, HS) & lowestOpenSetFScoreCell(T, HS, H, L).
lowestOpenSetFScoreCell(L):- .findall(O, openSet(O), List) & lowestOpenSetFScoreCell(List, L). // start

isAccessible([X,Y]):- grid_size(MX, MY) & X >= 0 & X < MX & Y >= 0 & Y < MY & 
                      (anObject([shore, [X,Y]]) | not(anObject([T, [X,Y]]) & (T == water | T == stone))).

+!getNeighbors([X,Y])<-
	if (isAccessible([X+1,Y])) {
		+neighbor([X+1,Y]);
	}
	if (isAccessible([X-1,Y])) {
		+neighbor([X-1,Y]);
	}
	if (isAccessible([X,Y+1])) {
		+neighbor([X,Y+1]); 
	}
	if (isAccessible([X,Y-1])) {
		+neighbor([X,Y-1]);
	}.

// A* finds a path from start to goal.
+!astar(Start, Goal): grid_size(Gx,Gy) <- // boosted version
  -path(_);
  .findall(C, anObject([T, C]) & (T == water | (not ignore & T == stone)), Obstacles); boosted.aStar(Start, Goal, Obstacles, Gx, Gy, P); // obstacles
  if (not .empty(P)) {
    +path(P);
  }.

// Jason implementation
+!astar(_, Goal): not isAccessible(Goal) <- .abolish(path(_)). // goal is not accessible -> can't construct a path
+!astar(Start, Goal) <-
  .abolish(path(_));
  // The set of discovered nodes that may need to be (re-)expanded.
  // Initially, only the start node is known.
	+openSet(Start);
	// For node n, cameFrom[n] is the node immediately preceding it on the cheapest path from start to n currently known.
	// For node n, gScore[n] is the cost of the cheapest path from start to n currently known.
	+gScore(Start, 0);
	// For node n, fScore[n] := gScore[n] + h(n).
	?distance(Start, Goal, Score);
	+fScore(Start, Score);
	while (openSet(_)) {
		// the node in openSet having the lowest fScore[] value
		?lowestOpenSetFScoreCell(Current);
		if (Current == Goal) {
			// reconstruct the path
			+path(Current);
			+current(Current);
			while (current(C) & cameFrom(C, _)) {
				?cameFrom(C, F);
				+path(F);
				-current(_);
				+current(F);
			}
			.findall(C, path(C), Path);
			.abolish(path(_));
			.abolish(openSet(_));
			.abolish(gScore(_,_));
			.abolish(fScore(_,_));
			.abolish(cameFrom(_,_));
			-current(_);
			?tail(Path, ResultPath); +path(ResultPath);
		}
		else {
			// remove current from openSet
			-openSet(Current);
			// for each neighbor of current
			!getNeighbors(Current);
			for (neighbor(N)) {
				//?gScore([Current, GSC]); 
				?getGScore(Current, GSC);
				?getGScore(N, GSN);
				// d(current,neighbor) is the weight of the edge from current to neighbor
				// tentative_gScore is the distance from start to the neighbor through current
				TGS = GSC + 1; // d(current, neighbor) is always 1
				if (TGS < GSN) {
					// This path to neighbor is better than any previous one. Record it!
					//cameFrom[neighbor] := current
					+cameFrom(N, Current);
					+gScore(N, TGS);
					?distance(N, Goal, D);	FScore = TGS + D;
					+fScore(N, FScore);
					+openSet(N);
				}
			}
			.abolish(neighbor(_));
		}
	}.
