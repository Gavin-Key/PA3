#Make file
PA3: myshell.o
	gcc myshell.o -o myshell

myshell.o: myshell.c
	gcc -c myshell.c