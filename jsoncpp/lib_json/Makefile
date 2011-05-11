# Makefile

# Compiler name
CXX=/bin/g++
MAKEDEPEND=gcc -M $(CXXFLAGS) -o $*.d $<

AR=ar rs

# Libraries to be included
LDLIBS=-static

# Flagas
CXXFLAGS=-Wall -O3 -I../include

# Variables
SRCS = json_reader.cpp  json_value.cpp  json_writer.cpp
OBJS = $(SRCS:.cpp=.o)

#Application name
libjson: $(OBJS)
	$(AR) lib_json.a $(OBJS)

%.o : %.c
	cp $*.d $*.P; \
        sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
            -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
        rm -f $*.d
	$(COMPILE.c) -MD -o $@ $<

%.P : %.cpp
	$(MAKEDEPEND)
	@sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' < $*.d > $@; \
	rm -f $*.d; [ -s $@ ] || rm -f $@

clean:
	rm -f *.o *.P lib_json.a

PHONY: clean

