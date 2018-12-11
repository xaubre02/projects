/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package pdb18.gui.spatial;

import java.awt.Graphics;
import java.awt.Point;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import javax.swing.JPanel;
import javax.swing.JButton;
import javax.swing.JOptionPane;

import java.util.Vector;
import java.lang.Exception;
import java.lang.IllegalStateException;
import java.sql.CallableStatement;

import java.sql.Connection;
import java.sql.Date;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.Statement;
import java.sql.Struct;
import java.text.SimpleDateFormat;
import java.util.TreeSet;
import javax.swing.DefaultListModel;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JTextField;
import oracle.spatial.geometry.JGeometry;
import org.jdesktop.swingx.JXDatePicker;

import pdb18.connection.OracleDBConnection;

/**
 * The class represents ZOO map and implements work with it.
 * Implements interactive map work, adding, editing and deleting
 * object and spatial operations over data.
 *
 * @author David Bednařík (xbedna62@stud.fit.vutbr.cz)
 */
public class ZooMap extends JPanel {
	private JButton m_buttonAddEnclosure;
	private JButton m_buttonAddPark;
	private JButton m_buttonAddLake;
	private JButton m_buttonAddTree;
	private JButton m_buttonAddPath;
	private JButton m_buttonSave;
	private JButton m_buttonDistance;
        private JButton m_buttonInside;
        private JButton m_buttonNeighbors;
        private JButton m_buttonRelate;
        private JButton m_buttonMostFood;
	private JLabel m_labelArea;
	private JLabel m_labelLength;
	private JTextField m_textFieldEnclosureType;
	private DefaultListModel<String> m_listOfAnimals;
	private JXDatePicker m_datePickerValidFrom;
	private JXDatePicker m_datePickerValidTo;

	private Vector<MapObject> m_groundObjects; // enclosures, parks
	private Vector<MapObject> m_aboveObjects; // lakes, trees, paths
	private Vector<MapObject> m_deletedObjects;
	private Vector<MapObject> m_updatedObjects;
	private Vector<MapObject> m_newObjects;

	private java.util.Date m_validFrom;
	private java.util.Date m_validTo;

	private MapObject m_focusedObject;

	private boolean m_addingNewObject; // adding mode
	private Vector<Point> m_pointsOfNewObject;

	private boolean m_distanceMode; // distance mode
	private boolean m_relateMode; // relate mode
	private MapObject m_modeObject;

	private long m_enclosureSeq;
	private long m_objectSeq;

	public ZooMap()
	{
		super();
		m_groundObjects = new Vector<MapObject>();
		m_aboveObjects = new Vector<MapObject>();
		m_deletedObjects = new Vector<MapObject>();
		m_updatedObjects = new Vector<MapObject>();
		m_newObjects = new Vector<MapObject>();

		m_focusedObject = null;

		m_addingNewObject = false;
		m_pointsOfNewObject = new Vector<Point>();

		m_distanceMode = false;
		m_relateMode = false;
		m_modeObject = null;

		m_enclosureSeq = 0;
		m_objectSeq = 0;

		initilizeListeners();
	}

