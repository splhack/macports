--- src/gui_mac.c.orig
+++ src/gui_mac.c
@@ -58,7 +58,8 @@
 SInt32 gMacSystemVersion;
 
 #ifdef MACOS_CONVERT
-# define USE_CARBONKEYHANDLER
+//# define USE_CARBONKEYHANDLER
+#ifdef USE_CARBONKEYHANDLER
 
 static int im_is_active = FALSE;
 #if 0
@@ -86,6 +87,7 @@
 static pascal OSStatus gui_mac_unicode_key_event(
 	EventHandlerCallRef nextHandler, EventRef theEvent);
 
+#endif	// USE_CARBONKEYHANDLER
 #endif
 
 
@@ -156,6 +158,13 @@
 /* Vector of char_u --> control index for hotkeys in dialogs */
 static short *gDialogHotKeys;
 
+/* INLINE IM */
+#ifdef USE_IM_CONTROL
+TSMDocumentID s_TSMDocID = 0;
+TSMTERecHandle s_TSMTERecHandle = 0;
+TEHandle s_TEHandle = 0;
+#endif /* USE_IM_CONTROL */
+
 static struct
 {
     FMFontFamily family;
@@ -167,6 +176,7 @@
 #ifdef MACOS_CONVERT
 # define USE_ATSUI_DRAWING
 int	    p_macatsui_last;
+int     p_antialias_last = -1;
 ATSUStyle   gFontStyle;
 # ifdef FEAT_MBYTE
 ATSUStyle   gWideFontStyle;
@@ -301,8 +311,14 @@
 #ifdef USE_ATSUI_DRAWING
 static void gui_mac_set_font_attributes(GuiFont font);
 static void gui_mac_dispose_atsui_style(void);
+static void gui_mac_change_antialias();
 #endif
 
+#ifdef USE_IM_CONTROL
+static pascal void tsmproc_pre(TEHandle textH, SInt32 refCon);
+static pascal void tsmproc_post(TEHandle textH, SInt32 fixLen, SInt32 inputAreaStart, SInt32 inputAreaEnd, SInt32 pinStart, SInt32 pinEnd, SInt32 refCon);
+#endif /* USE_IM_CONTROL */
+
 /*
  * ------------------------------------------------------------
  * Conversion Utility
@@ -1975,11 +1991,26 @@
     {
 	ControlRef rootControl;
 	GetRootControl(gui.VimWindow, &rootControl);
-	if ((event->modifiers) & activeFlag)
+	if ((event->modifiers) & activeFlag) {
 	    ActivateControl(rootControl);
-	else
+#ifdef USE_IM_CONTROL
+	    /* INLINE IM */
+	    if (s_TSMDocID) {
+	        TEActivate(s_TEHandle);
+	        ActivateTSMDocument(s_TSMDocID);
+	    }
+#endif /* USE_IM_CONTROL */
+	} else {
 	    DeactivateControl(rootControl);
+#ifdef USE_IM_CONTROL
+	    if (s_TSMDocID) {
+	        FixTSMDocument(s_TSMDocID);
+	        DeactivateTSMDocument(s_TSMDocID);
+	        TEDeactivate(s_TEHandle);
+	    }
+#endif /* USE_IM_CONTROL */
     }
+	}
 
     /* Activate */
     gui_focus_change((event->modifiers) & activeFlag);
@@ -3132,6 +3163,38 @@
     gui.scrollbar_height = gui.scrollbar_width = 15; /* cheat 1 overlap */
     gui.border_offset = gui.border_width = 2;
 
