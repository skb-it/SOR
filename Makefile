all: main patient doctor registration

main: main.c errors.c common.h 
	gcc -Wall -g -pthread main.c errors.c -o main

patient: patient.c errors.c  common.h
	gcc -Wall -g -pthread patient.c errors.c -o patient

doctor: doctor.c errors.c  common.h
	gcc -Wall -g -pthread doctor.c errors.c -o doctor

registration: registration.c errors.c  common.h
	gcc -Wall -g -pthread registration.c errors.c -o registration

clean:
	rm -f main patient doctor registration *.o sor_log.txt