/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package pdb18.gui.spatial;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.Polygon;
import java.awt.Stroke;
import java.awt.BasicStroke;
import java.awt.geom.Area;

import java.lang.Math;
import java.util.Vector;

import oracle.spatial.geometry.JGeometry;

/**
 * The class represents path in map.
 * 
 * @author David Bednařík (xbedna62@stud.fit.vutbr.cz)
 */
public class Path implements MapObject {
	static final int WIDTH = 10;

	private long m_id;
        private java.util.Date m_valid_from;
        
	private Vector<Point> m_points;

	private boolean m_highlight;
	private boolean m_active;
	private int m_focusPoint;

	private Point m_movingPoint;

	/**
	 * Creates path from JGeometry.
	 * @param geometry JGeometry
	 * @return new instance of path
	 */
	static Path createFromJGeometry(long id, JGeometry geometry, java.util.Date from)
	{
		Vector<Point> points = new Vector<Point>();
		double[] ordinates = geometry.getOrdinatesArray();

		for (int i = 0; i < geometry.getNumPoints() * 2; i += 2)
			points.add(new Point((int)ordinates[i], (int)ordinates[i + 1]));

                Path newPath = new Path(id, points);
                newPath.setValid_from(from);
		return newPath;
	}

	public Path(long id, Vector<Point> points)
	{
		m_id = id;
                m_valid_from = new java.util.Date(); // init with current date
		m_points = (Vector<Point>)points.clone();

		m_highlight = false;
		m_active = false;
		m_focusPoint = -1;

		m_movingPoint = null;
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
		try {
			findStartPointOfEdge(point);
		}
		catch (IllegalStateException e) {
			return false;
		}

		return true;
	}

	@Override
	public JGeometry getJGeometry()
	{
		int[] elemInfo = {1, 2, 1};
		double[] ordinates = new double[2 * m_points.size()];

		int i = 0;
		for (Point point : m_points) {
			ordinates[i] = point.getX();
			ordinates[i + 1] = point.getY();

			i += 2;
		}

		if (m_points.size() == 2)
			return new JGeometry(2002, 0, elemInfo, ordinates);
		else
			return new JGeometry(2006, 0, elemInfo, ordinates);
	}

	@Override
	public void press(Point point)
	{
		if (!m_active) {
			m_active = true;
			return;
		}

		m_focusPoint = -1;

		// Check if the mouse was pressed on some point circle.
		try {
			m_focusPoint = findIndexOfPoint(point);
			m_movingPoint = m_points.get(m_focusPoint);
			return;
		}
		catch (IllegalStateException e) {
			// It was not clicked at any of the editing object.
		}

		// Check if the mouse was pressed on some edge (extend object).
		try {
			int i = findStartPointOfEdge(point);
			addPointAfter(i, point);
			m_focusPoint = i + 1;
			m_movingPoint = point;
			return;
		}
		catch (IllegalStateException e) {
			// It was not clicked at any edge of the editing object.
		}

		// It was not clicked on the object.
		if (!isIn(point)) {
			release();
			throw new IllegalStateException("it was not clicked to the object");
		}
	}

	@Override
	public void drag(Point point)
	{
		if (m_movingPoint == null)
			return;

		if (m_focusPoint >= 0)
			m_points.get(m_focusPoint).setLocation(point.getX(), point.getY());

		m_movingPoint = point;
	}

	@Override
	public void release()
	{
		m_active = false;
		m_focusPoint = -1;
		m_movingPoint = null;
	}

