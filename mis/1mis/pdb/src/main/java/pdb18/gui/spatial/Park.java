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

import java.util.Vector;

import oracle.spatial.geometry.JGeometry;

/**
 * The class represents park in map.
 * 
 * @author David Bednařík (xbedna62@stud.fit.vutbr.cz)
 */
public class Park extends Enclosure {
        private java.util.Date m_valid_from;
    
	static Park createPark(long id, Vector<Point> points)
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

		return new Park(id, xpoints, ypoints, npoints);
	}

	/**
	 * Creates park from JGeometry.
	 * @param geometry JGeometry
	 * @return new instance of park
	 */
	static Park createFromJGeometry(long id, JGeometry geometry, java.util.Date from)
	{
		int[] xpoints = new int[geometry.getNumPoints() - 1];
		int[] ypoints = new int[geometry.getNumPoints() - 1];
		double[] ordinates = geometry.getOrdinatesArray();

		for (int i = 0, j = 0; i < geometry.getNumPoints() - 1; i++, j += 2) {
			xpoints[i] = (int)ordinates[j];
			ypoints[i] = (int)ordinates[j + 1];
		}

                Park newPark = new Park(id, xpoints, ypoints, geometry.getNumPoints() - 1);
                newPark.setValid_from(from);
		return newPark;
	}

	public Park(long id, int[] xpoints, int[] ypoints, int npoints)
	{
		super(id, xpoints, ypoints, npoints);
	}

	@Override
	public void draw(Graphics g)
	{
		if (m_highlight) {
			highlightDraw(g);
			return;
		}
		
		Color color = g.getColor();

		g.setColor(new Color(56, 202, 57));
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

					g.drawOval(xpoints[i] - (RADIUS), ypoints[i] - (RADIUS), RADIUS*2, RADIUS*2);

					g.setColor(tmpColor);
				}
				else {
					g.drawOval(xpoints[i] - (RADIUS), ypoints[i] - (RADIUS), RADIUS*2, RADIUS*2);
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

		g.setColor(new Color(56, 202, 57));
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
	public String type()
	{
		return "park";
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
}