+    /* INLINE IM */
+#ifdef USE_IM_CONTROL
+    if (!s_TSMDocID)
+    {
+	OSErr		    err;
+	Rect		    rect;
+	InterfaceTypeList   supportedInterfaces;
+
+	memset(&rect, 0, sizeof(rect)); /* Just dummy initialization */
+	s_TEHandle = TEStyleNew(&rect, &rect);
+	supportedInterfaces[0] = kTSMTEInterfaceType;
+	err = NewTSMDocument(1, supportedInterfaces, &s_TSMDocID,
+		(SInt32)&s_TSMTERecHandle);
+	if (err == noErr && s_TSMDocID && s_TSMTERecHandle)
+	{
+	    TSMTERecPtr prec = *(s_TSMTERecHandle);
+
+	    prec->textH		    = s_TEHandle;
+	    prec->preUpdateProc	    = NewTSMTEPreUpdateUPP(tsmproc_pre);
+	    prec->postUpdateProc    = NewTSMTEPostUpdateUPP(tsmproc_post);
+	    prec->updateFlag	    = kTSMTEAutoScroll;
+	    prec->refCon	    = (SInt32)gui.VimWindow;
+
+	    err = UseInputWindow(s_TSMDocID, FALSE);
+	    if (err != noErr)
+		printf("UseInputWindow() error %d\n", err);
+	}
+	else
+	    printf("NewTSMDocument() error %d\n", err);
+    }
+#endif /* USE_IM_CONTROL */
+
     /* If Quartz-style text anti aliasing is available (see
        gui_mch_draw_string() below), enable it for all font sizes. */
     vim_setenv((char_u *)"QDTEXT_MINSIZE", (char_u *)"1");
@@ -3227,6 +3290,9 @@
 gui_mch_open(void)
 {
     ShowWindow(gui.VimWindow);
+#ifdef USE_TRANSPARENCY
+    gui_mch_set_transparency(p_transparency);
+#endif // USE_TRANSPARENCY
 
     if (gui_win_x != -1 && gui_win_y != -1)
 	gui_mch_set_winpos(gui_win_x, gui_win_y);
@@ -3260,6 +3326,22 @@
     void
 gui_mch_exit(int rc)
 {
+    /* INLINE IM */
+#ifdef USE_IM_CONTROL
+    if (s_TSMDocID)
+    {
+	OSErr err;
+
+	err = DeactivateTSMDocument(s_TSMDocID);
+	if (err != noErr)
+	    printf("DeactivateTSMDocument() error %d\n", err);
+	err = DeleteTSMDocument(s_TSMDocID);
+	if (err != noErr)
+	    printf("DeleteTSMDocument() error %d\n", err);
+	s_TSMDocID = NULL;
+    }
+#endif
+
     /* TODO: find out all what is missing here? */
     DisposeRgn(cursorRgn);
 
@@ -3369,6 +3451,45 @@
     *screen_h = screenRect.bottom - 40;
 }
 
+#ifdef USE_ATSUI_DRAWING
+    static void 
+gui_mac_change_antialias()
+{
+    ATSStyleRenderingOptions	renderingOptions;
+    
+    if (gMacSystemVersion >= 0x1020) {
+	renderingOptions	= p_antialias ? kATSStyleApplyAntiAliasing : kATSStyleNoAntiAliasing;;
+    } else {
+	renderingOptions	= kATSStyleNoOptions;
+    }
+
+    ATSUAttributeTag attribTags[] =
+    {
+	kATSUStyleRenderingOptionsTag,
+    };
+
+    ByteCount attribSizes[] =
+    {
+	sizeof(ATSStyleRenderingOptions),
+    };
+
+    ATSUAttributeValuePtr attribValues[] =
+    {
+	&renderingOptions,
+    };
+
+    if (gFontStyle)
+	ATSUSetAttributes(gFontStyle,
+	    (sizeof attribTags) / sizeof(ATSUAttributeTag),
+		attribTags, attribSizes, attribValues);
+    if (gWideFontStyle)
+	ATSUSetAttributes(gWideFontStyle,
+	    (sizeof attribTags) / sizeof(ATSUAttributeTag),
+		attribTags, attribSizes, attribValues);
+
+    p_antialias_last = p_antialias;
+}
+#endif
 
 /*
  * Open the Font Panel and wait for the user to select a font and
@@ -4245,6 +4366,9 @@
 	/* switch from nomacatsui to macatsui */
 	gui_mac_create_atsui_style();
 
+    if (p_antialias != p_antialias_last)
+	gui_mac_change_antialias();
+
     if (p_macatsui)
 	draw_string_ATSUI(row, col, s, len, flags);
     else
@@ -6352,10 +6476,12 @@
 #endif
 }
 
