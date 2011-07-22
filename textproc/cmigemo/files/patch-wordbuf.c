--- src/wordbuf.c.orig
+++ src/wordbuf.c
@@ -9,6 +9,7 @@
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
+#include <limits.h>
 #include "wordbuf.h"
 
 #define WORDLEN_DEF 64
