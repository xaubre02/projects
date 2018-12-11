/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package pdb18.gui.spatial;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.Polygon;
import java.awt.Stroke;
import java.awt.geom.Area;

import java.lang.Math;
import java.lang.IllegalStateException;
import java.util.Vector;

import oracle.spatial.geometry.JGeometry;

/**
 * The class represents enclosure in map.
 * 
 * @author David Bednařík (xbedna62@stud.fit.vutbr.cz)
 */
public class Enclosure extends Polygon implements MapObject {
	protected long m_id;
	private String m_type;
        private java.util.Date m_valid_from;
        
	protected boolean m_highlight;
	protected boolean m_active;
	protected int m_focusPoint;

	protected Point m_movingPoint;

	static Enclosure createEnclosure(long id, Vector<Point> points)
	{
		int[] xpoints = new int[points.size()];
		int[] ypoints = new int[points.size()];
		int npoints = points.size();

		int i = 0;
		for (Point point : points) {
			xpoints[i] = (int)point.getX();
			ypoints[i] = (int)point.getY();

			i++;
		}

		return new Enclosure(id, xpoints, ypoints, npoints);
	}

	/**
	 * Creates enclosure from JGeometry.
	 * @param geometry JGeometry
	 * @return new instance of enclosure
	 */
	static Enclosure createFromJGeometry(long id, String type, JGeometry geometry, java.util.Date from)
	{
		int[] xpoints = new int[geometry.getNumPoints() - 1];
		int[] ypoints = new int[geometry.getNumPoints() - 1];
		double[] ordinates = geometry.getOrdinatesArray();

		for (int i = 0, j = 0; i < geometry.getNumPoints() - 1; i++, j += 2) {
			xpoints[i] = (int)ordinates[j];
			ypoints[i] = (int)ordinates[j + 1];
		}

		Enclosure newEnclosure = new Enclosure(id, xpoints, ypoints, geometry.getNumPoints() - 1);
		newEnclosure.setType(type);
                newEnclosure.setValid_from(from);
		return newEnclosure;
	}

	public Enclosure(long id, int[] xpoints, int[] ypoints, int npoints)
	{
		super(xpoints, ypoints, npoints);

		m_id = id;
                m_valid_from = new java.util.Date(); // init with current date
		m_highlight = false;
		m_active = false;
		m_focusPoint = -1;
		m_movingPoint = null;
	}
	
	public String getType()
	{
		return m_type;
	}
	
	public void setType(String type)
	{
		m_type = type;
	}

	@Override
	public long getId()
	{
		return m_id;
	}
        
        @Override
        public java.util.Date getValid_from()
	{
		return m_valid_from;
	}
	
        @Override
	public void setValid_from(java.util.Date d)
	{
		m_valid_from = d;
	}

	@Override
	public boolean isIn(Point point)
	{
		Area tmpArea = new Area(this);
		if (tmpArea.contains(point.getX(), point.getY()))
			return true;
		else
			return false;
	}

	@Override
	public JGeometry getJGeometry()
	{
		double[] ordinates = new double[npoints * 2 + 2];
		for (int i = 0, j = 0; i < npoints; i++, j += 2) {
			ordinates[j] = (double)xpoints[i];
			ordinates[j + 1] = (double)ypoints[i];
		}
		ordinates[npoints * 2] = (double)xpoints[0];
		ordinates[npoints * 2 + 1] = (double)ypoints[0];

		int[] elemInfo = {1, 1003, 1};
		JGeometry jgeom = new JGeometry(2003, 0, elemInfo, ordinates);

		return jgeom;
	}

	@Override
	public void press(Point point) throws IllegalStateException
	{
		// First press in the object.
		if (!m_active) {
			m_active = true;
			return;
		}

		m_focusPoint = -1;

		// Check if the mouse was pressed on some point circle.
		try {
			m_focusPoint = findIndexOfPoint(point);
			m_movingPoint = new Point(xpoints[m_focusPoint], ypoints[m_focusPoint]);
			return;
		}
		catch (IllegalStateException e) {
			// It was not clicked at any of the editing object.
		}

		// Check if the mouse was pressed on some edge (extend object).
		try {
			int i = findStartPointOfEdge(point);
			addPointAfter(i, (int)point.getX(), (int)point.getY());
			m_focusPoint = i + 1;
			m_movingPoint = point;
			return;
		}
		catch (IllegalStateException e) {
			// It was not clicked at any edge of the editing object.
		}

		// It is clicked to object with intend to move it.
		if (isIn(point)) {
			m_movingPoint = point;
		}
		else {
			release();
			throw new IllegalStateException("it was not clicked to the object");
		}
	}

	@Override
	public void release()
	{
		m_active = false;
		m_focusPoint = -1;
		m_movingPoint = null;
	}

	@Override
	public void drag(Point point)
	{
		if (m_movingPoint == null)
			return;

		if (m_focusPoint >= 0) {
			xpoints[m_focusPoint] = (int)point.getX();
			ypoints[m_focusPoint] = (int)point.getY();
		}
		else {
			translate(
				(int)point.getX() - (int)m_movingPoint.getX(),
				(int)point.getY() - (int)m_movingPoint.getY());
		}

		m_movingPoint = point;
	}

