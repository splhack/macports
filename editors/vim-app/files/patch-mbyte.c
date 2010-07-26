--- src/mbyte.c.orig
+++ src/mbyte.c
@@ -6194,13 +6194,27 @@
     void
 im_set_active(int active)
 {
+#  if defined(FEAT_GUI_MAC) || defined(FEAT_GUI_MACVIM)
+    if (gui.in_use)
+	gui_im_set_active(active);
+    else
+	uimfep_set_active(active);
+#  else // FEAT_GUI_MAC || FEAT_GUI_MACVIM
     uimfep_set_active(active);
+#  endif // FEAT_GUI_MAC || FEAT_GUI_MACVIM
 }
 
     int
 im_get_status(void)
 {
+#  if defined(FEAT_GUI_MAC) || defined(FEAT_GUI_MACVIM)
+    if (gui.in_use)
+	return gui_im_get_status();
+    else
+	return uimfep_get_status();
+#  else // FEAT_GUI_MAC || FEAT_GUI_MACVIM
     return uimfep_get_status();
+#  endif // FEAT_GUI_MAC || FEAT_GUI_MACVIM
 }
 # endif
 
