--- src/mnode.c.orig
+++ src/mnode.c
@@ -33,6 +33,13 @@
 # define INLINE 
 #endif
 
+#if defined(__APPLE__)
+static int _isspace(int c) {
+	return c >= 0x80 ? 0 : isspace(c);
+}
+# define isspace(c) _isspace(c)
+#endif  // __APPLE__
+
 int n_mnode_new = 0;
 int n_mnode_delete = 0;
 
