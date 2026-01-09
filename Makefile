all: director patient pc_doctor registration generator

director: director.c errors.c common.h 
	gcc -Wall -g director.c errors.c -o director

generator: generator.c errors.c common.h
	gcc -Wall -g generator.c errors.c -o generator

patient: patient.c errors.c  common.h
	gcc -Wall -g patient.c errors.c -o patient

pc_doctor: pc_doctor.c errors.c  common.h
	gcc -Wall -g pc_doctor.c errors.c -o pc_doctor

registration: registration.c errors.c  common.h
	gcc -Wall -g registration.c errors.c -o registration

clean:
	rm -f director patient pc_doctor registration generator *.o sor_log.txt