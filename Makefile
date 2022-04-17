all: libdma.a app

libdma.a: dma.c
	gcc -Wall -g -c dma.c -lrt -pthread
	ar rcs libdma.a dma.o
app: app.c
	gcc -Wall -g -o app app.c -L. -ldma -lm -lrt -pthread

clean:
	rm -fr *~ libdma.a dma.o app.o app
