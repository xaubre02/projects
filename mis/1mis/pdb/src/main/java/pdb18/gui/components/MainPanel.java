/* Components package */
package pdb18.gui.components;

/**
 * The MainPanel is the wrapper for the application layout.
 * 
 * @author Tomas Aubrecht (xaubre02@stud.fit.vutbr.cz)
 */
public class MainPanel extends javax.swing.JPanel {

    /**
     * Create a new wrapper.
     */
    public MainPanel() {
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

        tabbedPane = new javax.swing.JTabbedPane();
        mapPanel = new pdb18.gui.databases.MapPanel();
        mmPanel = new pdb18.gui.databases.MMPanel();
        employeePanel = new pdb18.gui.databases.EmployeePanel();
        logoutPanel = new pdb18.gui.components.LogoutPanel();

        setMinimumSize(new java.awt.Dimension(1009, 770));

        tabbedPane.setBorder(new javax.swing.border.SoftBevelBorder(javax.swing.border.BevelBorder.RAISED));
        tabbedPane.setTabLayoutPolicy(javax.swing.JTabbedPane.SCROLL_TAB_LAYOUT);
        tabbedPane.setFocusable(false);
        tabbedPane.setFont(new java.awt.Font("Dialog", 1, 24)); // NOI18N
        tabbedPane.setMinimumSize(new java.awt.Dimension(1009, 770));
        tabbedPane.addTab("Mapa", mapPanel);

        mmPanel.addComponentListener(new java.awt.event.ComponentAdapter() {
            public void componentShown(java.awt.event.ComponentEvent evt) {
                mmPanelComponentShown(evt);
            }
        });
        tabbedPane.addTab("Zvířata", mmPanel);

        employeePanel.addComponentListener(new java.awt.event.ComponentAdapter() {
            public void componentShown(java.awt.event.ComponentEvent evt) {
                employeePanelComponentShown(evt);
            }
        });
        tabbedPane.addTab("Zaměstnanci", employeePanel);
        tabbedPane.addTab("Odhlásit", logoutPanel);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(tabbedPane, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(tabbedPane, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
        );
    }// </editor-fold>//GEN-END:initComponents

    private void mmPanelComponentShown(java.awt.event.ComponentEvent evt) {//GEN-FIRST:event_mmPanelComponentShown
        mmPanel.initContent();
    }//GEN-LAST:event_mmPanelComponentShown

    private void employeePanelComponentShown(java.awt.event.ComponentEvent evt) {//GEN-FIRST:event_employeePanelComponentShown
        employeePanel.initContent();
    }//GEN-LAST:event_employeePanelComponentShown

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private pdb18.gui.databases.EmployeePanel employeePanel;
    private pdb18.gui.components.LogoutPanel logoutPanel;
    private pdb18.gui.databases.MapPanel mapPanel;
    private pdb18.gui.databases.MMPanel mmPanel;
    private javax.swing.JTabbedPane tabbedPane;
    // End of variables declaration//GEN-END:variables
}
