svlocaledir = $(datadir)/locale/sv/LC_MESSAGES
svlocale_DATA = mednaffe.mo

mednaffe.mo: sv.po
	$(AM_V_GEN)$(MSGFMT) --check -o $@ $<

EXTRA_DIST = mednaffe.pot sv.po
CLEANFILES = mednaffe.mo