-#if (defined(USE_IM_CONTROL) || defined(PROTO)) && defined(USE_CARBONKEYHANDLER)
+#if (defined(USE_IM_CONTROL) || defined(PROTO))
 /*
  * Input Method Control functions.
  */
+static int im_row = 0;
+static int im_col = 0;
 
 /*
  * Notify cursor position to IM.
@@ -6363,13 +6489,11 @@
     void
 im_set_position(int row, int col)
 {
-#if 0
-    /* TODO: Implement me! */
-    im_start_row = row;
-    im_start_col = col;
-#endif
+    im_row = row;
+    im_col = col;
 }
 
+#if defined(USE_CARBONKEYHANDLER)
 static ScriptLanguageRecord gTSLWindow;
 static ScriptLanguageRecord gTSLInsert;
 static ScriptLanguageRecord gTSLDefault = { 0, 0 };
@@ -6429,78 +6553,127 @@
 	DeactivateTSMDocument(gTSMDocument);
     }
 }
+#endif	// USE_CARBONKEYHANDLER
 
 /*
  * Set IM status on ("active" is TRUE) or off ("active" is FALSE).
  */
     void
+#if defined(FEAT_UIMFEP)
+gui_im_set_active(int active)
+#else // FEAT_UIMFEP
 im_set_active(int active)
+#endif // FEAT_UIMFEP
 {
-    ScriptLanguageRecord *slptr = NULL;
-    OSStatus err;
+    KeyScript(active ? smKeySysScript : smKeyRoman);
+}
 
-    if (! gui.in_use)
-	return;
+/*
+ * Get IM status.  When IM is on, return not 0.  Else return 0.
+ */
+    int
+#if defined(FEAT_UIMFEP)
+gui_im_get_status(void)
+#else // FEAT_UIMFEP
+im_get_status(void)
+#endif // FEAT_UIMFEP
+{
+    SInt32 script = GetScriptManagerVariable(smKeyScript);
+    return (script != smRoman
+	    && script == GetScriptManagerVariable(smSysScript)) ? 1 : 0;
+}
 
