--- src/gui_mac.m.orig
+++ src/gui_mac.m
@@ -21,6 +21,7 @@
 
 #include "vim.h"
 #import <Cocoa/Cocoa.h>
+#import <Carbon/Carbon.h>
 #import <PSMTabBarControl/PSMTabBarControl.h>
 
 #define GUI_MAC_DEBUG     1
@@ -102,7 +102,7 @@
 
 #define FF_Y(row)               (gui_mac.main_height - FILL_Y(row))
 #define FT_Y(row)               (gui_mac.main_height - TEXT_Y(row))
-#define VIM_BG_ALPHA            ((100 - p_transp) / 100.0)
+#define VIM_BG_ALPHA            (p_transp / 255.0)
 
 /* A simple view to make setting text area, scrollbar position inside 
  * vim window easier */
@@ -546,16 +547,24 @@
 
 /* Input Method Handling {{{ */
 
+#if defined(FEAT_UIMFEP)
+int gui_im_get_status()
+#else // FEAT_UIMFEP
 int im_get_status()
+#endif // FEAT_UIMFEP
 {
-    if (! gui.in_use)
-            return 0;
-
-    return 0;
+	SInt32 script = GetScriptManagerVariable(smKeyScript);
+	return (script != smRoman
+		&& script == GetScriptManagerVariable(smSysScript)) ? 1 : 0;
 }
 
+#if defined(FEAT_UIMFEP)
+void gui_im_set_active(int active)
+#else // FEAT_UIMFEP
 void im_set_active(int active)
+#endif // FEAT_UIMFEP
 {
+	KeyScript(active ? smKeySysScript : smKeyRoman);
 }
 
 void im_set_position(int row, int col)
@@ -1269,6 +1278,7 @@
 
 void gui_mch_new_colors()
 {
+    must_redraw = CLEAR;
 }
 
 guicolor_T gui_mch_get_color(char_u *name)
@@ -2835,6 +2844,13 @@
 
     if (markedRange.length > 0)
     {
+		int idx = syn_name2id((char_u *)"IMLine");
+		[self setMarkedTextAttribute:NSColorFromGuiColor(
+			highlight_gui_color_rgb(idx, TRUE), 1.0)
+			forKey:NSBackgroundColorAttributeName];
+		[self setMarkedTextAttribute:NSColorFromGuiColor(
+			highlight_gui_color_rgb(idx, FALSE), 1.0)
+			forKey:NSForegroundColorAttributeName];
     } else
     {
         // gui_mac_msg(MSG_DEBUG, @"clear markedText");
@@ -3244,6 +3260,8 @@
         default:
             if (vim_modifiers & MOD_MASK_CTRL)
             {
+				if (original_char == 'h' && [self hasMarkedText])
+					goto insert_text;
                 result[len++] = modified_char;
                 add_to_input_buf(result, len);
                 // gui_mac_msg(MSG_DEBUG, @"CTRL-%c, add_to_input_buf: %d", original_char, len);
