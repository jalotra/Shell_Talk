compiler := gcc
dir := src
output_binary := shell_talk.out
binary_deps := -pthread

main_deps := io.o network.o list.o main_thread_runnable.c  
main : 
	$(compiler) $(main_deps) $(binary_deps) -o $(output_binary)
	
io_deps := io.c io.h 
io : $(io_deps)
	$(compiler) -c $(io_deps)

network_deps := network.c network.h 
network : $(network_deps)
	$(compiler) -c $(network_deps)


