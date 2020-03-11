#COPYRIGHT © 2012-2014 G.TEC MEDICAL ENGINEERING GMBH, AUSTRIA
CC = g++
CFLAGS = -Wall
OBJSEMOTIV = main.o interfazEEG.o preprocesado.o best_referencies.o emotiv.o
OBJSGTEC = main.o interfazEEG.o preprocesado.o best_referencies.o gtec.o
LIBS = -l:libgusbampapi.so.1.14 -L/home/gnb/EmotivResearch_1.0.0.5/lib -ledk -ledk_utils -lgsl -lgslcblas -lpthread

all: undo ejecucionEmotiv ejecucionGTec

ejecucionEmotiv: $(OBJSEMOTIV)
	$(CC) $(CFLAGS) -o ejecucionEmotiv $(OBJSEMOTIV) $(LIBS)

ejecucionGTec: $(OBJSGTEC)
	$(CC) $(CFLAGS) -o ejecucionGTec $(OBJSGTEC) $(LIBS)

undo:
	rm -f *.o
	rm -f ejecucionEmotiv
	rm -f ejecucionGTec

clean:
	rm -f *.o




