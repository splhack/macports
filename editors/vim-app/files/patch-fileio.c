--- src/fileio.c.orig
+++ src/fileio.c
@@ -358,6 +358,7 @@
 		pstate = &enc_table[j];
 		if (!pstate->enable || !pstate->check)
 		    continue;
+	#undef check
 		switch (pstate->check(pstate, d))
 		{
 		    case 0: /* keep "alive" state */
