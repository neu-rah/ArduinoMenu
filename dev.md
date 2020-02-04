# development notes

## path reference

**a) using current position**

+direct index  
-root access must check NULL pointer  
-delivered to parent object
+no parent reference or rescan needed
+uses one less depth level
+non navigable items need not the extra level

**b) using length**

-every index must be decreased by one  
+root access on index 0
+delivered to focus object
-parent print needs parent reference or rescan
-uses an extra depth level
-non navigable items need not the extra level
