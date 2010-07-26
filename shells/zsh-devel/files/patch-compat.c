--- Src/compat.c.orig
+++ Src/compat.c
@@ -777,6 +777,7 @@
 
   return width;
 }
+#endif /* 0 */
 
 
 /*
@@ -856,6 +857,7 @@
 }
 
 
+#if 0
 int mk_wcswidth_cjk(const wchar_t *pwcs, size_t n)
 {
   int w, width = 0;
