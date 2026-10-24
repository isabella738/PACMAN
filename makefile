all:
	g++ -g -O0 -I./include source/.main.cpp source/definitions.cpp source/functions.cpp source/terminal.cpp source/bfs.cpp -o PACMAN