	/**
	 * Loads zoo map from the database.
	 */
	public void loadFromDatabase()
	{
		m_focusedObject = null;
		m_distanceMode = false;
		m_relateMode = false;
		m_modeObject = null;
		m_addingNewObject = false;
		m_pointsOfNewObject.removeAllElements();

		m_groundObjects.removeAllElements();
		m_aboveObjects.removeAllElements();

		m_deletedObjects.removeAllElements();
		m_updatedObjects.removeAllElements();
		m_newObjects.removeAllElements();
                m_listOfAnimals.removeAllElements();

                m_textFieldEnclosureType.setText("");
                m_labelArea.setText("");
                m_labelLength.setText("");

                String tempEnclosuresQuery = "SELECT v1.cislo_vybehu, v1.typ, v1.geometrie, v1.validni_od FROM vybeh v1";
                String tempMapObjectsQuery = "SELECT o1.cislo_objektu, o1.typ, o1.geometrie, o1.validni_od FROM mapovy_objekt o1";
		m_validFrom = m_datePickerValidFrom.getDate();
		m_validTo = m_datePickerValidTo.getDate();
                // get current date
                java.util.Date current_date = new java.util.Date();

                if (m_validFrom != null && m_validTo != null) {
                    if (m_validFrom.getTime() > m_validTo.getTime()) {
                            JOptionPane.showMessageDialog(null, "Date valid to has to be later than date valid from.", "Error", JOptionPane.ERROR_MESSAGE);
                            return;
                    }
                    if (m_validFrom.getTime() > current_date.getTime()) {
                            JOptionPane.showMessageDialog(null, "Date valid from cannot be specified in future.", "Error", JOptionPane.ERROR_MESSAGE);
                            return;
                    }
                    // load objects that are valid in part of time in specified interval
                    tempEnclosuresQuery += " WHERE v1.validni_od = "
                                            + "(SELECT MAX(v2.validni_od) "
                                            + "FROM vybeh v2 "
                                            + "WHERE v1.cislo_vybehu = v2.cislo_vybehu AND "
                                                + "(v2.validni_od < ? AND (v2.validni_do IS NULL OR v2.validni_do > ?)))";

                    tempMapObjectsQuery += " WHERE o1.validni_od = "
                                            + "(SELECT MAX(o2.validni_od) "
                                            + "FROM mapovy_objekt o2 "
                                            + "WHERE o1.cislo_objektu = o2.cislo_objektu AND "
                                                + "(o2.validni_od < ? AND (o2.validni_do IS NULL OR o2.validni_do > ?)))";
		}
                else if (m_validFrom != null) { // only date_from was specified
                    if (m_validFrom.getTime() > current_date.getTime()) {
                            JOptionPane.showMessageDialog(null, "Date valid from cannot be specified in future.", "Error", JOptionPane.ERROR_MESSAGE);
                            return;
                    }
                    // load objects that to is later that specified validTo
                    tempEnclosuresQuery += " WHERE v1.validni_od = "
                                            + "(SELECT MAX(v2.validni_od) "
                                            + "FROM vybeh v2 "
                                            + "WHERE v1.cislo_vybehu = v2.cislo_vybehu) "
                                        + "AND (v1.validni_do IS NULL OR v1.validni_do > ?)";

                    tempMapObjectsQuery += " WHERE o1.validni_od = "
                                            + "(SELECT MAX(o2.validni_od) "
                                            + "FROM mapovy_objekt o2 "
                                            + "WHERE o1.cislo_objektu = o2.cislo_objektu) "
                                        + "AND (o1.validni_do IS NULL OR o1.validni_do > ?)";
                }
                else if (m_validTo != null) {  // only validTo was specified
                    // load objects that validity_from is lower that specified validTo
                    tempEnclosuresQuery += " WHERE v1.validni_od = "
                                            + "(SELECT MAX(v2.validni_od) "
                                            + "FROM vybeh v2 "
                                            + "WHERE v1.cislo_vybehu = v2.cislo_vybehu AND v2.validni_od < ?)";

                    tempMapObjectsQuery += " WHERE o1.validni_od = "
                                            + "(SELECT MAX(o2.validni_od) "
                                            + "FROM mapovy_objekt o2 "
                                            + "WHERE o1.cislo_objektu = o2.cislo_objektu AND o2.validni_od < ?)";
                }
                else {
                    // load valid current map
                    tempEnclosuresQuery += " WHERE v1.validni_do IS NULL";
                    tempMapObjectsQuery += " WHERE o1.validni_do IS NULL";
                }


		try {
			Connection conn = OracleDBConnection.getConnection();

			JGeometry jgeom = null;

			// Load enclosures
			try (PreparedStatement stmt = conn.prepareStatement(tempEnclosuresQuery)) {
                                // add temporality conditions
                                if (m_validFrom != null && m_validTo != null) {
                                        stmt.setDate(1, new java.sql.Date(m_validTo.getTime()));
                                        stmt.setDate(2, new java.sql.Date(m_validFrom.getTime()));
                                }
                                else if (m_validFrom != null) {
                                        stmt.setDate(1, new java.sql.Date(m_validFrom.getTime()));
                                }
                                else if (m_validTo != null) {
                                        stmt.setDate(1, new java.sql.Date(m_validTo.getTime()));
                                }

				try (ResultSet rset = stmt.executeQuery()) {
					while (rset.next()) {
						long enclosureId = rset.getLong("cislo_vybehu");
						String type = rset.getNString("typ");
						Struct object = (Struct)rset.getObject("geometrie");
                                                java.sql.Date valid_from = rset.getDate("validni_od");

						jgeom = JGeometry.loadJS(object);
                                                java.util.Date valid_fromConverted = new java.util.Date(valid_from.getTime());

						m_groundObjects.add(Enclosure.createFromJGeometry(enclosureId, type, jgeom, valid_fromConverted));
						if (m_enclosureSeq < enclosureId)
							m_enclosureSeq = enclosureId;
					}
				}
			}

			// Load other map objects
			try (PreparedStatement stmt = conn.prepareStatement(tempMapObjectsQuery)) {
                                // add temporality conditions
                                if (m_validFrom != null && m_validTo != null) {
                                        stmt.setDate(2, new java.sql.Date(m_validFrom.getTime()));
                                        stmt.setDate(1, new java.sql.Date(m_validTo.getTime()));
                                }
                                else if (m_validFrom != null) {
                                        stmt.setDate(1, new java.sql.Date(m_validFrom.getTime()));
                                }
                                else if (m_validTo != null) {
                                        stmt.setDate(1, new java.sql.Date(m_validTo.getTime()));
                                }

				try (ResultSet rset = stmt.executeQuery()) {
					while (rset.next()) {
                                                long objectId = rset.getLong("cislo_objektu");
						String type = rset.getNString("typ");
						Struct object = (Struct)rset.getObject("geometrie");
                                                java.sql.Date valid_from = rset.getDate("validni_od");

						jgeom = JGeometry.loadJS(object);
                                                java.util.Date valid_fromConverted = new java.util.Date(valid_from.getTime());

						if (type.equals("park"))
							m_groundObjects.add(Park.createFromJGeometry(objectId, jgeom, valid_fromConverted));
						else if (type.equals("lake"))
							m_aboveObjects.add(Lake.createFromJGeometry(objectId, jgeom, valid_fromConverted));
						else if (type.equals("tree"))
							m_aboveObjects.add(Tree.createFromJGeometry(objectId, jgeom, valid_fromConverted));
						else if (type.equals("path"))
							m_aboveObjects.add(Path.createFromJGeometry(objectId, jgeom, valid_fromConverted));

						if (m_objectSeq < objectId)
							m_objectSeq = objectId;
					}
				}
			}
		}
		catch (Exception e) {
			System.err.println("Exception: " + e.getMessage());
			JOptionPane.showMessageDialog(null, "Error with databse: " + e.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
		}

		repaint();
	}

	/**
	 * Save changes to database.
	 * @throws SQLException when error with databse occures
	 */
	private void saveChangesToDatabase() throws Exception
	{
                Connection conn = OracleDBConnection.getConnection();

		try {
			conn.setAutoCommit(false);

			// Insert new objects
			for (MapObject obj : m_newObjects) {
				if (obj.type().equals("enclosure")) {
					try (PreparedStatement istmt = conn.prepareStatement("INSERT INTO vybeh (cislo_vybehu, typ, geometrie, validni_od) VALUES(?, ?, ?, to_char(sysdate,'dd-mm-rr'))")) {
						Enclosure enc = (Enclosure)obj;
						Struct newObj = JGeometry.storeJS(conn, obj.getJGeometry());
						istmt.setLong(1, enc.getId());
						istmt.setNString(2, enc.getType());
						istmt.setObject(3, newObj);
						istmt.executeUpdate();
					}
				}
				else {
					try (PreparedStatement istmt = conn.prepareStatement("INSERT INTO mapovy_objekt (cislo_objektu, typ, geometrie, validni_od) VALUES(?, ?, ?, to_char(sysdate,'dd-mm-rr'))")) {
						Struct newObj = JGeometry.storeJS(conn, obj.getJGeometry());
						istmt.setLong(1, obj.getId());
						istmt.setNString(2, obj.type());
						istmt.setObject(3, newObj);
						istmt.executeUpdate();
					}
				}
			}
			conn.commit();
			m_newObjects.clear();

			// Update updated objects <=> end of previous validity as update record + inserting new updated valid record
			for (MapObject obj : m_updatedObjects) {
				if (obj.type().equals("enclosure")) {
                                        // call procedure updateVybeh(n_cislo_vybehu IN NUMBER, n_validni_od IN DATE, n_typ IN VARCHAR, n_geometrie IN SDO_GEOMETRY)
                                        try (CallableStatement cstmt = conn.prepareCall("BEGIN updateVybeh(?, ?, ?, ?); END;")) {
						Enclosure enc = (Enclosure)obj;
                                                Struct newObj = JGeometry.storeJS(conn, enc.getJGeometry());
                                                cstmt.setLong(1, enc.getId());
                                                cstmt.setDate(2, new java.sql.Date(enc.getValid_from().getTime()));
                                                cstmt.setNString(3, enc.getType());
						cstmt.setObject(4, newObj);
						cstmt.execute();
					}
				}
				else {
                                        // updateMapovyObjekt(n_cislo_objektu IN NUMBER, n_validni_od IN DATE, n_typ IN VARCHAR, n_geometrie IN SDO_GEOMETRY)
					try (CallableStatement cstmt = conn.prepareCall("BEGIN updateMapovyObjekt(?, ?, ?, ?); END;")) {
                                                Struct newObj = JGeometry.storeJS(conn, obj.getJGeometry());
						cstmt.setLong(1, obj.getId());
                                                cstmt.setDate(2, new java.sql.Date(obj.getValid_from().getTime()));
                                                cstmt.setNString(3, obj.type());
                                                cstmt.setObject(4, newObj);
						cstmt.execute();
					}
				}
			}
			conn.commit();
			m_updatedObjects.clear();

			// Delete deleted objects <=> End of object temporal validity
			for (MapObject obj : m_deletedObjects) {
				if (obj.type().equals("enclosure")) {
                                        // call procedure deleteVybeh(n_cislo_vybehu IN NUMBER)
                                        try (CallableStatement cstmt = conn.prepareCall("BEGIN deleteVybeh(?); END;")) {
						cstmt.setLong(1, obj.getId());
						cstmt.execute();
					}
				}
				else {
					// call procedure deleteMapovyObjekt(n_cislo_objektu IN NUMBER)
                                        try (CallableStatement cstmt = conn.prepareCall("BEGIN deleteMapovyObjekt(?); END;")) {
						cstmt.setLong(1, obj.getId());
						cstmt.execute();
					}
				}
			}
			conn.commit();
			m_deletedObjects.clear();
		}
		catch (Exception e) {
			conn.rollback();
			throw e;
		}
		finally {
			conn.setAutoCommit(true);
		}
	}

	public void setButtonAddEnclosure(JButton buttonAddEnclosure)
	{
		m_buttonAddEnclosure = buttonAddEnclosure;
	}

	public void setButtonAddPark(JButton buttonAddPark)
	{
		m_buttonAddPark = buttonAddPark;
	}

	public void setButtonAddLake(JButton buttonAddLake)
	{
		m_buttonAddLake = buttonAddLake;
	}

	public void setButtonAddTree(JButton buttonAddTree)
	{
		m_buttonAddTree = buttonAddTree;
	}

	public void setButtonAddPath(JButton buttonAddPath)
	{
		m_buttonAddPath = buttonAddPath;
	}

	public void setButtonSave(JButton buttonSave)
	{
		m_buttonSave = buttonSave;
	}

	public void setButtonDistance(JButton buttonDistance)
	{
		m_buttonDistance = buttonDistance;
	}

	public void setButtonInside(JButton buttonInside)
	{
		m_buttonInside = buttonInside;
	}

	public void setButtonNeighbors(JButton buttonNeighbors)
	{
		m_buttonNeighbors = buttonNeighbors;
	}

	public void setButtonRelate(JButton buttonRelate)
	{
		m_buttonRelate = buttonRelate;
	}

        public void setButtonMostFood(JButton buttonMostFood)
	{
		m_buttonMostFood = buttonMostFood;
	}

	public void setLabelArea(JLabel labelArea)
	{
		m_labelArea = labelArea;
	}

	public void setLabelLength(JLabel labelLength)
	{
		m_labelLength = labelLength;
	}

	public void setTextFieldEnclosureType(JTextField textFieldEnclosureType)
	{
		m_textFieldEnclosureType = textFieldEnclosureType;
	}

	public void setListOfAnimals(JList<String> listOfAnimals)
	{
		m_listOfAnimals = new DefaultListModel<String>();
		listOfAnimals.setModel(m_listOfAnimals);
	}

	public void setDatePickerValidFrom(JXDatePicker datePickerValidFrom)
	{
		m_datePickerValidFrom = datePickerValidFrom;
                // set format, only date
                java.text.SimpleDateFormat format = new java.text.SimpleDateFormat("dd.MM.yyyy");
                final java.util.Date startDate = new java.util.Date(0);//01.01.1970
                format.set2DigitYearStart(startDate);
                m_datePickerValidFrom.setFormats(format);
	}

	public void setDatePickerValidTo(JXDatePicker datePickerValidTo)
	{
		m_datePickerValidTo = datePickerValidTo;

                // set format, only date
                java.text.SimpleDateFormat format = new java.text.SimpleDateFormat("dd.MM.yyyy");
                final java.util.Date startDate = new java.util.Date(0);//01.01.1970
                format.set2DigitYearStart(startDate);
                m_datePickerValidTo.setFormats(format);
	}

	/**
	 * Initializes listeners for all buttons.
	 */
	public void initilizeButtonListeners()
	{
		m_buttonAddEnclosure.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae)
			{
                                // New enclosure cannot be add to past configuration
                                if (m_validFrom != null || m_validTo != null) {
                                    JOptionPane.showMessageDialog(null, "It is not possible to add new enclosure.\n"
                                        + "Please remove values from datepickers and reload map with valid objects.",
                                        "Error - impossible change of past configuration", JOptionPane.ERROR_MESSAGE);
                                    return;
                                }
				if (m_addingNewObject) {
					if (m_pointsOfNewObject.size() > 2) {
						m_enclosureSeq++;
						Enclosure newObject = Enclosure.createEnclosure(m_enclosureSeq, m_pointsOfNewObject);
						m_groundObjects.add(newObject);
						m_newObjects.add(newObject);
					}
					else {
						JOptionPane.showMessageDialog(null, "Not enough points to create enclosure.", "Alert", JOptionPane.ERROR_MESSAGE);
					}

					m_addingNewObject = false;
					m_pointsOfNewObject.removeAllElements();
					m_buttonAddEnclosure.setText("Výběh");
					m_buttonAddPark.setEnabled(true);
					m_buttonAddLake.setEnabled(true);
					m_buttonAddTree.setEnabled(true);
					m_buttonAddPath.setEnabled(true);
				}
				else {
					m_addingNewObject = true;
					m_buttonAddEnclosure.setText("Dokončit výběh");
					m_buttonAddPark.setEnabled(false);
					m_buttonAddLake.setEnabled(false);
					m_buttonAddTree.setEnabled(false);
					m_buttonAddPath.setEnabled(false);

					if (m_focusedObject != null)
						unfocusObject();
				}

				repaint();
			}
		});

		m_buttonAddPark.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae)
			{
                                // New map object cannot be add to past configuration
                                if (m_validFrom != null || m_validTo != null) {
                                    JOptionPane.showMessageDialog(null, "It is not possible to add new park object.\n"
                                        + "Please remove values from datepickers and reload map with valid objects.",
                                        "Error - impossible change of past configuration", JOptionPane.ERROR_MESSAGE);
                                    return;
                                }
				if (m_addingNewObject) {
					if (m_pointsOfNewObject.size() > 2) {
						m_objectSeq++;
						Park newObject = Park.createPark(m_objectSeq, m_pointsOfNewObject);
						m_groundObjects.add(newObject);
						m_newObjects.add(newObject);
					}
					else {
						JOptionPane.showMessageDialog(null, "Not enough points to create park.", "Alert", JOptionPane.ERROR_MESSAGE);
					}

					m_addingNewObject = false;
					m_pointsOfNewObject.removeAllElements();
					m_buttonAddPark.setText("Park");
					m_buttonAddEnclosure.setEnabled(true);
					m_buttonAddLake.setEnabled(true);
					m_buttonAddTree.setEnabled(true);
					m_buttonAddPath.setEnabled(true);
				}
				else {
					m_addingNewObject = true;
					m_buttonAddPark.setText("Dokončit park");
					m_buttonAddEnclosure.setEnabled(false);
					m_buttonAddLake.setEnabled(false);
					m_buttonAddTree.setEnabled(false);
					m_buttonAddPath.setEnabled(false);

					if (m_focusedObject != null)
						unfocusObject();
				}

				repaint();
			}
		});

		m_buttonAddLake.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae)
			{
                                // New map object cannot be add to past configuration
                                if (m_validFrom != null || m_validTo != null) {
                                    JOptionPane.showMessageDialog(null, "It is not possible to add new lake object.\n"
                                        + "Please remove values from datepickers and reload map with valid objects.",
                                        "Error - impossible change of past configuration", JOptionPane.ERROR_MESSAGE);
                                    return;
                                }
				if (m_addingNewObject) {
					if (m_pointsOfNewObject.size() == 2) {
						m_objectSeq++;
						Lake newObject = new Lake(m_objectSeq, m_pointsOfNewObject.get(0), m_pointsOfNewObject.get(1));
						m_aboveObjects.add(newObject);
						m_newObjects.add(newObject);
					}
					else {
						JOptionPane.showMessageDialog(null, "To create lake is needed 2 points", "Alert", JOptionPane.ERROR_MESSAGE);
					}

					m_addingNewObject = false;
					m_pointsOfNewObject.removeAllElements();
					m_buttonAddLake.setText("Vodní plocha");
					m_buttonAddEnclosure.setEnabled(true);
					m_buttonAddPark.setEnabled(true);
					m_buttonAddTree.setEnabled(true);
					m_buttonAddPath.setEnabled(true);
				}
				else {
					m_addingNewObject = true;
					m_buttonAddLake.setText("Dokončit vodní plochu");
					m_buttonAddEnclosure.setEnabled(false);
					m_buttonAddPark.setEnabled(false);
					m_buttonAddTree.setEnabled(false);
					m_buttonAddPath.setEnabled(false);

					if (m_focusedObject != null)
						unfocusObject();
				}

				repaint();
			}
		});

		m_buttonAddTree.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae)
			{
                                // New map object cannot be add to past configuration
                                if (m_validFrom != null || m_validTo != null) {
                                    JOptionPane.showMessageDialog(null, "It is not possible to add new tree object.\n"
                                        + "Please remove values from datepickers and reload map with valid objects.",
                                        "Error - impossible change of past configuration", JOptionPane.ERROR_MESSAGE);
                                    return;
                                }
				if (m_addingNewObject) {
					if (m_pointsOfNewObject.size() == 1) {
						m_objectSeq++;
						Tree newObject = new Tree(m_objectSeq, m_pointsOfNewObject.get(0));
						m_aboveObjects.add(newObject);
						m_newObjects.add(newObject);
					}
					else {
						JOptionPane.showMessageDialog(null, "To create lake is needed 1 points", "Alert", JOptionPane.ERROR_MESSAGE);
					}

					m_addingNewObject = false;
					m_pointsOfNewObject.removeAllElements();
					m_buttonAddTree.setText("Strom");
					m_buttonAddEnclosure.setEnabled(true);
					m_buttonAddPark.setEnabled(true);
					m_buttonAddLake.setEnabled(true);
					m_buttonAddPath.setEnabled(true);
				}
				else {
					m_addingNewObject = true;
					m_buttonAddTree.setText("Dokončit strom");
					m_buttonAddEnclosure.setEnabled(false);
					m_buttonAddPark.setEnabled(false);
					m_buttonAddLake.setEnabled(false);
					m_buttonAddPath.setEnabled(false);

					if (m_focusedObject != null)
						unfocusObject();
				}

				repaint();
			}
		});

		m_buttonAddPath.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae)
			{
                                // New map object cannot be add to past configuration
                                if (m_validFrom != null || m_validTo != null) {
                                    JOptionPane.showMessageDialog(null, "It is not possible to add new path object.\n"
                                        + "Please remove values from datepickers and reload map with valid objects.",
                                        "Error - impossible change of past configuration", JOptionPane.ERROR_MESSAGE);
                                    return;
                                }
				if (m_addingNewObject) {
					if (m_pointsOfNewObject.size() >= 2) {
						m_objectSeq++;
						Path newObject = new Path(m_objectSeq, m_pointsOfNewObject);
						m_aboveObjects.add(newObject);
						m_newObjects.add(newObject);
					}
					else {
						JOptionPane.showMessageDialog(null, "To create path is needed at least two points", "Alert", JOptionPane.ERROR_MESSAGE);
					}

					m_addingNewObject = false;
					m_pointsOfNewObject.removeAllElements();
					m_buttonAddPath.setText("Cesta");
					m_buttonAddEnclosure.setEnabled(true);
					m_buttonAddPark.setEnabled(true);
					m_buttonAddLake.setEnabled(true);
					m_buttonAddTree.setEnabled(true);
				}
				else {
					m_addingNewObject = true;
					m_buttonAddPath.setText("Dokončit cestu");
					m_buttonAddEnclosure.setEnabled(false);
					m_buttonAddPark.setEnabled(false);
					m_buttonAddLake.setEnabled(false);
					m_buttonAddTree.setEnabled(false);

					if (m_focusedObject != null)
						unfocusObject();
				}

				repaint();
			}
		});

		m_buttonSave.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae)
			{
				try {
                                        // Changes could be applied to current map objects
                                        if (m_validFrom != null || m_validTo != null) {
                                            JOptionPane.showMessageDialog(null, "It is not possible to change or add map configuration.\n"
                                                + "Please remove values from datepickers and reload map for current objects.",
                                                "Error - impossible changes of past configuration", JOptionPane.ERROR_MESSAGE);
                                            return;
                                        }
					saveChangesToDatabase();
                                        loadFromDatabase();

					JOptionPane.showMessageDialog(null, "Changes was saved.", "Success", JOptionPane.INFORMATION_MESSAGE);
				}
				catch (Exception e) {
					System.err.println("Exception: " + e.getMessage());
					JOptionPane.showMessageDialog(null, "Error with databse: " + e.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
				}
			}
		});

		m_buttonDistance.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae)
			{
                                // Function of distance can be applied only to current valid objects
                                if (m_validFrom != null || m_validTo != null) {
                                    JOptionPane.showMessageDialog(null, "It is not possible to compute distance of past map configuration.\n"
                                        + "Please remove values from datepickers and reload map with valid objects.",
                                        "Error - distance in past is not supported", JOptionPane.ERROR_MESSAGE);
                                    return;
                                }
				if (m_addingNewObject || m_relateMode)
					return;

				m_distanceMode = true;
				m_modeObject = m_focusedObject;
			}
		});

		m_buttonRelate.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae)
			{
                                // Function of relation can be applied only to current valid objects
                                if (m_validFrom != null || m_validTo != null) {
                                    JOptionPane.showMessageDialog(null, "It is not possible to find object relation in past map configuration.\n"
                                        + "Please remove values from datepickers and reload map with valid objects.",
                                        "Error - relation between objects in past is not supported", JOptionPane.ERROR_MESSAGE);
                                    return;
                                }
				if (m_addingNewObject || m_distanceMode)
					return;

				m_relateMode = true;
				m_modeObject = m_focusedObject;
			}
		});

		m_buttonNeighbors.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae)
			{
                                // Function of neighbors can be applied only to current valid objects
                                if (m_validFrom != null || m_validTo != null) {
                                    JOptionPane.showMessageDialog(null, "It is not possible to find neighbors in past map configuration.\n"
                                        + "Please remove values from datepickers and reload map with valid objects.",
                                        "Error - finding neighbors in past is not supported", JOptionPane.ERROR_MESSAGE);
                                    return;
                                }
				if (m_addingNewObject || m_distanceMode || m_relateMode)
					return;

				if (m_focusedObject == null)
					return;

				Vector<MapObject> neighbors = null;
				try {
					neighbors = getNeighbors(m_focusedObject);
				}
				catch (Exception e) {
					System.err.println("Exception: " + e.getMessage());
							JOptionPane.showMessageDialog(null, "Error with databse: " + e.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
				}

				for (MapObject object : neighbors)
					object.highlightDrawRequest();

				repaint();
			}
		});

		m_buttonInside.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae)
			{
                                // Function of inside objects can be applied only to current valid objects
                                if (m_validFrom != null || m_validTo != null) {
                                    JOptionPane.showMessageDialog(null, "It is not possible to find nested objects in past map configuration.\n"
                                        + "Please remove values from datepickers and reload map with valid objects.",
                                        "Error - finding nested objects in past is not supported", JOptionPane.ERROR_MESSAGE);
                                    return;
                                }
				if (m_addingNewObject || m_distanceMode || m_relateMode)
					return;

				if (m_focusedObject == null)
					return;

				Vector<MapObject> insideObjects = null;
				try {
					insideObjects = getInsideObjects(m_focusedObject);
				}
				catch (Exception e) {
					System.err.println("Exception: " + e.getMessage());
							JOptionPane.showMessageDialog(null, "Error with databse: " + e.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
				}

				for (MapObject object : insideObjects)
					object.highlightDrawRequest();

				repaint();
			}
		});

                m_buttonMostFood.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae)
			{
                            if (m_focusedObject == null) return;
                            if (!m_focusedObject.type().equals("enclosure")) return;

                            // select for animals in focused enclosure
                            String sql = "SELECT druh, mnozstviKrmivaProKrmeni, typKrmiva, z.cislo_zvirete, z.jmeno FROM vybeh v, zvire z NATURAL JOIN druh_zvirete" +
                                        " WHERE v.cislo_vybehu = z.cislo_vybehu" +
                                        " AND v.cislo_vybehu=" + m_focusedObject.getId() +
                                        " AND v.validni_od = ?" +
                                        " AND z.validni_od >= v.validni_od";

                            // subselect for getting most food value
                            String subselect = " AND mnozstviKrmivaProKrmeni = (SELECT MAX(mnozstviKrmivaProKrmeni)" +
                                                                                " FROM vybeh vv, zvire zz NATURAL JOIN druh_zvirete" +
                                                                                " WHERE vv.cislo_vybehu = zz.cislo_vybehu" +
                                                                                " AND vv.cislo_vybehu = " + m_focusedObject.getId() +
                                                                                " AND vv.validni_od = ?" +
                                                                                " AND zz.validni_od >= vv.validni_od";

                            if (m_validFrom != null && m_validTo != null) {
                                // load animals that to is later that specified validTo
                                sql += " AND (z.validni_od < ? AND ((z.validni_do IS NULL AND v.validni_do IS NULL) OR z.validni_do > ?))";
                                sql += subselect;
                                sql += " AND (zz.validni_od < ? AND ((zz.validni_do IS NULL AND vv.validni_do IS NULL) OR zz.validni_do > ?)))";
                            }
                            else if (m_validFrom != null) { // only date_from was specified
                                // load animals that to is later that specified validTo
                                sql += " AND ((z.validni_do IS NULL AND v.validni_do IS NULL) OR z.validni_do > ?)";
                                sql += subselect;
                                sql += " AND ((zz.validni_do IS NULL AND vv.validni_do IS NULL) OR zz.validni_do > ?))";
                            }
                            else if (m_validTo != null) {  // only validTo was specified
                                // load animals that validity_from is lower that specified validTo
                                sql += " AND z.validni_od < ?";
                                sql += subselect;
                                sql += " AND zz.validni_od < ?)";
                            }
                            else {
                                // load valid current map
                                sql += " AND z.validni_do IS NULL";
                                sql += subselect;
                                sql += " AND zz.validni_do IS NULL)";
                            }
                            sql += " ORDER BY z.cislo_zvirete";


                            try {
                                Connection conn = OracleDBConnection.getConnection();
                                try (PreparedStatement stmt = conn.prepareStatement(sql)) {
                                    // add valid_from value
                                    stmt.setDate(1, new java.sql.Date(m_focusedObject.getValid_from().getTime()));

                                    // add temporality conditions(datepickers has been specified)
                                    if (m_validFrom != null && m_validTo != null) {
                                        stmt.setDate(2, new java.sql.Date(m_validTo.getTime()));
                                        stmt.setDate(3, new java.sql.Date(m_validFrom.getTime()));
                                        // subselect
                                        stmt.setDate(4, new java.sql.Date(m_focusedObject.getValid_from().getTime()));
                                        stmt.setDate(5, new java.sql.Date(m_validTo.getTime()));
                                        stmt.setDate(6, new java.sql.Date(m_validFrom.getTime()));
                                    }
                                    else if (m_validFrom != null) {
                                        stmt.setDate(2, new java.sql.Date(m_validFrom.getTime()));
                                        // subselect
                                        stmt.setDate(3, new java.sql.Date(m_focusedObject.getValid_from().getTime()));
                                        stmt.setDate(4, new java.sql.Date(m_validFrom.getTime()));
                                    }
                                    else if (m_validTo != null) {
                                        stmt.setDate(2, new java.sql.Date(m_validTo.getTime()));
                                        // subselect
                                        stmt.setDate(3, new java.sql.Date(m_focusedObject.getValid_from().getTime()));
                                        stmt.setDate(4, new java.sql.Date(m_validTo.getTime()));
                                    }
                                    else {
                                        // subselect
                                        stmt.setDate(2, new java.sql.Date(m_focusedObject.getValid_from().getTime()));
                                    }

                                    boolean empty = true;
                                    String maxFood = "";
                                    // execute query
                                    try (ResultSet rset = stmt.executeQuery()) {
                                        maxFood += "Nejvíce krmení v tomto výběhu spotřebuje:\n\n";
                                        while (rset.next()) {
                                            empty = false;
                                            // create string to be displayed
                                            maxFood += rset.getLong("cislo_zvirete") + "- " + rset.getNString("druh") + "(" + rset.getNString("jmeno") + ") : ";
                                            maxFood += rset.getNString("mnozstviKrmivaProKrmeni") + "kg krmiva typu '" + rset.getNString("typKrmiva") + "'\n\n";
                                        }

                                        if (empty)
                                            JOptionPane.showMessageDialog(null, "V tomto výběhu nejsou žádná zvířata.", "Info", JOptionPane.INFORMATION_MESSAGE);
                                        else
                                            JOptionPane.showMessageDialog(null, maxFood, "Info", JOptionPane.INFORMATION_MESSAGE);
                                    }
                                }
                            }
                            catch (Exception e) {
                                    System.err.println("Exception: " + e.getMessage());
                                    JOptionPane.showMessageDialog(null, "Error with database: " + e.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
                            }
			}
		});

		m_textFieldEnclosureType.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae)
			{
				if (m_focusedObject != null && m_focusedObject.type().equals("enclosure")) {
					Enclosure enc = (Enclosure)m_focusedObject;
					enc.setType(m_textFieldEnclosureType.getText());

					if (m_updatedObjects.indexOf(m_focusedObject) < 0 && m_newObjects.indexOf(m_focusedObject) < 0)
						m_updatedObjects.add(m_focusedObject);
				}
			}
		});
	}

	/**
	 * Initializes listeners for the mouse and keyboard.
	 */
	private void initilizeListeners()
	{
		addMouseListener(new MouseAdapter() {
			public void mousePressed(MouseEvent me)
			{
				requestFocusInWindow();

				// Adding new point to new object.
				if (m_addingNewObject) {
					m_pointsOfNewObject.add(me.getPoint());

					repaint();
					return;
				}
				// Finding out the distance between two objects.
				else if (m_distanceMode) {
					MapObject obj;
					try {
						obj = findObject(me.getPoint());
					}
					catch (IllegalStateException e) {
						// it was not clicked to any object.
						return;
					}

					if (m_modeObject == null) {
						focusObject(obj, me.getPoint());
						m_modeObject = m_focusedObject;
					}
					else {
						double distance = 0;
						try {
							distance = distance(m_modeObject, obj);
						}
						catch (Exception e) {
							System.err.println("Exception: " + e.getMessage());
							JOptionPane.showMessageDialog(null, "Error with databse: " + e.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
						}

						JOptionPane.showMessageDialog(null, "Vzdálenost objektů je " + String.format("%.2f", distance) + " m", "Info", JOptionPane.INFORMATION_MESSAGE);
						m_distanceMode = false;
						m_modeObject = null;
						unfocusObject();
					}

					repaint();
					return;
				}
				// Finding out the relation between two objects.
				else if (m_relateMode) {
					MapObject obj;
					try {
						obj = findObject(me.getPoint());
					}
					catch (IllegalStateException e) {
						// it was not clicked to any object.
						return;
					}

					if (m_modeObject == null) {
						focusObject(obj, me.getPoint());
						m_modeObject = m_focusedObject;
					}
					else {
						String relation = "";
						try {
							relation = relation(m_modeObject, obj);
						}
						catch (Exception e) {
							System.err.println("Exception: " + e.getMessage());
							JOptionPane.showMessageDialog(null, "Error with databse: " + e.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
						}

						JOptionPane.showMessageDialog(null, relation, "Info", JOptionPane.INFORMATION_MESSAGE);
						m_relateMode = false;
						m_modeObject = null;
						unfocusObject();
					}

					repaint();
					return;
				}

				// Check if it was not clicked to some object. If yes activate it.
				if (m_focusedObject == null) {
					try {
						focusObject(findObject(me.getPoint()), me.getPoint());
						repaint();
					}
					catch (IllegalStateException e) {
						// it was not clicked to any object.
					}

					return;
				}

				// Check if it was clicked to focused object
				try {
					m_focusedObject.press(me.getPoint());

					repaint();
					return;
				}
				catch (IllegalStateException e) {
					unfocusObject();
				}

				// It was not clicked to focused object then check if it was cliked to some another object.
				try {
					focusObject(findObject(me.getPoint()), me.getPoint());
					repaint();
					return;
				}
				catch (IllegalStateException e) {
					// it was not clicked to any object.
				}

				repaint();
			}
		});

		addMouseMotionListener(new MouseAdapter() {
			public void mouseDragged(MouseEvent me)
			{
				if (m_focusedObject != null) {
					m_focusedObject.drag(me.getPoint());

					if (m_updatedObjects.indexOf(m_focusedObject) < 0 && m_newObjects.indexOf(m_focusedObject) < 0)
						m_updatedObjects.add(m_focusedObject);
				}

				repaint();
			}
		});

		addKeyListener(new KeyAdapter() {
			public void keyPressed(KeyEvent ke)
			{
				if (ke.getKeyCode() == KeyEvent.VK_DELETE) {
					if (m_focusedObject == null)
						return;

                                        // Function of inside objects can be applied only to current valid objects
                                        if (m_validFrom != null || m_validTo != null) {
                                            JOptionPane.showMessageDialog(null, "It is not possible to delete objects from past map configuration.\n"
                                                + "Please remove values from datepickers and reload map with valid objects.",
                                                "Error - deleting objects in past is not supported", JOptionPane.ERROR_MESSAGE);
                                            return;
                                        }

                                        // check if any animals has been in selected enclosure
                                        try {
                                            Connection conn = OracleDBConnection.getConnection();
                                            // select all valid animals in selected enclosure
                                            try (PreparedStatement stmt = conn.prepareStatement("SELECT * FROM zvire " +
                                                                                        "WHERE cislo_vybehu = " + m_focusedObject.getId() + " AND validni_do IS NULL")) {
                                                try (ResultSet rset = stmt.executeQuery()) {
                                                    // if there are any animals -> error
                                                    while (rset.next()) {
                                                        JOptionPane.showMessageDialog(null, "Nelze smazat vybeh, kde se nachazeji zvirata!\n" +
                                                                    "Nejprve prestehujte zvirata do platneho vybehu.",
                                                                    "Error", JOptionPane.ERROR_MESSAGE);
                                                        return;
                                                    }
                                                }
                                            }
                                        }
                                        catch (Exception e) {
                                                System.err.println("Exception: " + e.getMessage());
                                                JOptionPane.showMessageDialog(null, "Error with database: " + e.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
                                        }

					int answer = JOptionPane.showConfirmDialog(null, "Do you want to delete it?", "Warning", JOptionPane.YES_NO_OPTION);

					if (answer != JOptionPane.YES_OPTION)
						return;

					boolean removeObject = m_focusedObject.delete();
					if (removeObject) { // deleting the focused object
						if (m_focusedObject instanceof Enclosure || m_focusedObject instanceof Park)
							m_groundObjects.remove(m_focusedObject);
						else if (m_focusedObject instanceof Lake || m_focusedObject instanceof Tree || m_focusedObject instanceof Path)
							m_aboveObjects.remove(m_focusedObject);

						m_deletedObjects.add(m_focusedObject);
						m_updatedObjects.remove(m_focusedObject);
						m_newObjects.remove(m_focusedObject);
						unfocusObject();
					}
				}

				repaint();
			}
		});
	}

	@Override
	public void paintComponent(Graphics g)
	{
		super.paintComponent(g);

		for (MapObject object : m_groundObjects)
			object.draw(g);

		boolean focusedDraw = false;
		if (m_focusedObject != null) {
			if (m_focusedObject instanceof Enclosure || m_focusedObject instanceof Park) {
				m_focusedObject.draw(g);
				focusedDraw = true;
			}
		}

		for (MapObject object : m_aboveObjects)
			object.draw(g);

		if (m_focusedObject != null || !focusedDraw) {
			if (m_focusedObject instanceof Lake || m_focusedObject instanceof Tree || m_focusedObject instanceof Path)
				m_focusedObject.draw(g);
		}

		// draw creating object
		for (int i = 0; i < m_pointsOfNewObject.size(); i++) {
			if (i == m_pointsOfNewObject.size() - 1) {
				g.drawOval(
					(int)m_pointsOfNewObject.get(i).getX() - (MapObject.RADIUS),
					(int)m_pointsOfNewObject.get(i).getY() - (MapObject.RADIUS),
					MapObject.RADIUS*2, MapObject.RADIUS*2);

				break;
			}

			g.drawLine(
				(int)m_pointsOfNewObject.get(i).getX(), (int)m_pointsOfNewObject.get(i).getY(),
				(int)m_pointsOfNewObject.get(i + 1).getX(), (int)m_pointsOfNewObject.get(i + 1).getY());

			g.drawOval(
				(int)m_pointsOfNewObject.get(i).getX() - (MapObject.RADIUS),
				(int)m_pointsOfNewObject.get(i).getY() - (MapObject.RADIUS),
				MapObject.RADIUS*2, MapObject.RADIUS*2);
		}

		// Draw measure
		g.drawLine(525,12, 625,12);
		g.drawLine(525,8, 525,16);
		g.drawLine(625,8, 625,16);
		g.drawString("100 m", 563, 25);
	}

	/**
	 * @param point point where was clicked by mouse.
	 * @return map object on which was clicked
	 * @throws IllegalStateException when it was not clicked to any object
	 */
	private MapObject findObject(Point point) throws IllegalStateException
	{
		for (MapObject object : m_aboveObjects) {
			if (object.isIn(point)) {
				return object;
			}
		}

		for (MapObject object : m_groundObjects) {
			if (object.isIn(point)) {
				return object;
			}
		}

		throw new IllegalStateException("it was not clicked to any object");
	}

	/**
	 * Focuses the object.
	 * @param object which is focusing
	 * @param point point where was clicked by mouse
	 */
	private void focusObject(MapObject object, Point point)
	{
		m_focusedObject = object;
		m_focusedObject.press(point);

		Connection conn = OracleDBConnection.getConnection();

		if (m_focusedObject.type().equals("enclosure")) {
			Enclosure enc = (Enclosure)m_focusedObject;
			m_textFieldEnclosureType.setText(enc.getType());

                        String sql = "SELECT druh, z.cislo_zvirete, z.jmeno FROM vybeh v, zvire z NATURAL JOIN druh_zvirete "
                                    + "WHERE v.cislo_vybehu = z.cislo_vybehu" +
                                            " AND v.cislo_vybehu=" + m_focusedObject.getId() +
                                            " AND v.validni_od = ?" +
                                            " AND z.validni_od >= v.validni_od ";

                        if (m_validFrom != null && m_validTo != null) {
                            // load animals that to is later that specified validTo
                            sql += "AND (z.validni_od < ? AND ((z.validni_do IS NULL AND v.validni_do IS NULL) OR z.validni_do > ?))";
                        }
                        else if (m_validFrom != null) { // only date_from was specified
                            // load animals that to is later that specified validTo
                            sql += "AND ((z.validni_do IS NULL AND v.validni_do IS NULL) OR z.validni_do > ?)";
                        }
                        else if (m_validTo != null) {  // only validTo was specified
                            // load animals that validity_from is lower that specified validTo
                            sql += "AND z.validni_od < ?";
                        }
                        else {
                            // load valid current map
                            sql += "AND z.validni_do IS NULL";
                        }

                        sql += " ORDER BY z.cislo_zvirete";

			try {
                                try (PreparedStatement stmt = conn.prepareStatement(sql)) {
                                        // add valid_from value
                                        stmt.setDate(1, new java.sql.Date(m_focusedObject.getValid_from().getTime()));

                                        // add temporality conditions(datepickers has been specified)
                                        if (m_validFrom != null && m_validTo != null) {
                                                stmt.setDate(2, new java.sql.Date(m_validTo.getTime()));
                                                stmt.setDate(3, new java.sql.Date(m_validFrom.getTime()));
                                        }
                                        else if (m_validFrom != null) {
                                                stmt.setDate(2, new java.sql.Date(m_validFrom.getTime()));
                                        }
                                        else if (m_validTo != null) {
                                                stmt.setDate(2, new java.sql.Date(m_validTo.getTime()));
                                        }

                                        // execute query
					try (ResultSet rset = stmt.executeQuery()) {
						while (rset.next()) {
                                                    // add animals to jList
                                                    m_listOfAnimals.addElement(rset.getLong(2) + " - " + rset.getNString(3) + " (" + rset.getNString(1) + ")");
						}
					}
				}
			}
			catch (Exception e) {
				System.err.println("Exception: " + e.getMessage());
				JOptionPane.showMessageDialog(null, "Error with databse: " + e.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
			}
		}

		String sql = "SELECT SDO_GEOM.SDO_AREA(geometrie, 0.5) plocha, SDO_GEOM.SDO_LENGTH(geometrie, 0.5) delka FROM ";
		if (m_focusedObject.type().equals("enclosure"))
			sql += "vybeh WHERE cislo_vybehu=" + m_focusedObject.getId();
		else
			sql += "mapovy_objekt WHERE cislo_objektu=" + m_focusedObject.getId();
                // add temporal condition
                sql += " AND validni_od = ?";

		double area = 0;
		double length = 0;
		try {
                        try (PreparedStatement stmt = conn.prepareStatement(sql)) {
                                // add valid_from value
                                stmt.setDate(1, new java.sql.Date(m_focusedObject.getValid_from().getTime()));

				try (ResultSet rset = stmt.executeQuery()) {
					while (rset.next()) {
						area = rset.getDouble(1);
						length = rset.getDouble(2);
					}
				}
			}
		}
		catch (Exception e) {
			System.err.println("Exception: " + e.getMessage());
			JOptionPane.showMessageDialog(null, "Error with databse: " + e.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
		}

		m_labelArea.setText(String.format("%.2f m2", area));
		m_labelLength.setText(String.format("%.2f m", length));
	}

	/**
	 * Unfocuses the focused object.
	 */
	private void unfocusObject()
	{
		if (m_focusedObject.type().equals("enclosure")) {
			m_textFieldEnclosureType.setText("");

			m_listOfAnimals.removeAllElements();
		}

		m_labelArea.setText("");
		m_labelLength.setText("");

		m_focusedObject.release();
		m_focusedObject = null;
	}

	/**
	 * Counts the distance between two objects.
	 * @param obj1
	 * @param obj2
	 * @return distance between the objects in meters
	 * @throws Exception
	 */
	private double distance(MapObject obj1, MapObject obj2) throws Exception
	{
		Connection conn = OracleDBConnection.getConnection();

		String sql = "SELECT SDO_GEOM.SDO_DISTANCE(o1.geometrie, o2.geometrie, 0.5) vz FROM ";
		if (obj1.type().equals("enclosure")) {
			sql += "vybeh o1, ";
			if (obj2.type().equals("enclosure"))
				sql += "vybeh o2 WHERE o1.cislo_vybehu=" + obj1.getId() + " AND o2.cislo_vybehu=" + obj2.getId();
			else
				sql += "mapovy_objekt o2 WHERE o1.cislo_vybehu=" + obj1.getId() + " AND o2.cislo_objektu=" + obj2.getId();
		}
		else {
			sql += "mapovy_objekt o1, ";
			if (obj2.type().equals("enclosure"))
				sql += "vybeh o2 WHERE o1.cislo_objektu=" + obj1.getId() + " AND o2.cislo_vybehu=" + obj2.getId();
			else
				sql += "mapovy_objekt o2 WHERE o1.cislo_objektu=" + obj1.getId() + " AND o2.cislo_objektu=" + obj2.getId();
		}
                // add validity condition - distance is supported only for current valid objects
                sql += " AND o1.validni_do IS NULL AND o2.validni_do IS NULL";

		double distance = 0;
		try (Statement stmt = conn.createStatement()) {
			try (ResultSet rset = stmt.executeQuery(sql)) {
				while (rset.next()) {
					distance = rset.getDouble(1);
				}
			}
		}

		return distance;
	}

	/**
	 * Finds out the relation between the given objects.
	 * @param obj1
	 * @param obj2
	 * @return relation between two given objects.
	 * @throws Exception
	 */
	private String relation(MapObject obj1, MapObject obj2) throws Exception
	{
		Connection conn = OracleDBConnection.getConnection();

		String sql = "SELECT SDO_GEOM.RELATE(o1.geometrie, 'determine' , o2.geometrie, 0.5) vz FROM ";
		if (obj1.type().equals("enclosure")) {
			sql += "vybeh o1, ";
			if (obj2.type().equals("enclosure"))
				sql += "vybeh o2 WHERE o1.cislo_vybehu=" + obj1.getId() + " AND o2.cislo_vybehu=" + obj2.getId();
			else
				sql += "mapovy_objekt o2 WHERE o1.cislo_vybehu=" + obj1.getId() + " AND o2.cislo_objektu=" + obj2.getId();
		}
		else {
			sql += "mapovy_objekt o1, ";
			if (obj2.type().equals("enclosure"))
				sql += "vybeh o2 WHERE o1.cislo_objektu=" + obj1.getId() + " AND o2.cislo_vybehu=" + obj2.getId();
			else
				sql += "mapovy_objekt o2 WHERE o1.cislo_objektu=" + obj1.getId() + " AND o2.cislo_objektu=" + obj2.getId();
		}
                // add validity condition - relation is supported only for current valid objects
                sql += " AND o1.validni_do IS NULL AND o2.validni_do IS NULL";

		String relation = "";
		try (Statement stmt = conn.createStatement()) {
			try (ResultSet rset = stmt.executeQuery(sql)) {
				while (rset.next()) {
					relation = rset.getNString(1);
				}
			}
		}

		if (relation.equals("CONTAINS") || relation.equals("COVERS") || relation.equals("COVEREDBY"))
			return "První objekt obsahuje druhý objekt.";
		else if (relation.equals("INSIDE"))
			return "První objekt je uvnitř druhého objektu.";
		else if (relation.equals("DISJOINT"))
			return "Objekty nemají žádný vztah.";
		else if (relation.equals("EQUAL"))
			return "První objekt je roven druhému objektu.";
		else if (relation.equals("OVERLAPBDYDISJOINT") || relation.equals("OVERLAPBDYINTERSECT"))
			return "První objekt překrývá druhý objekt.";
		else if (relation.equals("TOUCH"))
			return "První objekt se dotýká druhého objektu.";
		else if (relation.equals("ON"))
			return "První objekt je na druhém objektu.";


		return relation;
	}

	/**
	 * @param obj the object for which the neighbors are acquired
	 * @return map objects which are less than 20 meters away from the given object
	 * @throws SQLException when error with databse occures
	 */
	private Vector<MapObject> getNeighbors(MapObject obj) throws Exception
	{
		Vector<MapObject> neighbors = new Vector<MapObject>();

		Connection conn = OracleDBConnection.getConnection();

		TreeSet<Long> enclosureIds = new TreeSet<Long>();
		TreeSet<Long> objectIds = new TreeSet<Long>();
		if (obj.type().equals("enclosure")) {
			String sql1 = "SELECT o1.cislo_vybehu " +
						  "FROM vybeh o1, vybeh o2 " +
						  "WHERE o2.cislo_vybehu=" + obj.getId() + " " +
						  "AND SDO_NN(o1.geometrie, o2.geometrie, 'distance=20', 0.5)='TRUE' " +
						  "AND SDO_GEOM.SDO_DISTANCE(o1.geometrie, o2.geometrie, 0.5)>0 " +
						  "AND o1.cislo_vybehu<>o2.cislo_vybehu " +
                                                  "AND o1.validni_do IS NULL AND o2.validni_do IS NULL";

			try (Statement stmt = conn.createStatement()) {
				try (ResultSet rset = stmt.executeQuery(sql1)) {
					while (rset.next()) {
						enclosureIds.add(rset.getLong(1));
					}
				}
			}

			String sql2 = "SELECT o1.cislo_objektu " +
						  "FROM mapovy_objekt o1, vybeh o2 " +
						  "WHERE o2.cislo_vybehu=" + obj.getId() + " " +
						  "AND SDO_NN(o1.geometrie, o2.geometrie, 'distance=20', 0.5)='TRUE' " +
						  "AND SDO_GEOM.SDO_DISTANCE(o1.geometrie, o2.geometrie, 0.5)>0 " +
                                                  "AND o1.validni_do IS NULL AND o2.validni_do IS NULL";

			try (Statement stmt = conn.createStatement()) {
				try (ResultSet rset = stmt.executeQuery(sql2)) {
					while (rset.next()) {
						objectIds.add(rset.getLong(1));
					}
				}
			}
		}
		else {
			String sql1 = "SELECT o1.cislo_vybehu " +
						  "FROM vybeh o1, mapovy_objekt o2 " +
						  "WHERE o2.cislo_objektu=" + obj.getId() + " " +
						  "AND SDO_NN(o1.geometrie, o2.geometrie, 'distance=20', 0.5)='TRUE' " +
						  "AND SDO_GEOM.SDO_DISTANCE(o1.geometrie, o2.geometrie, 0.5)>0 " +
                                                  "AND o1.validni_do IS NULL AND o2.validni_do IS NULL";

			try (Statement stmt = conn.createStatement()) {
				try (ResultSet rset = stmt.executeQuery(sql1)) {
					while (rset.next()) {
						enclosureIds.add(rset.getLong(1));
					}
				}
			}

			String sql2 = "SELECT o1.cislo_objektu " +
						  "FROM mapovy_objekt o1, mapovy_objekt o2 " +
						  "WHERE o2.cislo_objektu=" + obj.getId() + " " +
						  "AND SDO_NN(o1.geometrie, o2.geometrie, 'distance=20', 0.5)='TRUE' " +
						  "AND SDO_GEOM.SDO_DISTANCE(o1.geometrie, o2.geometrie, 0.5)>0 " +
						  "AND o1.cislo_objektu<>o2.cislo_objektu " +
                                                  "AND o1.validni_do IS NULL AND o2.validni_do IS NULL";

			try (Statement stmt = conn.createStatement()) {
				try (ResultSet rset = stmt.executeQuery(sql2)) {
					while (rset.next()) {
						objectIds.add(rset.getLong(1));
					}
				}
			}
		}

		for (MapObject object : m_aboveObjects) {
			if (objectIds.contains(object.getId()))
				neighbors.addElement(object);
		}

		for (MapObject object : m_groundObjects) {
			if (object.type().equals("enclosure")) {
				if (enclosureIds.contains(object.getId()))
					neighbors.addElement(object);
			}
			else {
				if (objectIds.contains(object.getId()))
					neighbors.addElement(object);
			}
		}

		return neighbors;
	}

	/**
	 * @param obj the object for which the inside objects are acquired
	 * @return map objects which are inside the given object
	 * @throws SQLException when error with databse occures
	 */
	private Vector<MapObject> getInsideObjects(MapObject obj) throws Exception
	{
		Vector<MapObject> neighbors = new Vector<MapObject>();

		Connection conn = OracleDBConnection.getConnection();

		TreeSet<Long> enclosureIds = new TreeSet<Long>();
		TreeSet<Long> objectIds = new TreeSet<Long>();
		if (obj.type().equals("enclosure")) {
			String sql1 = "SELECT o1.cislo_vybehu " +
						  "FROM vybeh o1, vybeh o2 " +
						  "WHERE o2.cislo_vybehu=" + obj.getId() + " " +
						  "AND SDO_RELATE(o1.geometrie, o2.geometrie, 'mask=inside')='TRUE' " +
						  "AND o1.cislo_vybehu<>o2.cislo_vybehu " +
                                                  "AND o1.validni_do IS NULL AND o2.validni_do IS NULL";

			try (Statement stmt = conn.createStatement()) {
				try (ResultSet rset = stmt.executeQuery(sql1)) {
					while (rset.next()) {
						enclosureIds.add(rset.getLong(1));
					}
				}
			}

			String sql2 = "SELECT o1.cislo_objektu " +
						  "FROM mapovy_objekt o1, vybeh o2 " +
						  "WHERE o2.cislo_vybehu=" + obj.getId() + " " +
						  "AND SDO_RELATE(o1.geometrie, o2.geometrie, 'mask=inside')='TRUE' " +
                                                  "AND o1.validni_do IS NULL AND o2.validni_do IS NULL";

			try (Statement stmt = conn.createStatement()) {
				try (ResultSet rset = stmt.executeQuery(sql2)) {
					while (rset.next()) {
						objectIds.add(rset.getLong(1));
					}
				}
			}
		}
		else {
			String sql1 = "SELECT o1.cislo_vybehu " +
						  "FROM vybeh o1, mapovy_objekt o2 " +
						  "WHERE o2.cislo_objektu=" + obj.getId() + " " +
						  "AND SDO_RELATE(o1.geometrie, o2.geometrie, 'mask=inside')='TRUE' " +
                                                  "AND o1.validni_do IS NULL AND o2.validni_do IS NULL";

			try (Statement stmt = conn.createStatement()) {
				try (ResultSet rset = stmt.executeQuery(sql1)) {
					while (rset.next()) {
						enclosureIds.add(rset.getLong(1));
					}
				}
			}

			String sql2 = "SELECT o1.cislo_objektu " +
						  "FROM mapovy_objekt o1, mapovy_objekt o2 " +
						  "WHERE o2.cislo_objektu=" + obj.getId() + " " +
						  "AND SDO_RELATE(o1.geometrie, o2.geometrie, 'mask=inside')='TRUE' " +
						  "AND o1.cislo_objektu<>o2.cislo_objektu " +
                                                  "AND o1.validni_do IS NULL AND o2.validni_do IS NULL";

			try (Statement stmt = conn.createStatement()) {
				try (ResultSet rset = stmt.executeQuery(sql2)) {
					while (rset.next()) {
						objectIds.add(rset.getLong(1));
					}
				}
			}
		}

		for (MapObject object : m_aboveObjects) {
			if (objectIds.contains(object.getId()))
				neighbors.addElement(object);
		}

		for (MapObject object : m_groundObjects) {
			if (object.type().equals("enclosure")) {
				if (enclosureIds.contains(object.getId()))
					neighbors.addElement(object);
			}
			else {
				if (objectIds.contains(object.getId()))
					neighbors.addElement(object);
			}
		}

		return neighbors;
	}
}
