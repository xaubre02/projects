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
import java.awt.Stroke;
import java.awt.geom.Area;

import java.lang.Math;

import oracle.spatial.geometry.JGeometry;

/**
 * The class represents lake in map.
 * 
 * @author David Bednařík (xbedna62@stud.fit.vutbr.cz)
 */
public class Lake implements MapObject {
	private long m_id;
        private java.util.Date m_valid_from;
        
	private Point m_middle;
	private int m_radius;

	private boolean m_highlight;
	private boolean m_active;
	private boolean m_scaling;

	private Point m_movingPoint;

	/**
	 * Creates lake from JGeometry.
	 * @param geometry JGeometry
	 * @return new instance of lake
	 */
	static Lake createFromJGeometry(long id, JGeometry geometry, java.util.Date from)
	{
		double[] ordinates = geometry.getOrdinatesArray();

		Point middle = new Point((int)ordinates[0], (int)ordinates[5]);
		Point second = new Point((int)ordinates[4], (int)ordinates[5]);
                
                Lake newLake = new Lake(id, middle, second);
                newLake.setValid_from(from);
		return newLake;
	}

	public Lake(long id, Point middle, Point second)
	{
		m_id = id;
                m_valid_from = new java.util.Date(); // init with current date
                
		m_middle = middle;
		m_radius = countRadius(second);

		m_highlight = false;
		m_active = false;
		m_scaling = false;
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
		Area tmpArea = new Area(getJGeometry().createShape());
		if (tmpArea.contains(point.getX(), point.getY()))
			return true;
		else
			return false;
	}

	@Override
	public JGeometry getJGeometry()
	{
		double[] ordinates = new double[6];
		ordinates[0] = (int)m_middle.getX();
		ordinates[1] = (int)(m_middle.getY() + m_radius);
		ordinates[2] = (int)m_middle.getX();
		ordinates[3] = (int)(m_middle.getY() - m_radius);
		ordinates[4] = (int)(m_middle.getX() + m_radius);
		ordinates[5] = (int)m_middle.getY();

		int[] elemInfo = {1, 1003, 4};
		JGeometry jgeom = new JGeometry(2003, 0, elemInfo, ordinates);

		return jgeom;

		//return JGeometry.createCircle((int)m_middle.getX(), (int)m_middle.getY(), m_radius, 0);
	}

	@Override
	public void press(Point point) throws IllegalStateException
	{
		if (!m_active) {
			m_active = true;
			return;
		}

		m_scaling = false;

		// Check if the mouse was pressed on point circle to scale lake.
		int x = (int)m_middle.getX() + m_radius;
		int y = (int)m_middle.getY();
		if (Math.abs(x - point.getX()) <= RADIUS &&
			Math.abs(y - point.getY()) <= RADIUS) {
				m_scaling = true;
				m_movingPoint = new Point(x, y);

				return;
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
	public void drag(Point point)
	{
		if (m_movingPoint == null)
			return;

		if (m_scaling) {
			m_radius = countRadius(point);
		}
		else {
			int deltaX = (int)point.getX() - (int)m_movingPoint.getX();
			int deltaY = (int)point.getY() - (int)m_movingPoint.getY();

			m_middle.setLocation((int)m_middle.getX() + deltaX, (int)m_middle.getY() + deltaY);
		}

		m_movingPoint = point;
	}

	@Override
	public void release()
	{
		m_active = false;
		m_scaling = false;
		m_movingPoint = null;
	}

	@Override
	public boolean delete()
	{
		return true;
	}

	@Override
	public void draw(Graphics g)
	{
		if (m_highlight) {
			highlightDraw(g);
			return;
		}
			
		
		Color color = g.getColor();

		g.setColor(new Color(0, 130, 183));
		g.fillOval((int)m_middle.getX() - m_radius, (int)m_middle.getY() - m_radius, m_radius*2, m_radius*2);

		if (!m_active)
			g.setColor(Color.black);
		else
			g.setColor(Color.red);
		g.drawOval((int)m_middle.getX() - m_radius, (int)m_middle.getY() - m_radius, m_radius*2, m_radius*2);

		// Draw circles in every point
		if (m_active) {
			g.setColor(Color.black);

			int x = (int)m_middle.getX() + m_radius;
			int y = (int)m_middle.getY();
			if (m_scaling) {
				Color tmpColor = g.getColor();
				g.setColor(Color.red);

				g.drawOval(x - RADIUS, y - RADIUS, RADIUS*2, RADIUS*2);

				g.setColor(tmpColor);
			}
			else {
				g.drawOval(x - RADIUS, y - RADIUS, RADIUS*2, RADIUS*2);
			}
		}

		g.setColor(color);
	}
	
	@Override
	public void highlightDraw(Graphics g)
	{
		m_highlight = false;
		
		Color color = g.getColor();

		g.setColor(new Color(0, 130, 183));
		g.fillOval((int)m_middle.getX() - m_radius, (int)m_middle.getY() - m_radius, m_radius*2, m_radius*2);

		g.setColor(Color.red);
		Graphics2D tmpG = (Graphics2D)g;

		Stroke stroke = tmpG.getStroke();
		tmpG.setStroke(new BasicStroke(2));
		g.drawOval((int)m_middle.getX() - m_radius, (int)m_middle.getY() - m_radius, m_radius*2, m_radius*2);

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
		return "lake";
	}

	private int countRadius(Point point)
	{
		double lenght = Math.sqrt(Math.pow(point.getX() - m_middle.getX(), 2) + Math.pow(point.getY() - m_middle.getY(), 2));
		return (int)Math.abs(lenght);
	}
}
