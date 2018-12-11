/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package pdb18.gui.spatial;

import java.awt.Graphics;
import java.awt.Point;

import oracle.spatial.geometry.JGeometry;

/**
 * The interface represents all map objects 
 * which the map can display.
 * 
 * @author David Bednařík (xbedna62@stud.fit.vutbr.cz)
 */
public interface MapObject {
	static final int RADIUS = 8;

	/**
	 * @return id of object 
	 */
	long getId();
	/**
	 * @param point the point to determine if it is inside an object
	 * @return true if the point is in object otherwise false
	 */
	boolean isIn(Point point);
	/**
	 * Converts object to JGeometry.
	 * @return resulting JGeometry
	 */
	JGeometry getJGeometry();
	/**
	 * The mouse was pressed on the object.
	 * @param point point where the mouse was pressed
	 */
	void press(Point point);
	/**
	 * The mouse dragged with the object.
	 * @param point point where the mouse was dragged
	 */
	void drag(Point point);
	/**
	 * The mouse was released on the object. 
	 */
	void release();
	/**
	 * Delete the object or just one point of the object.
	 * @return true if the object has to be deleted, otherwise false
	 */
	boolean delete();
	/**
	 * The object draw himself to the given Graphics.
	 * @param g Graphics on which the object is drawn.
	 */
	void draw(Graphics g);
	/**
	 * The object draw himself to the given Graphics in highlight mode.
	 * @param g Graphics on which the object is drawn.
	 */
	void highlightDraw(Graphics g);
	/**
	 * Next draw of the object will be higlight. 
	 */
	void highlightDrawRequest();
	/**
	 * @return type of the object 
	 */
	String type();
        /**
	 * @return date valid from
	 */
        java.util.Date getValid_from();
        /**
	 * set date valid from
	 */
        void setValid_from(java.util.Date d);
}