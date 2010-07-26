--- PTYTextView.m
+++ PTYTextView.m
@@ -92,7 +92,7 @@ static BOOL tigerOrLater;
 
     [self setMarkedTextAttributes:
         [NSDictionary dictionaryWithObjectsAndKeys:
-            [NSColor yellowColor], NSBackgroundColorAttributeName,
+            [NSColor greenColor], NSBackgroundColorAttributeName,
             [NSColor blackColor], NSForegroundColorAttributeName,
             nafont, NSFontAttributeName,
             [NSNumber numberWithInt:2],NSUnderlineStyleAttributeName,
@@ -444,7 +444,7 @@ static BOOL tigerOrLater;
     nafont=naFont;
     [self setMarkedTextAttributes:
         [NSDictionary dictionaryWithObjectsAndKeys:
-            [NSColor yellowColor], NSBackgroundColorAttributeName,
+            [NSColor greenColor], NSBackgroundColorAttributeName,
             [NSColor blackColor], NSForegroundColorAttributeName,
             nafont, NSFontAttributeName,
             [NSNumber numberWithInt:2],NSUnderlineStyleAttributeName,
