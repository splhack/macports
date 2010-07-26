--- Src/zsh.h.orig
+++ Src/zsh.h
@@ -2464,7 +2464,7 @@
 #define MB_METASTRLEN2(str, widthp)	mb_metastrlen(str, widthp)
 
 #ifdef BROKEN_WCWIDTH
-#define WCWIDTH(wc)	mk_wcwidth(wc)
+#define WCWIDTH(wc)	mk_wcwidth_cjk(wc)
 #else
 #define WCWIDTH(wc)	wcwidth(wc)
 #endif
