package boosted;

import jason.*;
import jason.asSemantics.*;
import jason.asSyntax.*;

import java.util.Collections;
import java.util.HashMap;
import java.util.ArrayList;
import java.util.List;
import java.awt.Point;


/**
 * Action that calculates the path to a goal destination using A* searching algorithm.
 */
public class aStar extends DefaultInternalAction {

    /**
     * Calculate distance between two points. Distance is the heuristic function.
     * @param p1 point 1
     * @param p2 point 2
     * @return distance
     */
    public static int distance(Point p1, Point p2) {
        return Math.abs(p2.x - p1.x) + Math.abs(p2.y - p1.y);
    }

    /**
     * Check whether a point is accessible. It cannot be an obstacle or outside of the grid.
     * @param p point to be checked
     * @param obstacles list of obstacles
     * @param gridX grid horizontal size
     * @param gridY grid vertical size
     * @return accessibility flag
     */
    private boolean isAccessible(Point p, List<Point> obstacles, int gridX, int gridY) {
        return p.x >= 0 && p.x < gridX && p.y >= 0 && p.y <gridY && !obstacles.contains(p);
    }

    /**
     * Get a list of all accessible neighbors.
     * @param p point to be checked
     * @param obstacles list of obstacles
     * @param gridX grid horizontal size
     * @param gridY grid vertical size
     * @return neighbor list
     */
    private List<Point> neighborsOf(Point p, List<Point> obstacles, int gridX, int gridY) {
        List<Point> neighbors = new ArrayList<>();
        Point left  = new Point(p.x-1, p.y);
        Point right = new Point(p.x+1, p.y);
        Point upper = new Point(p.x, p.y-1);
        Point lower = new Point(p.x, p.y+1);

        if (isAccessible(left,  obstacles, gridX, gridY)) { neighbors.add(left); }
        if (isAccessible(right, obstacles, gridX, gridY)) { neighbors.add(right); }
        if (isAccessible(upper, obstacles, gridX, gridY)) { neighbors.add(upper); }
        if (isAccessible(lower, obstacles, gridX, gridY)) { neighbors.add(lower); }

        return neighbors;
    }

    /**
     * Get the point from openSet with the lowest fScore.
     * @param openSet openSet
     * @param fScore fScore map
     * @return point with the lowest fScore
     */
    private Point getLowestFscorePoint(List<Point> openSet, HashMap<Point, Integer> fScore) {
        Point lowest = openSet.get(0);
        int min = fScore.get(lowest);
        int cur;
        for (Point point : openSet) {
            cur = fScore.get(point);
            if (cur < min) {
                min = cur;
                lowest = point;
            }
        }
        return lowest;
    }

    /**
     * Search for the shortest path to the goal destination using A* algorithm.
     * @param start starting position
     * @param goal goal position
     * @param obstacles list of obstacles
     * @param gridX grid horizontal size
     * @param gridY grid vertical size
     * @return path to the goal position
     */
    private List<Point> findPath (Point start, Point goal, List<Point> obstacles, int gridX, int gridY) {
        // goal is not accessible
        if (!isAccessible(goal, obstacles, gridX, gridY)) {
            return new ArrayList<>();
        }

        List<Point>            openSet = new ArrayList<>(); // The set of discovered nodes that may need to be (re-)expanded. Initially, only the start node is known.
        HashMap<Point, Point> cameFrom = new HashMap<>();   // For node n, cameFrom[n] is the node immediately preceding it on the cheapest path from start to n currently known.
        HashMap<Point, Integer> gScore = new HashMap<>();   // For node n, gScore[n] is the cost of the cheapest path from start to n currently known.
        HashMap<Point, Integer> fScore = new HashMap<>();   // For node n, fScore[n] := gScore[n] + h(n).

        // map with default value of Infinity
        for (int x = 0; x < gridX; x++) {
            for (int y = 0; y < gridY; y++) {
                gScore.put(new Point(x, y), 65535);
                fScore.put(new Point(x, y), 65535);
            }
        }
        // initialize
        Point current;
        int tentative;
        openSet.add(start);
        gScore.put(start, 0);
        fScore.put(start, distance(start, goal));
        while (!openSet.isEmpty()) {
            // current := the node in openSet having the lowest fScore[] value
            current = getLowestFscorePoint(openSet, fScore);
            // path found
            if (current.equals(goal)) {
                // reconstruct hte path
                List<Point> path = new ArrayList<>();
                path.add(current);
                while (cameFrom.containsKey(current)) {
                    current = cameFrom.get(current);
                    path.add(current);
                }
                // remove the last point (start) and reverse the order
                path.remove(path.size() - 1);
                Collections.reverse(path);
                return path;
            }
            openSet.remove(current);
            for (Point neighbor : neighborsOf(current, obstacles, gridX, gridY)) {
                // d(current,neighbor) is the weight of the edge from current to neighbor -> always 1
                // tentative_gScore is the distance from start to the neighbor through current
                tentative = gScore.get(current) + 1;
                if (tentative < gScore.get(neighbor)) {
                    cameFrom.put(neighbor, current);
                    gScore.put(neighbor, tentative);
                    fScore.put(neighbor, tentative + distance(neighbor, goal));
                    if (!openSet.contains(neighbor)) {
                        openSet.add(neighbor);
                    }
                }
            }
        }
        // no path found -> return empty list
        return new ArrayList<>();
    }

    /**
     * Jason action interface.
     */
    @Override
    public Object execute(TransitionSystem ts, Unifier un, Term[] args) throws Exception {
        Point           start = JasonTypes.coords2point((ListTerm)args[0]);
        Point            goal = JasonTypes.coords2point((ListTerm)args[1]);
        List<Point> obstacles = JasonTypes.listTerm2listPoints((ListTerm)args[2]);
        int             gridX = JasonTypes.numberTerm2int((NumberTerm)args[3]);
        int             gridY = JasonTypes.numberTerm2int((NumberTerm)args[4]);
        
        // find the path
        List<Point> path = findPath(start, goal, obstacles, gridX, gridY);
		return un.unifies(args[5], JasonTypes.listPoints2listTermImpl(path)); 
    }
}