all: director patient pc_doctor registration generator laryngologist neurologist eyedoc cardiologist pediatrician surgeon

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

laryngologist: laryngologist.c errors.c common.h
	gcc -Wall -g laryngologist.c errors.c -o laryngologist

neurologist: neurologist.c errors.c common.h
	gcc -Wall -g neurologist.c errors.c -o neurologist

surgeon: surgeon.c errors.c common.h
	gcc -Wall -g surgeon.c errors.c -o surgeon

eyedoc: eyedoc.c errors.c common.h
	gcc -Wall -g eyedoc.c errors.c -o eyedoc

cardiologist: cardiologist.c errors.c common.h
	gcc -Wall -g cardiologist.c errors.c -o cardiologist

pediatrician: pediatrician.c errors.c common.h
	gcc -Wall -g pediatrician.c errors.c -o pediatrician

clean:
	rm -f director patient pc_doctor registration generator laryngologist neurologist surgeon eyedoc cardiologist pediatrician *.o log_sor.txt