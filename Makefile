compiler = gcc
dir = src
output_binary = shell_talk.out
binary_deps = -pthread

main_deps = io.o network.o list.o main_thread_runnable.c  
main : $(main_deps)
	$(compiler) $(main_deps) $(binary_deps) -o $(output_binary)
	
io_deps = io.c io.h 
io.o : $(io_deps)
	$(compiler) -c io.c

network_deps = network.c network.h 
network.o : $(network_deps)
	$(compiler) -c network.c

.PHONY: clean
clean :
	rm *.gch io.o network.o 
