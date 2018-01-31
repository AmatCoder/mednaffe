bin_PROGRAMS=mednaffe
mednaffe_SOURCES=mednaffe.c about.c active.c command.c list.c toggles.c input.c gui.c prefs.c log.c md5.c resource.c common.h mednaffe_glade.h settings_glade.h  system_glade.h toggles.h prefs.h input.h log.h md5.h resource.h

if LINUX
	mednaffe_SOURCES+= joystick_linux.c joystick_linux.h
endif

override CFLAGS +=-std=c99 -Wall -Wl,-export-dynamic

AM_CPPFLAGS        = @GTK_CFLAGS@ $(GTK_DEFINES)
LDADD           = @GTK_LIBS@
CLEANFILES      = *~
DISTCLEANFILES  = .deps/*.P
