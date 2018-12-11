/* Multimedia package */
package pdb18.gui.multimedia;

/**
 * This class checks whether the picture has been modified.
 * 
 * @author Tomas Aubrecht (xaubre02@stud.fit.vutbr.cz)
 */
public final class PictureChanges {
    /* Picture brightness parameters */
    public static final int PIC_BRIGHTEN =  1;
    public static final int PIC_DARKEN   = -1;
    
    /* PictureChanges attribues */
    private boolean vFlip;
    private boolean hFlip;
    private   float rotation;
    private     int brightness;

    /**
     * Construct a new PictureChanges object initialized as with no changes;
     */
    public PictureChanges() {
        this.cancel();
    }
    
    /**
     * Return true if the picture was modified, else false.
     * 
     * @return modification flag 
     */
    public boolean isChanged() {
        return vFlip || hFlip || rotation != 0.f || brightness != 0;
    }
    
    /**
     * Cancel the changes.
     */
    public void cancel() {
        vFlip      = false;
        hFlip      = false;
        rotation   = 0.f;
        brightness = 0;
    }
    
    /**
     * Get rollback operations in a single string.
     * 
     * @return rollback operations 
     */
    public String getRollbackOperations() {
        String ops = "";
        
        if (vFlip) {
            ops += " flip";
        }
        
        if (hFlip) {
            ops += " mirror";
        }
        
        if (rotation != 0) {
            ops += " rotate " + (-rotation);
        }
        
        if (brightness != 0) {
            float param = brightness > 0 ? 0.66f : 1.5f;
            for (int i = Math.abs(brightness); i > 0; i--) {
                ops += " gamma " + param;
            }
        }
        
        return ops;
    }
    
    /**
     * Save the change - vertical flip.
     */
    public void verticalFlip() {
        vFlip = !vFlip;
    }
    
    /**
     * Save the change - horizontal flip.
     */
    public void horizontalFlip() {
        hFlip = !hFlip;
    }
    
    /**
     * Save the change - rotation.
     * 
     * @param degrees rotated by number of degrees
     */
    public void rotation(float degrees) {
        rotation += degrees;
        rotation %= 360;
    }
    
    /**
     * Save the change - brightness.
     * 
     * @param change darken or brighten
     */
    public void brightness(int change) {
        brightness += change;
    }
}
