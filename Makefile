# Makefile

# Compiler name
CXX=/bin/g++
MAKEDEPEND=gcc -M $(CXXFLAGS) -o $*.d $<

# Libraries to be included
LDLIBS=./jsoncpp/lib_json/lib_json.a -lboost_thread-mt

# Flagas
CXXFLAGS=-Wall -O3 -I./jsoncpp/include -std=c++0x

# Variables
SRCS = main.cc stringTable.cc listing.cc product.cc adhoc/normalize.cc adhoc/matching.cc
OBJS = $(SRCS:.cc=.o)

#Application name
snapsort_challenge: $(OBJS)
	$(CXX) $(OBJS) $(LDLIBS)  -o snapsort-challenge

%.o : %.c
	cp $*.d $*.P; \
        sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
            -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
        rm -f $*.d
	$(COMPILE.c) -MD -o $@ $<

%.P : %.cc
	$(MAKEDEPEND)
	@sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' < $*.d > $@; \
	rm -f $*.d; [ -s $@ ] || rm -f $@

#include $(SRCS:.cc=.P)


clean:
	rm -f *.o adhoc/*.o *.P snapsort-challenge

PHONY: clean