-    if (im_initialized == 0)
-    {
-	im_initialized = 1;
+static RGBColor s_saved_fg;
+static RGBColor s_saved_bg;
 
-	/* save default TSM component (should be U.S.) to default */
-	GetDefaultInputMethodOfClass(&gTSCDefault, &gTSLDefault,
-				     kKeyboardInputMethodClass);
-    }
+/* INLINE IM */
+    static pascal void
+tsmproc_pre(TEHandle textH, SInt32 refCon)
+{
+    Rect rect;
+    TextStyle style;
+    RGBColor fore;
+	int idx = syn_name2id((char_u *)"IMLine");
 
-    if (active == TRUE)
+    /* Save current color and set IM color */
+    GetForeColor(&s_saved_fg);
+    GetBackColor(&s_saved_bg);
+    gui_mch_set_fg_color(highlight_gui_color_rgb(idx, TRUE));
+    gui_mch_set_bg_color(highlight_gui_color_rgb(idx, FALSE));
+    GetForeColor(&fore);
+    style.tsColor = fore;
+
+    /* Set IME font and color */
+    style.tsFont = gui.norm_font & 0xFFFF;
+    style.tsSize = gui.norm_font >> 16;
+    TESetStyle(doFont | doSize | doColor, &style, false, textH);
+
+    /* Set IME's rectangle */
     {
-	im_is_active = TRUE;
-	ActivateTSMDocument(gTSMDocument);
-	slptr = &gTSLInsert;
+	int row = im_row, col = im_col;
 
-	if (slptr)
+	if (col * 4 > gui.num_cols * 3)
 	{
-	    err = SetDefaultInputMethodOfClass(gTSCInsert, slptr,
-					       kKeyboardInputMethodClass);
-	    if (err == noErr)
-		err = SetTextServiceLanguage(slptr);
-
-	    if (err == noErr)
-		KeyScript(slptr->fScript | smKeyForceKeyScriptMask);
+	    /* Slide inline area to 1 line down or up */
+	    if (row > gui.num_rows / 2)
+		--row;
+	    else
+		++row;
+	    col = 0;
 	}
+	rect.top	= FILL_Y(row);
+	rect.left	= FILL_X(col);
+	rect.right	= FILL_X(screen_Columns);
+	rect.bottom	= rect.top + gui.char_height;
+	(**textH).destRect = rect;
+	(**textH).viewRect = rect;
+	TECalText(textH);
     }
-    else
-    {
-	err = GetTextServiceLanguage(&gTSLInsert);
-	if (err == noErr)
-	    slptr = &gTSLInsert;
+}
 
-	if (slptr)
-	    GetDefaultInputMethodOfClass(&gTSCInsert, slptr,
-					 kKeyboardInputMethodClass);
+    static pascal void
+tsmproc_post(TEHandle textH, SInt32 fixLen, SInt32 inputAreaStart,
+	SInt32 inputAreaEnd, SInt32 pinStart, SInt32 pinEnd, SInt32 refCon)
+{
+    char_u	*to = NULL;
+    vimconv_T	conv;
+    int		convlen = 0;
 
-	/* restore to default when switch to normal mode, so than we could
-	 * enter commands easier */
-	SetDefaultInputMethodOfClass(gTSCDefault, &gTSLDefault,
-				     kKeyboardInputMethodClass);
-	SetTextServiceLanguage(&gTSLDefault);
+    Rect rect = (**textH).viewRect;
 
-	im_is_active = FALSE;
-	DeactivateTSMDocument(gTSMDocument);
-    }
-}
+    /* Restore color */
+    RGBForeColor(&s_saved_fg);
+    RGBBackColor(&s_saved_bg);
+
+    /* Add IME's result string to input buffer */
+    if (fixLen > 0)
+    {
+	CharsHandle ch = TEGetText(textH);
+	conv.vc_type = CONV_NONE;
+	convlen = fixLen;
+	convert_setup(&conv, (char_u *)"sjis", p_enc? p_enc: (char_u *)"utf-8");
+	to = string_convert(&conv, (char_u *)*ch, &convlen);
+	if (to != NULL)
+	    add_to_input_buf_csi(to, convlen); 
+       	else
+	    add_to_input_buf_csi(*ch, fixLen); 
 
-/*
- * Get IM status.  When IM is on, return not 0.  Else return 0.
- */
-    int
-im_get_status(void)
-{
-    if (! gui.in_use)
-	return 0;
+	if (conv.vc_type != CONV_NONE)
+	    vim_free(to);
+	convert_setup(&conv, NULL, NULL);
+    }
 
-    return im_is_active;
+    if (inputAreaEnd < 0)
+    {
+	TESetText("", 0, textH);
+	/* Remove TSMTE garbage */
+	gui_redraw(rect.left, rect.top, rect.right - rect.left + 1,
+		rect.bottom - rect.top + 1);
+	gui_update_cursor(TRUE, FALSE);
+    }
 }
 
+
 #endif /* defined(USE_IM_CONTROL) || defined(PROTO) */
 
 
@@ -6904,3 +7077,11 @@
 }
 
 #endif // FEAT_GUI_TABLINE
+
+#if defined(USE_TRANSPARENCY)
+    void
+gui_mch_set_transparency(int alpha)
+{
+    SetWindowAlpha(gui.VimWindow, (float)alpha / 255.0f);
+}
+#endif	// USE_TRANSPARENCY
