### BUILDING MEDNAFFE ON WINDOWS [OBSOLETE]###

#### Dependencies ####

To build mednaffe on Windows you need [MinGW](http://www.mingw.org/). You can download this installer: [mingw-get-inst-20120426.exe](http://sourceforge.net/projects/mingw/files/Installer/mingw-get-inst/mingw-get-inst-20120426/mingw-get-inst-20120426.exe/download).

Also you need [GTK+ development files for Windows](http://www.gtk.org/download/win32.php). You want this: [gtk+-bundle\_2.24.10-20120208\_win32.zip](http://ftp.gnome.org/pub/gnome/binaries/win32/gtk+/2.24/gtk+-bundle_2.24.10-20120208_win32.zip).

Install MinGW in C:\MinGW\ and decompress GTK+ files in C:\gtk\ . Also decompress [mednaffe sources](https://sites.google.com/site/amatcoder/mednaffe/downloads/mednaffe-0.6.tar.gz?attredirects=0&d=1) in C:\mednaffe\

#### Compiling ####

Then you can follow these steps:

1) Open a Command Prompt window (cmd.exe)

2) Type (or copy&paste):
> `cd C:\mednaffe\src\`

3) Type (or copy&paste):
> `C:\MinGW\bin\gcc -O2 -std=c99 -Wall -DGTK2_ENABLED -mwindows -mms-bitfields -o mednaffe mednaffe.c active.c command.c gui.c prefs.c list.c toggles.c about.c -IC:/gtk/include/gtk-2.0 -IC:/gtk/lib/gtk-2.0/include -IC:/gtk/include/atk-1.0 -IC:/gtk/include/cairo -IC:/gtk/include/gdk-pixbuf-2.0 -IC:/gtk/include/pango-1.0 -IC:/gtk/include/glib-2.0 -IC:/gtk/lib/glib-2.0/include -IC:/gtk/include -IC:/gtk/include/freetype2 -IC:/gtk/include/libpng14  -LC:/gtk/lib -lgtk-win32-2.0 -lgdk-win32-2.0 -latk-1.0 -lgio-2.0 -lpangowin32-1.0 -lgdi32 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lpango-1.0 -lcairo -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lglib-2.0 -lintl`

4) If all goes well Mendaffe executable will be in C:\mednaffe\src\ .
