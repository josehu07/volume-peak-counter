

CC 		= gcc
CFLAGS 	= -O3 -I./src
RM		= rm -f

default:	gnuplot_i.o

gnuplot_i.o: src/gnuplot_i.c src/gnuplot_i.h
	$(CC) $(CFLAGS) -c -o gnuplot_i.o src/gnuplot_i.c

clean:
	$(RM) gnuplot_i.o