	@Override
	public boolean delete()
	{
		if (m_focusPoint >= 0) {
			m_points.remove(m_focusPoint);
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

		g.setColor(Color.gray);
		Graphics2D tmpG = (Graphics2D)g;

		Stroke stroke = tmpG.getStroke();
		tmpG.setStroke(new BasicStroke(WIDTH));

		for (int i = 0; i < m_points.size() - 1; i++) {
			tmpG.drawLine(
				(int)m_points.get(i).getX(), (int)m_points.get(i).getY(),
				(int)m_points.get(i + 1).getX(), (int)m_points.get(i + 1).getY());
		}

		tmpG.setStroke(stroke);

		// Draw circles in every point
		if (m_active) {
			g.setColor(Color.black);
			for (int i = 0; i < m_points.size(); i++) {
				if (i == m_focusPoint) {
					Color tmpColor = g.getColor();
					g.setColor(Color.red);

					g.drawOval((int)m_points.get(i).getX() - RADIUS, (int)m_points.get(i).getY() - RADIUS, 2*RADIUS, 2*RADIUS);

					g.setColor(tmpColor);
				}
				else {
					g.drawOval((int)m_points.get(i).getX() - RADIUS, (int)m_points.get(i).getY() - RADIUS, 2*RADIUS, 2*RADIUS);
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

		g.setColor(new Color(200, 128, 128));
		Graphics2D tmpG = (Graphics2D)g;

		Stroke stroke = tmpG.getStroke();
		tmpG.setStroke(new BasicStroke(WIDTH));

		for (int i = 0; i < m_points.size() - 1; i++) {
			tmpG.drawLine(
				(int)m_points.get(i).getX(), (int)m_points.get(i).getY(),
				(int)m_points.get(i + 1).getX(), (int)m_points.get(i + 1).getY());
		}

		tmpG.setStroke(stroke);

		g.setColor(color);
	}
	
	@Override
	public void highlightDrawRequest()
	{
		m_highlight = true;
	}

	/**
	 * Tries to find point on which was clicked.
	 * @param point ordinates of point where was clicked
	 * @return index of point on which was clicked
	 */
	protected int findIndexOfPoint(Point point) throws IllegalStateException
	{
		for (int i = 0; i < m_points.size(); i++) {
			if (Math.abs(m_points.get(i).getX() - point.getX()) > RADIUS ||
				Math.abs(m_points.get(i).getY() - point.getY()) > RADIUS) {
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
		for (int i = 0; i < m_points.size() - 1; i++) {
			double deltaX = Math.abs(m_points.get(i + 1).getX() - m_points.get(i).getX());
			double deltaY = Math.abs(m_points.get(i + 1).getY() - m_points.get(i).getY());

			Polygon tmpPolygon = new Polygon();
			if (deltaX > deltaY) {
				tmpPolygon.addPoint((int)m_points.get(i).getX(), (int)m_points.get(i).getY() - WIDTH/2);
				tmpPolygon.addPoint((int)m_points.get(i).getX(), (int)m_points.get(i).getY() + WIDTH/2);
				tmpPolygon.addPoint((int)m_points.get(i + 1).getX(), (int)m_points.get(i + 1).getY() + WIDTH/2);
				tmpPolygon.addPoint((int)m_points.get(i + 1).getX(), (int)m_points.get(i + 1).getY() - WIDTH/2);
			}
			else {
				tmpPolygon.addPoint((int)m_points.get(i).getX() - WIDTH/2, (int)m_points.get(i).getY());
				tmpPolygon.addPoint((int)m_points.get(i).getX() + WIDTH/2, (int)m_points.get(i).getY());
				tmpPolygon.addPoint((int)m_points.get(i + 1).getX() + WIDTH/2, (int)m_points.get(i + 1).getY());
				tmpPolygon.addPoint((int)m_points.get(i + 1).getX() - WIDTH/2, (int)m_points.get(i + 1).getY());
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
	 * @param point new point
	 */
	protected void addPointAfter(int index, Point point)
	{
		Vector<Point> newPoints = new Vector<Point>();

		int i = 0;
		for (Point p : m_points) {
			newPoints.add(p);
			if (i == index)
				newPoints.add(point);

			i++;
		}

		m_points = newPoints;
	}

	@Override
	public String type()
	{
		return "path";
	}
}
