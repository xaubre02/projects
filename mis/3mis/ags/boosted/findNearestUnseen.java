package boosted;

import jason.*;
import jason.asSemantics.*;
import jason.asSyntax.*;

import java.util.Collections;
import java.util.HashMap;
import java.util.ArrayList;
import java.util.List;
import java.awt.Point;

public class findNearestUnseen extends DefaultInternalAction {

    /**
     * Find a minimal distance between the given position and a point in the list.
     * @param pos given position
     * @param points list of points
     * @return minimal distance
     */
    private int getMinimalDistance(Point pos, List<Point> points) {
        int dis, min = 65535;
        for (Point point : points) {
            dis = aStar.distance(pos, point);
            if (dis < min) {
                min = dis;
            }
        }
        return min;
    }

    /**
     * Find a minimal distance between two lists of points.
     * @param points1 list of points #1
     * @param points2 list of points #2
     * @return minimal distance
     */
    private int getMinimalDistance(List<Point> points1, List<Point> points2) {
        int dis, min = 65535;
        for (Point point1 : points1) {
            for (Point point2 : points2) {
                dis = aStar.distance(point1, point2);
                if (dis < min) {
                    min = dis;
                }
            }
        }
        return min;
    }

    /**
     * Get a list of points that are equally distant from the given position.
     * @param pos position
     * @param points list of points
     * @param distance distance
     * @return list of equally distant points
     */
    private List<Point> getPointsWithDistance(Point pos, List<Point> points, int distance) {
        List<Point> distant = new ArrayList<>();
        for (Point point : points) {
            if (aStar.distance(pos, point) == distance) {
                distant.add(point);
            }
        }
        return distant;
    }

    /**
     * Get a list of points that are equally distant from the given position.
     * @param points1 list of points #1
     * @param points2 list of points #2
     * @param distance distance
     * @return list of equally distant points
     */
    private List<Point> getPointsWithDistance(List<Point> points1, List<Point> points2, int distance) {
        List<Point> distant = new ArrayList<>();
        for (Point point1 : points1) {
            for (Point point2 : points2) {
                if (aStar.distance(point1, point2) == distance) {
                    distant.add(point1);
                }
            }
        }
        return distant;
    }

    /**
     * Find the nearest unseen point from my position. This point is also nearest to the depot from nearest unseen points from my position.
     * @param myPos my position
     * @param depot depot position
     * @param unseen list of unseen points
     * @return nearest unseen point
     */
    private Point findNearest(Point myPos, Point depot, List<Point> unseen) {
        // everything has been seen
        if (unseen.size() == 0) {
            return new Point(999,999);
        }

        // find minimal distances from my position and from depot
        int disMyPos = getMinimalDistance(myPos, unseen);
        int disDepot = getMinimalDistance(depot, unseen);
        // get points with those distances
        List<Point> nearestMyPos = getPointsWithDistance(myPos, unseen, disMyPos);
        List<Point> nearestDepot = getPointsWithDistance(depot, unseen, disDepot);
        // find minimal distance between 2 points from those sets
        int minDis = getMinimalDistance(nearestMyPos, nearestDepot);
        // get points from the first set with this distance
        return getPointsWithDistance(nearestMyPos, nearestDepot, minDis).get(0);
    }

    /**
     * Jason action interface.
     */
    @Override
    public Object execute(TransitionSystem ts, Unifier un, Term[] args) throws Exception {
        Point        myPos = JasonTypes.coords2point((ListTerm)args[0]);
        Point        depot = JasonTypes.coords2point((ListTerm)args[1]);
        List<Point> unseen = JasonTypes.listTerm2listPoints((ListTerm)args[2]);

        // find the nearest unseen point
        Point nearest = findNearest(myPos, depot, unseen);
        return un.unifies(args[3], JasonTypes.point2coords(nearest));
    }
}
