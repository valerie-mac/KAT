AUTOMAKE_OPTIONS = subdir-objects


# What to build
bin_PROGRAMS = kat

# Executable inputs
kat_CXXFLAGS = \
	-g -O3 -fwrapv -Wall -Wextra \
	-Wno-deprecated-declarations -Wno-unused-function -Wno-unused-parameter \
	-Wno-unused-variable -Wno-unused-command-line-argument \
	-ansi -pedantic -std=c++11 \
	@AM_CXXFLAGS@

kat_CPPFLAGS = \
	-isystem $(top_srcdir)/deps/seqan-library-2.0.0/include \
	-isystem $(top_srcdir)/deps/jellyfish-2.2.0/include \
	-isystem $(top_srcdir)/lib/include \
	@AM_CPPFLAGS@

kat_LDFLAGS = \
	-static \
	-L../lib \
	-L../deps/jellyfish-2.2.0 \
	@AM_LDFLAGS@

kat_LDADD = \
	@AM_LIBS@ \
	-lkat \
	-lkat_jellyfish

	
noinst_HEADERS = \
	plot_density.hpp \
	plot_profile.hpp \
	plot_spectra_cn.hpp \
	plot_spectra_mx.hpp \
	plot_spectra_hist.hpp \
	plot.hpp \
	filter_kmer.hpp \
	filter_sequence.hpp \
	filter.hpp \
	comp.hpp \
	gcp.hpp \
	histogram.hpp \
	sect.hpp
	
kat_SOURCES = \
	plot_density.cc \
	plot_profile.cc \
	plot_spectra_cn.cc \
	plot_spectra_mx.cc \
	plot_spectra_hist.cc \
	plot.cc \
	filter_kmer.cc \
	filter_sequence.cc \
	filter.cc \
	comp.cc \
	gcp.cc \
	histogram.cc \
	sect.cc \
	kat.cc
