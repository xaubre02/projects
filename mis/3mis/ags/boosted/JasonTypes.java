package boosted;

import jason.*;
import jason.asSemantics.*;
import jason.asSyntax.*;

import java.util.ArrayList;
import java.util.List;
import java.awt.Point;

/**
 * Jason - Java data type interface.
 */
public class JasonTypes {

    /**
     * Convert a NumberTerm object to an integer.
     * @param term NumberTerm object
     * @return int
     */
    public static int numberTerm2int(NumberTerm term) throws Exception {
        return (int)term.solve();
    }

    /**
     * Convert an integer to a NumberTermImpl object.
     * @param number int
     * @return NumberTermImpl
     */
    public static NumberTermImpl int2numberTermImpl(int number) throws Exception {
        return new NumberTermImpl(number);
    }

    /**
     * Convert coordinates to a Point.
     * @param list ListTerm object
     * @return Point
     */
    public static Point coords2point(ListTerm list) throws Exception {
        return new Point(numberTerm2int((NumberTerm)list.get(0)),
                         numberTerm2int((NumberTerm)list.get(1)));
    }

    /**
     * Convert a Point to a ListTermImpl object.
     * @param point Point
     * @return ListTermImpl
     */
    public static ListTermImpl point2coords(Point point) throws Exception {
        ListTermImpl list = new ListTermImpl();
        list.add(int2numberTermImpl(point.x));
        list.add(int2numberTermImpl(point.y));
        return list;
    }

    /**
     * Convert ListTerm object to a list of points.
     * @param list ListTerm object
     * @return List<Point>
     */
    public static List<Point> listTerm2listPoints(ListTerm list) throws Exception {
        List<Point> points = new ArrayList<>();
        for (Term coords : list) {
            points.add(coords2point((ListTerm)coords));
        }
        return points;
    }
    
    /**
     * Convert List<Point> object to a ListTermImpl.
     * @param list List<Point>
     * @return ListTermImpl
     */
    public static ListTermImpl listPoints2listTermImpl(List<Point> list) throws Exception {
        ListTermImpl lti = new ListTermImpl();
        for (Point point : list) {
            lti.add(point2coords(point));
        }
        return lti;
    }
}