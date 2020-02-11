/*
 * Common Logic for all Agents
 */

// Coordinates
head([H|_], H).
tail([_|T], T).
last([H], H).
last([_|T], Last) :- last(T, Last).
getX([X,_], X).
getY([_,Y], Y).

// Movement
getMove([Ax,_], [Bx,_], right):- Ax < Bx.
getMove([Ax,_], [Bx,_], left):-  Ax > Bx.
getMove([_,Ay], [_,By], down):-  Ay < By.
getMove([_,Ay], [_,By], up):-    Ay > By.
getMove(_, _, skip).

// distance between two points
distance([X1, Y1], [X2, Y2], math.abs(X2 - X1) + math.abs(Y2 - Y1)).

// nearest point from a list to the current position
nearestFrom(_, [], N, N).
nearestFrom(F, [H|T], M, N):- distance(F, H, DH) & distance(F, M, DM) & DH <= DM & nearestFrom(F, T, H, N).
nearestFrom(F, [_|T], M, N):- nearestFrom(F, T, M, N).
nearestFrom(F, List, N):- nearestFrom(F, List, [999,999], N).
nearest(List, N):- pos(X,Y) & nearestFrom([X,Y], List, [999,999], N). // default position is the current position

// nearest cell that hasn't been seen yet
+!nearestUnseen(P, NU, NUD) <-
  for (aUnseen(U)) {
    ?distance(P,U,D);
    if (minimum(_,MD)) {
      if (D < MD) {
        -minimum(_,_);
        +minimum(U,D);
      }
    }
    // first item
    else {
      +minimum(U,D);
    }
  }
  if (minimum(NU, NUD)) {
    -minimum(_,_);
  }
  else {
    NU = [999,999];
    NUD = -1;
  }.

+!nearestUnseenSet(_, []): not aUnseen(_). // every cell has been seen
+!nearestUnseenSet(P, US) <-
  !nearestUnseen(P, _, D);
  for (aUnseen(U)) {
    ?distance(P,U,UD);
    if (UD == D) {
      +nstUnseen(U);
    }
  }
  .findall(U, nstUnseen(U), US);
  .abolish(nstUnseen(_)).


+!nearestNextUnseenCell(NUC): pos(A,B) & depot(C,D) <- .findall(U, aUnseen(U), Unseen); boosted.findNearestUnseen([A,B], [C,D], Unseen, NUC). // boosted version
+!nearestNextUnseenCell([999,999]): not aUnseen(_). // every cell has been seen
+!nearestNextUnseenCell(NUC): pos(A,B) & depot(C,D) <-
  !nearestUnseenSet([A,B], NUS);
  ?nearestFrom([C,D], NUS, NUC).


// find nearest resource or item
findNearest(O):- .findall([X,Y], anObject([_, [X,Y]]), L) & nearest(L, O).

// find nearest specified resource or item
findNearest(T, O):- .findall([X,Y], anObject([T, [X,Y]]), L) & nearest(L, O).

// find nearest resource specified by list of types
findNearestTypes(LT, O):- .findall([X,Y], (anObject([T, [X,Y]]) & .member(T, LT)), L) & nearest(L, O).

// check whether there is solid ground next to this cell
isSolidGround([X,Y], [Xl,Xu], [Yl,Yu]):-
  grid_size(Mx, My) & not (X < 0 | Y < 0 | X >= Mx | Y >= My) &
  X >= Xl & X <= Xu & Y >= Yl & Y <= Yu & // lower and upper limit
  not (water(X,Y) | anObject([T, [X,Y]]) & (T == water | T == shore)).
isOnTheShore([X,Y], Lx, Ly):-
  isSolidGround([X+1,Y], Lx, Ly) |
  isSolidGround([X-1,Y], Lx, Ly) |
  isSolidGround([X,Y+1], Lx, Ly) |
  isSolidGround([X,Y-1], Lx, Ly) |
  isSolidGround([X+1,Y+1], Lx, Ly) |
  isSolidGround([X-1,Y+1], Lx, Ly) |
  isSolidGround([X+1,Y-1], Lx, Ly) |
  isSolidGround([X-1,Y-1], Lx, Ly).

// names
+!introduceMyself(T): .my_name(N) <- !tellFriends(tell, partner(T, N)).

// the gaol has to be called in the step 0 to init all cells as unseen
+!denoteAllCellsUnseen: grid_size(MaxX, MaxY) <-
	for(.range(X, 0, MaxX-1)) {
	for(.range(Y, 0, MaxY-1)) {
			+aUnseen([X, Y]);
		}
	}.

// send a message to all friendly agents
+!tellFriends(Type, Msg) <- 
  for(friend(F)){
    .send(F, Type, Msg);
  }.

// send a message to specific agent
+!tellLong(Type, Msg):       partner(long, N)       <- .send(N, Type, Msg).
+!tellBroad(Type, Msg):      partner(broad, N)      <- .send(N, Type, Msg).
+!tellSharpSight(Type, Msg): partner(sharpSight, N) <- .send(N, Type, Msg).

// knowledge removal
+!removeObject(O) <- .abolish(anObject(O)).
+!removeUnseen(U) <- .abolish(aUnseen(U)).

// perceive the surroundings
+!perceive: pos(A,B) & visibility(V) <-
  .findall(  [pergamen, [X,Y]],   pergamen(X, Y), L1);
  .findall(     [stone, [X,Y]],      stone(X, Y), L2);
  .findall(      [wood, [X,Y]],       wood(X, Y), L3);
  .findall(      [gold, [X,Y]],       gold(X, Y), L4);
  .findall([spectacles, [X,Y]], spectacles(X, Y), L5);
  .findall(    [gloves, [X,Y]],     gloves(X, Y), L6);
  .findall(     [shoes, [X,Y]],      shoes(X, Y), L7);

  // update what objects do you see and tell your friends about it
  .concat(L1, L2, L3, L4, L5, L6, L7, Objects);
  for (.member(O, Objects)) {
    +anObject(O);
    !tellFriends(tell, anObject(O));
  }

  // check objects in sight
  for(.range(Y, B-V, B+V)) {
    for(.range(X, A-V, A+V)) {
      // this cell has been seen now
      .abolish(aUnseen([X,Y]));
      !tellFriends(achieve, removeUnseen([X,Y]));
      // there should be an object
      if (anObject([T, [X,Y]])) {
        // object is no longer there -> someone took it
        if (T \== shore & not (.member([T, [X,Y]], Objects) | water(X,Y))) {
          .abolish(anObject([T, [X,Y]]));
          !tellFriends(achieve, removeObject([T, [X,Y]]));
          .println("------------------------------------------------------------------------- Someone has taken the ", T, " on position ", [X,Y]);
        }
      }
      // process water
      if (water(X,Y) & not anObject([shore, [X,Y]])) {
        // this water cell is on the shore
        if (isOnTheShore([X,Y], [A-V,A+V], [B-V,B+V])) {
          .abolish(anObject([water, [X,Y]]));
          !tellFriends(achieve, removeObject([water, [X,Y]]));
          +anObject([shore, [X,Y]]);
          !tellFriends(tell, anObject([shore, [X,Y]]));
        } else {
          +anObject([water, [X,Y]]);
          !tellFriends(tell, anObject([water, [X,Y]]));
        }
      }
    }
  }.
