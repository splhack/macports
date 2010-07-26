--- src/syntax.c
+++ src/syntax.c
@@ -6144,6 +6144,9 @@
 	"Cursor guibg=fg guifg=bg",
 	"lCursor guibg=fg guifg=bg", /* should be different, but what? */
 #endif
+#ifdef USE_IM_CONTROL
+	"IMLine guibg=White guifg=Black",
+#endif
 	NULL
     };
 
