EXEFILE = asg4
LIBS = -lglfw -lGLEW -lGL
LIBDIRS += -L/usr/X11R6/lib -L/usr/X11R6/lib64 -L/usr/local/lib
INCDIRS += -I/usr/include -I/usr/local/include -I/usr/include/GL


$(EXEFILE): main.o SPH.o GOBJ.o
	g++ -o $(EXEFILE) $(LIBDIRS) $(INCDIRS) $(LIBS) main.o SPH.o GOBJ.o

main.o: main.cpp SPH.h GOBJ.h Particle.h
	g++ -c -O3 main.cpp

SPH.o: SPH.cpp SPH.h GOBJ.h Particle.h
	g++ -c -O3 SPH.cpp

GOBJ.o: GOBJ.cpp GOBJ.h
	g++ -c -O3 GOBJ.cpp 

clean:
	rm $(EXEFILE) *.o *# *~
