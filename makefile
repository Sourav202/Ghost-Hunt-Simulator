OPT=-Wall -Wextra -Werror -lpthread
OBJ=evidence.o ghost.o house.o hunter.o logger.o main.o room.o utils.o print.o

FinalProject:	evidence.o ghost.o house.o hunter.o logger.o main.o room.o utils.o print.o 
				gcc ${OPT} -o FinalProject ${OBJ}

evidence.o:	evidence.c defs.h
			gcc ${OPT} -c evidence.c

ghost.o:	ghost.c defs.h
			gcc ${OPT} -c ghost.c

house.o:	house.c defs.h
			gcc ${OPT} -c house.c

hunter.o:	hunter.c defs.h
			gcc ${OPT} -c hunter.c

logger.o:	logger.c defs.h
			gcc ${OPT} -c logger.c

main.o:		main.c defs.h
			gcc ${OPT} -c main.c

room.o:		room.c defs.h
			gcc ${OPT} -c room.c

utils.o:	utils.c defs.h
			gcc ${OPT} -c utils.c

print.o:	print.c defs.h
			gcc ${OPT} -c print.c

clean:
			rm -f ${OBJ} link