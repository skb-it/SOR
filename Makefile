all: director patient pc_doctor registration generator laryngologist neurologist eyedoc cardiologist pediatrician surgeon

director: director.c common.c errors.c common.h errors.h
	gcc -Wall -g director.c common.c errors.c -o director -pthread

generator: generator.c common.c errors.c common.h errors.h
	gcc -Wall -g generator.c common.c errors.c -o generator -pthread

patient: patient.c common.c errors.c common.h errors.h
	gcc -Wall -g patient.c common.c errors.c -o patient

pc_doctor: pc_doctor.c common.c errors.c common.h errors.h
	gcc -Wall -g pc_doctor.c common.c errors.c -o pc_doctor

registration: registration.c common.c errors.c common.h errors.h
	gcc -Wall -g registration.c common.c errors.c -o registration

laryngologist: laryngologist.c common.c errors.c common.h errors.h
	gcc -Wall -g laryngologist.c common.c errors.c -o laryngologist

neurologist: neurologist.c common.c errors.c common.h errors.h
	gcc -Wall -g neurologist.c common.c errors.c -o neurologist

surgeon: surgeon.c common.c errors.c common.h errors.h
	gcc -Wall -g surgeon.c common.c errors.c -o surgeon

eyedoc: eyedoc.c common.c errors.c common.h errors.h
	gcc -Wall -g eyedoc.c common.c errors.c -o eyedoc

cardiologist: cardiologist.c common.c errors.c common.h errors.h
	gcc -Wall -g cardiologist.c common.c errors.c -o cardiologist

pediatrician: pediatrician.c common.c errors.c common.h errors.h
	gcc -Wall -g pediatrician.c common.c errors.c -o pediatrician

clean:
	rm -f director patient pc_doctor registration generator laryngologist neurologist surgeon eyedoc cardiologist pediatrician *.o log_sor.txt error_sor.txt