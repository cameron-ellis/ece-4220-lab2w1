Lab2_Week1: Lab2-Week1-Basic.c
		gcc -Wall $^ -o $@ -lpthread
		clear

.PHONY: clean
clean:
		rm -rf *.o *~ Lab2_Week1
		clear