CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -Iinclude
LIBS=-lws2_32


SRC=src/main.c src/util.c src/cidr.c src/route_table.c src/lookup.c src/router_cli.c src/graph.c src/ospf.c
OUT=routing_sim


$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LIBS) -o $(OUT)


clean:
	rm -f $(OUT)