/* Components package */
package pdb18.gui.components;

/* IMPORTS */
import pdb18.connection.OracleDBConnection;
import pdb18.gui.AppGUI;

import java.sql.SQLException;
import java.io.IOException;

import javax.swing.JOptionPane;

/**
 * Logout panel of the application.
 * 
 * @author Tomas Aubrecht (xaubre02@stud.fit.vutbr.cz)
 */
public class LogoutPanel extends javax.swing.JPanel {

    /**
     * Create a new LogoutPanel.
     */
    public LogoutPanel() {
        initComponents();
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        enclosingPanel = new javax.swing.JPanel();
        logoutLabel = new javax.swing.JLabel();
        logoutButton = new javax.swing.JButton();
        logoutLabel1 = new javax.swing.JLabel();
        buttonInitDatabase = new javax.swing.JButton();

        setMinimumSize(new java.awt.Dimension(330, 140));
        setLayout(new java.awt.GridBagLayout());

        enclosingPanel.setBackground(new java.awt.Color(255, 255, 255));
        enclosingPanel.setBorder(new javax.swing.border.LineBorder(new java.awt.Color(29, 161, 242), 1, true));
        enclosingPanel.setMinimumSize(new java.awt.Dimension(385, 145));

        logoutLabel.setFont(new java.awt.Font("Dialog", 1, 18)); // NOI18N
        logoutLabel.setForeground(new java.awt.Color(68, 68, 68));
        logoutLabel.setText("Klikněte zde pro incializaci databáze");

        logoutButton.setFont(new java.awt.Font("Dialog", 1, 18)); // NOI18N
        logoutButton.setForeground(new java.awt.Color(68, 68, 68));
        logoutButton.setText("Odhlásit");
        logoutButton.setFocusPainted(false);
        logoutButton.setFocusable(false);
        logoutButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                logoutButtonActionPerformed(evt);
            }
        });
        logoutButton.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyReleased(java.awt.event.KeyEvent evt) {
                logoutButtonKeyReleased(evt);
            }
        });

        logoutLabel1.setFont(new java.awt.Font("Dialog", 1, 18)); // NOI18N
        logoutLabel1.setForeground(new java.awt.Color(68, 68, 68));
        logoutLabel1.setText("Klikněte zde pro odhlášení z databáze");

        buttonInitDatabase.setFont(new java.awt.Font("Dialog", 1, 18)); // NOI18N
        buttonInitDatabase.setForeground(new java.awt.Color(68, 68, 68));
        buttonInitDatabase.setText("Inicializuj databázi");
        buttonInitDatabase.setFocusable(false);
        buttonInitDatabase.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                buttonInitDatabaseActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout enclosingPanelLayout = new javax.swing.GroupLayout(enclosingPanel);
        enclosingPanel.setLayout(enclosingPanelLayout);
        enclosingPanelLayout.setHorizontalGroup(
            enclosingPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(enclosingPanelLayout.createSequentialGroup()
                .addGroup(enclosingPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(enclosingPanelLayout.createSequentialGroup()
                        .addContainerGap()
                        .addComponent(logoutLabel1))
                    .addGroup(enclosingPanelLayout.createSequentialGroup()
                        .addContainerGap()
                        .addComponent(logoutLabel))
                    .addGroup(enclosingPanelLayout.createSequentialGroup()
                        .addGap(118, 118, 118)
                        .addComponent(logoutButton))
                    .addGroup(enclosingPanelLayout.createSequentialGroup()
                        .addGap(83, 83, 83)
                        .addComponent(buttonInitDatabase)))
                .addContainerGap())
        );
        enclosingPanelLayout.setVerticalGroup(
            enclosingPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(enclosingPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(logoutLabel1)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(logoutButton)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(logoutLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(buttonInitDatabase, javax.swing.GroupLayout.PREFERRED_SIZE, 35, Short.MAX_VALUE)
                .addGap(16, 16, 16))
        );

        add(enclosingPanel, new java.awt.GridBagConstraints());
    }// </editor-fold>//GEN-END:initComponents

    private void logoutButtonKeyReleased(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_logoutButtonKeyReleased
        if (evt.getKeyCode() == java.awt.event.KeyEvent.VK_ENTER) {
            processLogout();
        }
    }//GEN-LAST:event_logoutButtonKeyReleased

    private void logoutButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_logoutButtonActionPerformed
        processLogout();
    }//GEN-LAST:event_logoutButtonActionPerformed

    private void buttonInitDatabaseActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_buttonInitDatabaseActionPerformed
        try {
            OracleDBConnection.initDatabase("init_database.txt");
            try {
                OracleDBConnection.initPictures();
            } catch (IOException | SQLException ex) {
                JOptionPane.showMessageDialog(this,
                        "Nepodařilo se nahrát obrázky do databáze: " + ex.getMessage(),
                        "SQL Chyba",
                        JOptionPane.ERROR_MESSAGE);
            }
            JOptionPane.showMessageDialog(null, "Inicializace databáze proběhla úspěšně", "Inicializace", JOptionPane.INFORMATION_MESSAGE);
        } catch (Exception e) {
            System.err.println("Exception: " + e.getMessage());
            JOptionPane.showMessageDialog(null, "Nepodařilo se inicializovat databázi: " + e.getMessage(), "Chyba", JOptionPane.ERROR_MESSAGE);
        }
    }//GEN-LAST:event_buttonInitDatabaseActionPerformed

    /**
     * Disconnect from the database and display login panel.
     */
    private void processLogout() {
        if (OracleDBConnection.disconnect()) {
            /* Get the main frame */
            AppGUI app = (AppGUI)javax.swing.SwingUtilities.getWindowAncestor(this);
            app.setLogged(false);
        } else {
            /* Show dialog message */
            javax.swing.JOptionPane.showMessageDialog(null, "Nepodařilo se odhlásit z databáze.", "Chyba", javax.swing.JOptionPane.ERROR_MESSAGE);
        }
    }

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton buttonInitDatabase;
    private javax.swing.JPanel enclosingPanel;
    private javax.swing.JButton logoutButton;
    private javax.swing.JLabel logoutLabel;
    private javax.swing.JLabel logoutLabel1;
    // End of variables declaration//GEN-END:variables
}
