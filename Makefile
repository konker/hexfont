CC = gcc
CFLAGS = -Wall -g -fno-builtin
EXTRA_FLAGS = -L. -lhexfont
utilObjs = hexfont.o
utilObjsList = hexfont_list.o

example: example.c libhexfont.a
	$(CC) $(CFLAGS) -o $@ $< $(EXTRA_FLAGS)

libhexfont.a: $(utilObjs) $(utilObjsList)
	ar rc $@ $(utilObjs) $(utilObjsList)
	ranlib $@

$(utilObjs): hexfont.c hexfont.h
	$(CC) $(CFLAGS) -c $<

$(utilObjsList): hexfont_list.c hexfont_list.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o libhexfont.a example

all: example
.PHONY: all clean
