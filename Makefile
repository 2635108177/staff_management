all:c s
c:client.c
	gcc client.c -o c
s:server.c
	gcc server.c socketinit.c -o s -pthread -l sqlite3
clean:
	rm c s
