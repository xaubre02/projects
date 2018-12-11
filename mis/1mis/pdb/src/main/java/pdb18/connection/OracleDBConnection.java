/* Connection package */
package pdb18.connection;

/* IMPORTS */
import pdb18.gui.multimedia.Picture;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.charset.Charset;

import java.sql.SQLException;
import java.sql.Statement;
import java.sql.Connection;
import java.sql.ResultSet;

import oracle.jdbc.pool.OracleDataSource;
import oracle.jdbc.OracleDriver;

import java.util.Arrays;
import java.util.List;


/**
 * The OracleDBConnection class handles the connection to the database.
 * Main function is connecting, reconnecting and disconnecting from the database.
 * 
 * @author Tomas Aubrecht (xaubre02@stud.fit.vutbr.cz)
 */
public class OracleDBConnection {

    private static OracleDataSource ods;
    private static       Connection conn;
    
    /**
     * Initialize the resources for connecting to a database.
     * 
     * @param args arguments for the OracleDriver
     */
    public static void init(String[] args) {
        try {
            OracleDriver.main(args);
            ods = new OracleDataSource();
            ods.setURL("jdbc:oracle:thin:@//gort.fit.vutbr.cz:1521/orclpdb.gort.fit.vutbr.cz");
        } catch (SQLException sqlEx) {
            System.err.println("SQLException: " + sqlEx.getMessage());
        } catch (Exception ex) {
            System.err.println("Exception: " + ex.getMessage());
        }
    }
    
    /**
     * Connect to the database using username and password.
     * 
     * @param user database account username
     * @param pass database account password
     * @return true if connected, else false
     */
    public static boolean connect(String user, String pass) {
        ods.setUser(user);
        ods.setPassword(pass);
        
        try {
            conn = ods.getConnection();
            return true;
        } catch (SQLException sqlEx) {
            return false;
        }
    }
    
    /**
     * Reconnect to the database using previously used credentials.
     * 
     * @return true if reconnected, else false
     */
    public static boolean reconnect() {
        try {
            conn = ods.getConnection();
            return true;
        } catch (SQLException sqlEx) {
            return false;
        }
    }
    
    /**
     * Disconnect from the database.
     * 
     * @return true if disconnected, else false 
     */
    public static boolean disconnect() {
        if (conn == null)
            return true;
        
        try {
            if (!conn.isClosed()) {
                conn.close();
            }
            return true;
        } catch (SQLException sqlEx) {
            return false;
        }
    }
    
    /**
     * Return the instance of the database connection.
     *
     * @return database connection
     */
    public static Connection getConnection() {
        try {
            if (!conn.isValid(2)) {
                reconnect();
            }
        } catch (SQLException sqlEx) {
            return null;
        }

        return conn;
    }
	
    /**
     * Initialize database by given initialization script defined by path.
     * @param scriptPath path to initialization script
     * @throws Exception when something goes wrong (database error, not found file)
     */
    public static void initDatabase(String scriptPath) throws Exception {
        InputStream script = new FileInputStream(scriptPath);
        InputStreamReader reader = new InputStreamReader(script, Charset.forName("UTF-8"));
        BufferedReader br = new BufferedReader(reader);
        List<String> queries;
        String sqlContent = "";
        String line;

        while ((line = br.readLine()) != null) 
        {
                sqlContent += line + " ";
        }
        queries = Arrays.asList(sqlContent.split("&&"));

        for (String query : queries) {
            if (query.contains("INSERT") || query.contains("CREATE") || query.contains("DELETE")) {
                System.out.println("'" + query + "'");
                try (Statement stmt = conn.createStatement()) {
                    try (ResultSet rset = stmt.executeQuery(query)) {
                    }
                }
            }
        }
    }

    /**
     * Load pictures of the animals and save them in the database.
     *
     * @throws SQLException SQL error
     * @throws IOException  I/O error
     */
    public static void initPictures()  throws SQLException, IOException {
        String pics[] = new String[10];
        pics[0] = "src/main/resources/pics/lion.gif";
        pics[1] = "src/main/resources/pics/antilope.gif";
        pics[2] = "src/main/resources/pics/deer.gif";
        pics[3] = "src/main/resources/pics/koala.gif";
        pics[4] = "src/main/resources/pics/koala2.gif";
        pics[5] = "src/main/resources/pics/panda.gif";
        pics[6] = "src/main/resources/pics/piranha.gif";
        pics[7] = "src/main/resources/pics/shark.gif";
        pics[8] = "src/main/resources/pics/tiger.gif";
        pics[9] = "src/main/resources/pics/tiger2.gif";

        Picture pic;
        for (int i = 0; i < 10; i++) {
            System.out.println("  -> SAVING IMAGE: " + pics[i]);
            pic = new Picture(getConnection(), i + 1);
            pic.saveToDB(pics[i]);
        }
    }
}
