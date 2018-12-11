/* Multimedia package */
package pdb18.gui.multimedia;

/* IMPORTS */
import javax.swing.ImageIcon;
import java.io.IOException;

import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.sql.Connection;
import java.sql.ResultSet;

import oracle.jdbc.OraclePreparedStatement;
import oracle.jdbc.OracleResultSet;
import oracle.ord.im.OrdImage;

/**
 * The picture class that represents proxy of the OrdImage of an animal in the database. It provides methods for image manipulation.
 * 
 * @author Tomas Aubrecht (xaubre02@stud.fit.vutbr.cz)
 */
public class Picture {
    /* Image manipulation parameters */
    public static final int BRIGHTNESS_BRIGHTEN = 42;
    public static final int BRIGHTNESS_DARKEN = 43;
    
    public static final int FLIP_HORIZONTAL = 69;
    public static final int FLIP_VERTICAL = 70;
    
    /* SQL queries for image manipulation */
    private final String SQL_SELECT                 = "SELECT image FROM zvire WHERE cislo_zvirete = ?";
    private final String SQL_SELECT_FOR_UPDATE      = "SELECT image FROM zvire WHERE cislo_zvirete = ? FOR UPDATE";
    private final String SQL_UPDATE                 = "UPDATE zvire SET image = ? WHERE cislo_zvirete = ?";
    private final String SQL_UPDATE_STILLIMAGE      = "UPDATE zvire z SET z.image_si = SI_StillImage(z.image.getContent()) WHERE z.cislo_zvirete = ?";
    private final String SQL_UPDATE_STILLIMAGE_META = "UPDATE zvire SET image_ac = SI_AverageColor(image_si), image_ch = SI_ColorHistogram(image_si), image_pc = SI_PositionalColor(image_si), image_tx = SI_Texture(image_si) WHERE cislo_zvirete = ?";
    private final String SQL_DELETE                 = "UPDATE zvire SET image = ORDImage.init(), image_si = NULL, image_ac = NULL, image_ch = NULL, image_pc = NULL, image_tx = NULL  WHERE cislo_zvirete = ?";
    private final String SQL_MOST_SIMILAR           = "SELECT dst.cislo_zvirete, SI_ScoreByFtrList(new SI_FeatureList(src.image_ac,?,src.image_ch,?,src.image_pc,?,src.image_tx,?),dst.image_si) AS similarity FROM zvire src, zvire dst WHERE (src.cislo_zvirete = ?) AND (src.cislo_zvirete <> dst.cislo_zvirete) ORDER BY similarity ASC";

    /* Picture attribues such as database connection and the ID of the corresponding animal. */
    private final            int ID;
    private final     Connection conn;
    private final PictureChanges changes;
    private            ImageIcon imgIcon;
    private             OrdImage imgProxy;

    /**
     * Construct a new Picture object from the database connection and the corresponding animal ID.
     *
     * @param conn database connection
     * @param ID ID of the animal
     */
    public Picture(Connection conn, int ID) {
        this.ID = ID;
        this.conn = conn;
        this.imgIcon = null;
        this.imgProxy = null;
        this.changes = new PictureChanges();
    }

    /**
     * Returns the ImageIcon of the image in the database.
     *
     * @return ImageIcon of the animal
     * @throws SQLException SQL error
     * @throws IOException  I/O error
     */
    public ImageIcon getImageIcon() throws SQLException, IOException {
        // proxy image is not initialized
        if (imgProxy == null) {
            imgProxy = getProxy();
        }
        if (imgProxy != null) {
            if (imgIcon == null) {
                updateImageIcon();
            }
        }
        return imgIcon;
    }

    /**
     * Get the proxy image of the corresponding animal.
     *
     * @return image proxy
     * @throws SQLException SQL error
     */
    private OrdImage getProxy() throws SQLException {
        try (OraclePreparedStatement opstmt = (OraclePreparedStatement) conn.prepareStatement(SQL_SELECT)) {
            opstmt.setInt(1, ID);
            // get result
            try (OracleResultSet oresSet = (OracleResultSet) opstmt.executeQuery()) {
                if (oresSet.next()) {
                    return (OrdImage) oresSet.getORAData(1, OrdImage.getORADataFactory());
                }
                return null;
            }
        }
    }

