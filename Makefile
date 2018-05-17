shell: libCommandUtil.a
	gcc -L. -o shellC shell.c -lCommandUtil

libCommandUtil.a: commandUtil.o
	ar -cvq libCommandUtil.a commandUtil.o 

commandUtil.o:
	gcc -c commandUtil.c

clear:
	rm -f commandUtil shellC a.out lib*.a *.o
