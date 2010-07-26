--- src/romaji.c.orig
+++ src/romaji.c
@@ -31,6 +31,13 @@
 #define ROMAJI_FIXKEY_XTU "xtu"
 #define ROMAJI_FIXKEY_NONXTU "aiueon"
 
+#if defined(__APPLE__)
+static int _isspace(int c) {
+	return c >= 0x80 ? 0 : isspace(c);
+}
+# define isspace(c) _isspace(c)
+#endif  // __APPLE__
+
 /*
  * romanode interfaces
  */
