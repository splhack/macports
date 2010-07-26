--- src/option.c.orig
+++ src/option.c
@@ -2557,13 +2557,13 @@
 			    (char_u *)&p_tbis, PV_NONE,
 			    {(char_u *)"small", (char_u *)0L}},
 #endif
-    {"transparency",   "transp",  P_NUM|P_VIM|P_RCLR,
+    {"transparency",   "transp",  P_NUM|P_VI_DEF,
 #if defined(FEAT_GUI_COCOA)
                             (char_u *)&p_transp, PV_NONE,
 #else
                             (char_u *)NULL, PV_NONE,
 #endif
-                            {(char_u *)0L, (char_u *)0L} },
+                            {(char_u *)255L, (char_u *)0L} },
     {"ttimeout",    NULL,   P_BOOL|P_VI_DEF|P_VIM,
 			    (char_u *)&p_ttimeout, PV_NONE,
 			    {(char_u *)FALSE, (char_u *)0L}},
@@ -8004,15 +8004,14 @@
 #endif
 
 #if defined(FEAT_GUI_COCOA)
-     /* 'transparency' is a number between 0 and 100 */
+     /* 'transparency' is a number between 1 and 255 */
     else if (pp == &p_transp)
     {
-        if (p_transp < 0 || p_transp > 100)
+        if (p_transp < 1 || p_transp > 255)
         {
-            errmsg = e_invarg;
-            p_transp = old_value;
+            p_transp = 255;
         }
-        else if (gui.in_use)
+        if (gui.in_use)
             gui_mch_new_colors();
     }
 #endif
