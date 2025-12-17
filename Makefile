all: main patient doctor registration

main: main.c common.h
	gcc -Wall -g -pthread main.c -o main

patient: patient.c common.h
	gcc -Wall -g -pthread patient.c -o patient

doctor: doctor.c common.h
	gcc -Wall -g -pthread doctor.c -o doctor

registration: registration.c common.h
	gcc -Wall -g -pthread registration.c -o registration

clean:
	rm -f main patient doctor registration 