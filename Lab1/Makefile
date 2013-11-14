FILES=Makefile Questions/questions.pdf mandelbrot.c mandelbrot_main.c ppm.c ppm.h ppm_test.c gl_mandelbrot.c gl_mandelbrot.h mandelbrot_main.c mandelbrot.h compile COPYING.html plot_data.m run settings start time_difference_global.m time_difference_thread.m variables
ARCHIVE=Lab1.zip

# Suggested list of features to tune to measure performance
MAXITER=256
WIDTH=500
HEIGHT=375
LOWER_R=-2
UPPER_R=0.6
LOWER_I=-1
UPPER_I=1
NB_THREADS=0
LOADBALANCE=0
MANDELBROT_COLOR=0
GLUT=0

MEASURE_FLAG=$(if $(MEASURE),-DMEASURE,)
DEBUG_FLAG=$(if $(DEBUG),-DDEBUG,)
## -O3 -msse2 -mfpmath=sse -ftree-vectorize -funroll-loops
CFLAGS= -g -O0 -Wall -DMAXITER=$(MAXITER) -DWIDTH=$(WIDTH) -DHEIGHT=$(HEIGHT) -DLOWER_R=$(LOWER_R) -DUPPER_R=$(UPPER_R) -DLOWER_I=$(LOWER_I) -DUPPER_I=$(UPPER_I) -DNB_THREADS=$(NB_THREADS) -DLOADBALANCE=$(LOADBALANCE) $(MEASURE_FLAG) $(DEBUG_FLAG) -DMANDELBROT_COLOR=$(MANDELBROT_COLOR) -DGLUT=$(GLUT)
## Enable this CFLAG if you want to go faster
#CFLAGS= -O3 -msse2 -mfpmath=sse -ftree-vectorize -funroll-loops  -Wall -DMAXITER=$(MAXITER) -DWIDTH=$(WIDTH) -DHEIGHT=$(HEIGHT) -DLOWER_R=$(LOWER_R) -DUPPER_R=$(UPPER_R) -DLOWER_I=$(LOWER_I) -DUPPER_I=$(UPPER_I) -DNB_THREADS=$(NB_THREADS) -DLOADBALANCE=$(LOADBALANCE) $(MEASURE_FLAG) $(DEBUG_FLAG) -DMANDELBROT_COLOR=$(MANDELBROT_COLOR) -DGLUT=$(GLUT)
LDFLAGS=-lrt -lglut

all: mandelbrot-$(MAXITER)-$(WIDTH)-$(HEIGHT)-$(LOWER_R)-$(UPPER_R)-$(LOWER_I)-$(UPPER_I)-$(NB_THREADS)-$(LOADBALANCE)

clean:
	$(RM) mandelbrot-*
	$(RM) mandelbrot
	$(RM) *.o

mandelbrot-$(MAXITER)-$(WIDTH)-$(HEIGHT)-$(LOWER_R)-$(UPPER_R)-$(LOWER_I)-$(UPPER_I)-$(NB_THREADS)-$(LOADBALANCE): mandelbrot_main.c mandelbrot-$(MAXITER)-$(WIDTH)-$(HEIGHT)-$(LOWER_R)-$(UPPER_R)-$(LOWER_I)-$(UPPER_I)-$(NB_THREADS)-$(LOADBALANCE).o ppm.o gl_mandelbrot.o
	gcc $(LDFLAGS) $(CFLAGS) -o mandelbrot-$(MAXITER)-$(WIDTH)-$(HEIGHT)-$(LOWER_R)-$(UPPER_R)-$(LOWER_I)-$(UPPER_I)-$(NB_THREADS)-$(LOADBALANCE) mandelbrot-$(MAXITER)-$(WIDTH)-$(HEIGHT)-$(LOWER_R)-$(UPPER_R)-$(LOWER_I)-$(UPPER_I)-$(NB_THREADS)-$(LOADBALANCE).o ppm.o gl_mandelbrot.o mandelbrot_main.c

mandelbrot-$(MAXITER)-$(WIDTH)-$(HEIGHT)-$(LOWER_R)-$(UPPER_R)-$(LOWER_I)-$(UPPER_I)-$(NB_THREADS)-$(LOADBALANCE).o: mandelbrot.c
	gcc $(CFLAGS) -c -o mandelbrot-$(MAXITER)-$(WIDTH)-$(HEIGHT)-$(LOWER_R)-$(UPPER_R)-$(LOWER_I)-$(UPPER_I)-$(NB_THREADS)-$(LOADBALANCE).o mandelbrot.c
	
ppm.o: ppm.c
	gcc $(CFLAGS) -c -o ppm.o ppm.c
	
gl_mandelbrot.o: gl_mandelbrot.c
	gcc $(CFLAGS) -c -o gl_mandelbrot.o gl_mandelbrot.c

dist:
	zip $(ARCHIVE) $(FILES)
