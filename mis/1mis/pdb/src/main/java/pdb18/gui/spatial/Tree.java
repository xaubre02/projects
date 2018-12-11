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
import java.awt.geom.Point2D;

import java.lang.Math;

import oracle.spatial.geometry.JGeometry;

/**
 * The class represents tree in map.
 * 
 * @author David Bednařík (xbedna62@stud.fit.vutbr.cz)
 */
public class Tree implements MapObject {
	static final int RADIUS = 6;

	private long m_id;
        private java.util.Date m_valid_from;

	private Point m_point;

	private boolean m_highlight;
	private boolean m_active;

	private Point m_movingPoint;

	/**
	 * Creates tree from JGeometry.
	 * @param geometry JGeometry
	 * @return new instance of tree
	 */
	static Tree createFromJGeometry(long id, JGeometry geometry, java.util.Date from)
	{
		Point2D point = geometry.getJavaPoint();
                
                Tree newTree = new Tree(id, new Point((int)point.getX(), (int)point.getY()));
                newTree.setValid_from(from);
		return newTree;
	}

	public Tree(long id, Point point)
	{
		m_id = id;
                m_valid_from = new java.util.Date(); // init with current date
		m_point = point;

		m_highlight = false;
		m_active = false;
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
		if (Math.abs(m_point.getX() - point.getX()) <= RADIUS &&
			Math.abs(m_point.getY() - point.getY()) <= RADIUS) {
				return true;
		}
		else {
			return false;
		}
	}

	@Override
	public JGeometry getJGeometry()
	{
		return new JGeometry(m_point.getX(), m_point.getY(), 0);
	}

	@Override
	public void press(Point point)
	{
		if (!m_active) {
			m_active = true;
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

		int deltaX = (int)point.getX() - (int)m_movingPoint.getX();
		int deltaY = (int)point.getY() - (int)m_movingPoint.getY();

		m_point.setLocation((int)m_point.getX() + deltaX, (int)m_point.getY() + deltaY);

		m_movingPoint = point;
	}

	@Override
	public void release()
	{
		m_active = false;
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

		g.setColor(new Color(0, 90, 0));
		g.fillOval((int)m_point.getX() - RADIUS, (int)m_point.getY() - RADIUS, RADIUS*2, RADIUS*2);

		if (!m_active)
			g.setColor(Color.black);
		else
			g.setColor(Color.red);
		g.drawOval((int)m_point.getX() - RADIUS, (int)m_point.getY() - RADIUS, RADIUS*2, RADIUS*2);

		g.setColor(color);
	}
	
	@Override
	public void highlightDraw(Graphics g)
	{
		m_highlight = false;
		
		Color color = g.getColor();

		g.setColor(new Color(0, 90, 0));
		g.fillOval((int)m_point.getX() - RADIUS, (int)m_point.getY() - RADIUS, RADIUS*2, RADIUS*2);

		g.setColor(Color.red);
		Graphics2D tmpG = (Graphics2D)g;

		Stroke stroke = tmpG.getStroke();
		tmpG.setStroke(new BasicStroke(2));
		g.drawOval((int)m_point.getX() - RADIUS, (int)m_point.getY() - RADIUS, RADIUS*2, RADIUS*2);
			
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
		return "tree";
	}
}

