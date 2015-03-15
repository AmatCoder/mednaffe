## Building mednaffe on Linux
To build mednaffe you need GTK+2 (2.18 or above) or GTK+3 (3.4 or above) development libraries.

* Run the "configure" script here, e.g.:
  ``./configure``
  If you want to build gtk3 version:
  ``./configure --enable-gtk3``

* Then build with:
  ``make``

* To install it then do:
  ``make install``


*NOTE: You do not need to install mednaffe, it can be run from /src folder. Mednafen emulator must be in PATH*