	@Override
	public boolean delete()
	{
		if (m_focusPoint >= 0) {
			removeFocusedPoint();
			m_focusPoint = -1;
			return false;
		}
		else {
			return true;
		}
	}

	@Override
	public void draw(Graphics g)
	{
		if (m_highlight) {
			highlightDraw(g);
			return;
		}
		
		Color color = g.getColor();

		g.setColor(new Color(168, 146, 101));
		g.fillPolygon(this);

		if (!m_active)
			g.setColor(Color.black);
		else
			g.setColor(Color.red);
		g.drawPolygon(this);

		// Draw circles in every point
		if (m_active) {
			g.setColor(Color.black);
			for (int i = 0; i < npoints; i++) {
				if (i == m_focusPoint) {
					Color tmpColor = g.getColor();
					g.setColor(Color.red);

					g.drawOval(xpoints[i] - RADIUS, ypoints[i] - RADIUS, RADIUS*2, RADIUS*2);

					g.setColor(tmpColor);
				}
				else {
					g.drawOval(xpoints[i] - RADIUS, ypoints[i] - RADIUS, RADIUS*2, RADIUS*2);
				}
			}
		}
			
		g.setColor(color);
	}
	
	@Override
	public void highlightDraw(Graphics g)
	{
		m_highlight = false;
		
		Color color = g.getColor();

		g.setColor(new Color(168, 146, 101));
		g.fillPolygon(this);

		g.setColor(Color.red);
		Graphics2D tmpG = (Graphics2D)g;

		Stroke stroke = tmpG.getStroke();
		tmpG.setStroke(new BasicStroke(2));
		tmpG.drawPolygon(this);
			
		tmpG.setStroke(stroke);
		g.setColor(color);
	}
	
	@Override
	public void highlightDrawRequest()
	{
		m_highlight = true;
	}

	@Override
	public String type()
	{
		return "enclosure";
	}

	/**
	 * Tries to find point on which was clicked.
	 * @param point ordinates of point where was clicked
	 * @return index of point on which was clicked
	 */
	protected int findIndexOfPoint(Point point) throws IllegalStateException
	{
		for (int i = 0; i < npoints; i++) {
			if (Math.abs(xpoints[i] - point.getX()) > RADIUS ||
				Math.abs(ypoints[i] - point.getY()) > RADIUS) {
				continue;
			}

			return i;
		}

		throw new IllegalStateException("point not found");
	}

	/**
	 * Tries to find first point of edge on which was clicked.
	 * @param point ordinates of point where was clicked
	 * @return index of beginning pointof edge on which was clicked
	 */
	protected int findStartPointOfEdge(Point point) throws IllegalStateException
	{
		for (int i = 0; i < npoints; i++) {
			int j = i + 1;
			if (j == npoints)
				j = 0;

			double deltaX = Math.abs(xpoints[j] - xpoints[i]);
			double deltaY = Math.abs(ypoints[j] - ypoints[i]);

			Polygon tmpPolygon = new Polygon();
			if (deltaX > deltaY) {
				tmpPolygon.addPoint(xpoints[i], ypoints[i] - 5);
				tmpPolygon.addPoint(xpoints[i], ypoints[i] + 5);
				tmpPolygon.addPoint(xpoints[j], ypoints[j] + 5);
				tmpPolygon.addPoint(xpoints[j], ypoints[j] - 5);
			}
			else {
				tmpPolygon.addPoint(xpoints[i] - 5, ypoints[i]);
				tmpPolygon.addPoint(xpoints[i] + 5, ypoints[i]);
				tmpPolygon.addPoint(xpoints[j] + 5, ypoints[j]);
				tmpPolygon.addPoint(xpoints[j] - 5, ypoints[j]);
			}

			Area tmpArea = new Area(tmpPolygon);
			if (tmpArea.contains(point.getX(), point.getY()))
				return i;
		}

		throw new IllegalStateException("start point of edge not found");
	}

	/**
	 * It adds new point after some point defined by index.
	 * @param index after this point is added new point
	 * @param x x ordinate of new point
	 * @param y y ordinate of new point
	 */
	protected void addPointAfter(int index, int x, int y)
	{
		int[] xs = new int[npoints + 1];
		int[] ys = new int[npoints + 1];
		int j = 0;
		for (int i = 0; i < npoints; i++) {
			xs[j] = xpoints[i];
			ys[j] = ypoints[i];
			j++;

			if (i == index) {
				xs[j] = x;
				ys[j] = y;
				j++;
			}
		}

		xpoints = xs;
		ypoints = ys;
		npoints++;
	}

	/**
	 * Removes the focused point.
	 */
	protected void removeFocusedPoint()
	{
		int[] xs = new int[npoints - 1];
		int[] ys = new int[npoints - 1];
		int j = 0;
		for (int i = 0; i < npoints; i++) {
			if (i == m_focusPoint)
				continue;

			xs[j] = xpoints[i];
			ys[j] = ypoints[i];
			j++;
		}

		xpoints = xs;
		ypoints = ys;
		npoints--;
	}
}
