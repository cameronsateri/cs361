driver: driver.o statemodel.o system.o state.o accepting.o processing.o manufacturing.o shipping.o
	gcc driver.o statemodel.o system.o state.o accepting.o processing.o manufacturing.o shipping.o -o driver

driver.o: driver.c system.h statemodel.h
	gcc -c driver.c

system.o: system.c system.h
	gcc -c system.c -o system.o

statemodel.o: system.h statemodel.h statemodel.c
	gcc -c statemodel.c

state.o: state.h state.c
	gcc -c state.c -o state.o

accepting.o: state.h accepting.h accepting.c
	gcc -c accepting.c

processing.o: state.h processing.h processing.c
	gcc -c processing.c

manufacturing.o: state.h manufacturing.h manufacturing.c
	gcc -c manufacturing.c

clean:
	rm *.o ; rm driver
