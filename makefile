CC = gcc

CFLAGS = -Wall

INC_PATH =

OBJS = shell_final.o commands.o step2.o step3.o

LIB =

PROG = my_shell


all : $(OBJS) 
	$(CC) $(INC_PATH) -o $(PROG) $(OBJS) $(LIB) 

shell_final.o : ./src/shell_final.c
	$(CC) $(CFLAGS)  -c ./src/shell_final.c
commands.o : ./src/commands.c
	$(CC) $(CFLAGS) $(INC_PATH) -c $< 

step2.o : ./src/step2.c
	$(CC) $(CFLAGS) $(INC_PATH) -c $<

step3.o : ./src/step3.c
	$(CC) $(CFLAGS) $(INC_PATH) -c $<

clean :
	-rm *.o *~ $(PROG) 

