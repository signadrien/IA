#########
#
# Makefile pour fonctionTCP
#
#########

# For Solaris
#LD_FLAGS = -lsocket -lnsl

# For Linux
LD_FLAGS =

all: joueur arbitre MoteurIA.class

run : all
	konsole -e ./arbitre 5767&
	konsole -e ./joueur 5767&
	konsole -e ./joueur 5767&
	konsole -e java MoteurIA 127.0.0.1 5768&
	konsole -e java MoteurIA 127.0.0.1 5769

MoteurIA.class: MoteurIA.java
	javac MoteurIA.java

fonctionTCP.o: fonctionTCP.c fonctionTCP.h
	gcc -c -o fonctionTCP.o fonctionTCP.c $(LD_FLAGS)

joueur: Joueur.c fonctionTCP.o ProtoQuantik.h 
	gcc -Wall Joueur.c -o joueur fonctionTCP.o $(LD_FLAGS)
	
arbitre: Arbitre.c fonctionTCP.o quantik-fPIC.o ProtoQuantik.h
	gcc -Wall Arbitre.c -o arbitre quantik-fPIC.o fonctionTCP.o $(LD_FLAGS)
	

clean:
	rm *~ ; rm -i \#* ; rm *.o; \
    rm joueur ; rm arbitre
