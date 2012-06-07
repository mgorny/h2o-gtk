bin_PROGRAMS = h2o-gtk

h2o_gtk_SOURCES = src/main.cxx src/main.hxx \
	src/calcbox.cxx src/calcbox.hxx \
	src/dataentrypair.cxx src/dataentrypair.hxx \
	src/datainputoutput.cxx src/datainputoutput.hxx \
	src/expansionbox.cxx src/expansionbox.hxx \
	src/plot.cxx src/plot.hxx \
	src/saturationbox.cxx src/saturationbox.hxx
h2o_gtk_CPPFLAGS = $(LIBH2OXX_CFLAGS) $(GTKMM_CFLAGS) $(PLOTMM_CFLAGS)
h2o_gtk_LDADD = $(LIBH2OXX_LIBS) $(GTKMM_LIBS) $(PLOTMM_LIBS)

EXTRA_DIST = NEWS
NEWS: configure.ac Makefile.am
	git for-each-ref refs/tags --sort '-*committerdate' \
		--format '# %(tag) (%(*committerdate:short))%0a%(contents:body)' \
		> NEWS