    /**
     * Get the proxy image of the corresponding animal for update.
     *
     * @return image proxy locked for update
     * @throws SQLException SQL error
     */
    private OrdImage getProxyForUpdate() throws SQLException {
        try (OraclePreparedStatement opstmt = (OraclePreparedStatement) conn.prepareStatement(SQL_SELECT_FOR_UPDATE)) {
            opstmt.setInt(1, ID);
            // get result
            try (OracleResultSet oresSet = (OracleResultSet) opstmt.executeQuery()) {
                if (oresSet.next()) {
                    return (OrdImage) oresSet.getORAData(1, OrdImage.getORADataFactory());
                }
                return null;
            }
        }
    }

    /**
     * Update the image icon from the image proxy.
     *
     * @throws SQLException SQL error
     * @throws IOException  I/O error
     */
    private void updateImageIcon() throws SQLException, IOException {
        if (imgProxy == null) {
            return;
        }
        byte data[] = imgProxy.getDataInByteArray();
        if (data != null) {
            imgIcon = new ImageIcon(data);
        }
        else {
            imgIcon = null;
        }
    }
    
    /**
     * Check if the picture has been modified.
     * 
     * @return modification flag 
     */
    public boolean isModified() {
        return changes.isChanged();
    }
    
    /**
     * Changes are already saved - cancel changes.
     */
    public void saveChanges() {
        changes.cancel();
    }
    
    /**
     * Restore the image changes.
     *
     * @throws SQLException SQL error
     * @throws IOException  I/O error
     */
    public void restoreChanges() throws SQLException, IOException {
        // no changes has been made
        if (!changes.isChanged()) {
            return;
        }
        
        final boolean autoCommit = conn.getAutoCommit();
        conn.setAutoCommit(false);
        try {
            imgProxy = getProxyForUpdate();
            if (imgProxy != null) {
                // restore the image
                imgProxy.process(changes.getRollbackOperations());
                // cancel changes
                changes.cancel();
                conn.commit();
            }
        } finally {
            conn.setAutoCommit(autoCommit);
        }
        
        // unlock the row
        imgProxy = getProxy();
        // update the image icon
        updateImageIcon();
    }

    /**
     * Set the image brightness.
     *
     * @param brightness level of the brightness
     * @throws SQLException SQL error
     * @throws IOException  I/O error
     */
    public void changeBrightness(int brightness) throws SQLException, IOException {
        // no image available
        if (getImageIcon() == null) return;
        
        final boolean autoCommit = conn.getAutoCommit();
        conn.setAutoCommit(false);
        try {
            imgProxy = getProxyForUpdate();
            if (imgProxy != null) {
                // change the brightness of the the image
                switch (brightness) {
                    case BRIGHTNESS_BRIGHTEN: 
                        imgProxy.process("gamma 1.5");
                        changes.brightness(PictureChanges.PIC_BRIGHTEN);
                        break;
                    case BRIGHTNESS_DARKEN: 
                        imgProxy.process("gamma 0.66");
                        changes.brightness(PictureChanges.PIC_DARKEN);
                        break;
                    default:
                        return;
                }
                conn.commit();
            }
        } finally {
            conn.setAutoCommit(autoCommit);
        }
        
        // unlock the row
        imgProxy = getProxy();
        // update the image icon
        updateImageIcon();
    }

    /**
     * Flip the image in the provided direction.
     *
     * @param direction direction of the flip
     * @throws SQLException SQL error
     * @throws IOException  I/O error
     */
    public void flip(int direction) throws SQLException, IOException {
        // no image available
        if (getImageIcon() == null) return;
        
        final boolean autoCommit = conn.getAutoCommit();
        conn.setAutoCommit(false);
        try {
            imgProxy = getProxyForUpdate();
            if (imgProxy != null) {
                // flip the image
                switch (direction) {
                    case FLIP_HORIZONTAL:
                        imgProxy.process("mirror");
                        // save changes
                        changes.horizontalFlip();
                        break;
                    case FLIP_VERTICAL:
                        imgProxy.process("flip");
                        // save changes
                        changes.verticalFlip();
                        break;
                    default:
                        return;
                }
                conn.commit();
            }
        } finally {
            conn.setAutoCommit(autoCommit);
        }
        
        // unlock the row
        imgProxy = getProxy();
        // update the image icon
        updateImageIcon();
    }

    /**
     * Rotate the image by the provided degree.
     *
     * @param degrees degree of the rotation
     * @throws SQLException SQL error
     * @throws IOException  I/O error
     */
    public void rotate(float degrees) throws SQLException, IOException {
        // no image available
        if (getImageIcon() == null) return;
        
        final boolean autoCommit = conn.getAutoCommit();
        conn.setAutoCommit(false);
        try {
            imgProxy = getProxyForUpdate();
            if (imgProxy != null) {
                // rotate the image
                imgProxy.process("rotate " + degrees);
                // save changes
                changes.rotation(degrees);
                conn.commit();
            }
        } finally {
            conn.setAutoCommit(autoCommit);
        }
        
        // unlock the row
        imgProxy = getProxy();
        // update the image icon
        updateImageIcon();
    }

