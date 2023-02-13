Lab2_Week1 : Lab2-Week1-Basic.c
		gcc -pthread $^ -o $@
		clear


.PHONY : run
run :
		./Lab2_Week1

.PHONY : clean
clean :
		rm -rf *.o *~ Lab2_Week1
		clear