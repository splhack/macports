--- PTYSession.m
+++ PTYSession.m
@@ -245,7 +245,7 @@
     if ([env objectForKey:COLORFGBG_ENVNAME] == nil && COLORFGBG_VALUE != nil)
         [env setObject:COLORFGBG_VALUE forKey:COLORFGBG_ENVNAME];
 
-#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
+#if 0 //MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
     NSString* locale = [self _getLocale];
     if(locale != nil) {
         [env setObject:locale forKey:@"LANG"];
