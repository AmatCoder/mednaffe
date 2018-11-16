bin_PROGRAMS=mednaffe
mednaffe_SOURCES=about.c about.h\
bios.c bios.h\
logbook.c logbook.h\
mainwindow.c mainwindow.h\
manager.c manager.h\
mednaffe.c \
medprocess.c medprocess.h\
panedimage.c panedimage.h\
panedlist.c panedlist.h\
pathcombobox.c pathcombobox.h\
preferences.c preferences.h\
resources.c \
setup.c setup.h\
widgets/marshallers.c widgets/marshallers.h\
widgets/medbiosentry.c widgets/medbiosentry.h \
widgets/medcheckbutton.c widgets/medcheckbutton.h \
widgets/medcolorentry.c widgets/medcolorentry.h \
widgets/medcombobox.c widgets/medcombobox.h \
widgets/meddialogentry.c widgets/meddialogentry.h \
widgets/medentry.c widgets/medentry.h \
widgets/medinput.c widgets/medinput.h \
widgets/medirange.c widgets/medirange.h \
widgets/medjoy.c widgets/medjoy.h \
widgets/medlistjoy.c widgets/medlistjoy.h \
widgets/medrange.c widgets/medrange.h \
widgets/medscale.c widgets/medscale.h \
widgets/medspin.c widgets/medspin.h \
widgets/medwidget.c widgets/medwidget.h \
widgets/menuinput.c widgets/menuinput.h

if LINUX
  mednaffe_SOURCES+= widgets/joystick_linux.c widgets/joystick_linux.h
endif

override CFLAGS +=-std=c99 -Wall -Wl,-export-dynamic

AM_CPPFLAGS        = @GTK_CFLAGS@
LDADD           = @GTK_LIBS@
CLEANFILES      = *~
DISTCLEANFILES  = .deps/*.P
