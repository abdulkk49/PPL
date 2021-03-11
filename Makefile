#Group no. 18:
#2017B4A70696P   Abdul Kadir Khimani
#2017B3A70652P   Ayush Singh
#2018A7PS0266P   Alfred William Jacob
#2018A7PS0245P   Dhruv Jain


all:  all_dependencies Grammar.txt
	gcc -g -o run.exe *.o

all_dependencies: Grammar.txt
	gcc -g -c *.c

.PHONY: clean
clean:
	rm *.o
	rm *.exe