    /**
     * Delete the image from the database.
     *
     * @throws SQLException SQL error
     * @throws IOException  I/O error
     */
    public void deleteFromDB() throws SQLException, IOException {
        final boolean autoCommit = conn.getAutoCommit();
        conn.setAutoCommit(false);
        try {
            try (PreparedStatement pstmt = conn.prepareStatement(SQL_DELETE)){
                pstmt.setInt(1, ID);
                pstmt.executeUpdate();
                // cancel changes
                changes.cancel();
                conn.commit();
            }
        } finally {
            conn.setAutoCommit(autoCommit);
        }
        
        // unlock the row
        imgProxy = getProxy();
        updateImageIcon();
    }

    /**
     * Load an image of the animal from a local file and save it into the database.
     *
     * @param filename a file path to the image
     * @throws SQLException SQL error
     * @throws IOException  I/O error
     */
    public void saveToDB(String filename) throws SQLException, IOException {
        final boolean autoCommit = conn.getAutoCommit();
        conn.setAutoCommit(false);
        try {
            imgProxy = getProxyForUpdate();
            imgProxy.loadDataFromFile(filename);
            imgProxy.setProperties();
            insertIntoDB(imgProxy);
            // cancel changes
            changes.cancel();
            conn.commit();
        } finally {
            conn.setAutoCommit(autoCommit);
        }
        
        // unlock the row
        imgProxy = getProxy();
        updateImageIcon();
    }
    
    /**
     * Auxiliary method for inserting an image to the database.
     *
     * @param ordImg image to be inserted
     * @throws SQLException SQL error
     */
    private void insertIntoDB(OrdImage ordImg) throws SQLException {
        try (OraclePreparedStatement opstmt = (OraclePreparedStatement) conn.prepareStatement(SQL_UPDATE)) {
            opstmt.setORAData(1, ordImg);
            opstmt.setInt(2, ID);
            opstmt.executeUpdate();
        }
        // update STILL_IMAGE data
        updateStillImageData();
    }
    
    /**
     * Auxiliary method for updating the STILL_IMAGE and its properties(average color, color histogram, etc.).
     *
     * @throws SQLException SQL error
     */
    private void updateStillImageData() throws SQLException {
        // update STILL_IMAGE
        try (PreparedStatement pstmt = conn.prepareStatement(SQL_UPDATE_STILLIMAGE)){
            pstmt.setInt(1, ID);
            pstmt.executeUpdate();
        }
        // update STILL_IMAGE properties
        try (PreparedStatement pstmt = conn.prepareStatement(SQL_UPDATE_STILLIMAGE_META)) {
            pstmt.setInt(1, ID);
            pstmt.executeUpdate();
        }
    }
    
    /**
     * Find an animal in the database with the most similar image to this one.
     * 
     * @return ID of the animal with the most similar picture
     * @throws SQLException      SQL error
     */
    public int findTheMostSimilar() throws SQLException {
        try (PreparedStatement pstmt = conn.prepareStatement(SQL_MOST_SIMILAR)) {
            pstmt.setDouble(1, 0.7);
            pstmt.setDouble(2, 0.2);
            pstmt.setDouble(3, 0.1);
            pstmt.setDouble(4, 0.2);
            pstmt.setInt(5, ID);
            try (ResultSet resSet = pstmt.executeQuery()) {
                if (resSet.next()) {
                    return resSet.getInt(1);
                } else {
                    return -1;
                }
            }
        }
    }
    
    /**
     * Find an animal in the database with the most similar image to the specified file.
     *
     * @param filename a file path to the image to compare
     * @return ID of the animal with the most similar picture 
     * @throws SQLException SQL error
     * @throws IOException  I/O error
     */
    public int findTheMostSimilarToFile(String filename) throws SQLException, IOException {
        int animalID = Animal.getFirstAvailableID(conn, 1);
        Animal temp = new Animal(conn, animalID, 1, 1, "temp", new java.sql.Date(new java.util.Date().getTime()), null);
        int id = -1;
        try {
            temp.saveToDB();
            temp.getPicture().saveToDB(filename);
            id = temp.getPicture().findTheMostSimilar();
        } finally {
            temp.simpleDeleteFromDB();
        }
        return id;
    }
}
