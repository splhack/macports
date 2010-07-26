--- src/vim.h
+++ src/vim.h
@@ -479,6 +479,15 @@
 # define USE_AMBIWIDTH_AUTO
 #endif
 
+#if defined(FEAT_GUI_MAC)
+# if !defined(USE_IM_CONTROL)
+#  define USE_IM_CONTROL
+# endif
+# if !defined(USE_TRANSPARENCY)
+#  define USE_TRANSPARENCY
+# endif
+#endif
+
 /*
  * For dynamically loaded gettext library.  Currently, only for Win32.
  */
