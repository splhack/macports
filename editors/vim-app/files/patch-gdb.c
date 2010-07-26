--- src/gdb.c.orig
+++ src/gdb.c
@@ -1605,7 +1605,7 @@
     if (this->buf == NULL)
     {
 	p_mm = MAXMEM;	/* buffer max virtual memory */
-	(void)do_ecmd(0, name, NULL, NULL, ECMD_ONE, ECMD_HIDE);
+	(void)do_ecmd(0, name, NULL, NULL, ECMD_ONE, ECMD_HIDE, NULL);
 	p_mm = s_mm;
 
 	vim_free(name);			/* free name with vim_free */
@@ -1665,7 +1665,7 @@
     if (win_split(0, WSP_BOT) == OK)
     {
 	/* create the asm buffer empty */
-	(void)do_ecmd(0, NULL, NULL, NULL, ECMD_ONE, ECMD_HIDE);
+	(void)do_ecmd(0, NULL, NULL, NULL, ECMD_ONE, ECMD_HIDE, NULL);
 	gdb_as_setname(name);
 # ifdef FEAT_AUTOCMD
 	/* allow autocommands for loading syntax */
@@ -1972,7 +1972,7 @@
 	}
 
 	(void)do_ecmd(0, fpn, NULL, NULL, lnum,
-		      (P_HID(curbuf) ? ECMD_HIDE : 0) + ECMD_OLDBUF);
+		      (P_HID(curbuf) ? ECMD_HIDE : 0) + ECMD_OLDBUF, NULL);
     }
     else if (buf != curbuf)
 	set_curbuf(buf, DOBUF_GOTO);
