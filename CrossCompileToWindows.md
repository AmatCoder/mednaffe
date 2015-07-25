##Cross compile to Windows

Since 0.7 version, Mednaffe for Windows is a statically linked build.
This means that all the libraries needed by the program are included in the resulting executable.

To achieve this, you need to download this:  [mxe-mednaffe-0.8.tar.xz]( https://drive.google.com/file/d/0B_h7m3aBOt9PNDlDeEtoM2wxUjg/view?usp=sharing "mxe-mednaffe-0.8.tar.xz").

This archive includes complete sources and patches used to cross compile Mednaffe to Windows.
Also it includes a modified and stripped-down version of MXE (M cross environment)  http://mxe.cc/

####Building

First, make sure that you get the requirements. See: http://mxe.cc/#requirements

Then you can follow these steps:

1. Download and save [mxe-mednaffe-0.8.tar.xz](https://drive.google.com/file/d/0B_h7m3aBOt9PNDlDeEtoM2wxUjg/view?usp=sharing "mxe-mednaffe-0.8.tar.xz") into some folder (like **/tmp**)
2. Extract it with:

   `xz -d mxe-mednaffe-0.8.tar.xz && tar -xf mxe-mednaffe-0.8.tar`
3. Change to **mxe-mednaffe** directory:

  `cd /tmp/mxe-mednaffe`
4. Type:

  `make mednaffe`

It takes a while because it compiles all packages needed (gcc included).

When it's done you'll find the **mednaffe.exe** file into **/tmp/mxe-mednaffe** folder.
