OUTPUT = shell
FLAGS = -g -Wall -Wvla -fsanitize=address

%: %.c
	gcc $(FLAGS) -o $@ $< ./string/string.c ./string-array/string-array.c ./job/job.c ./job-array/job-array.c

all: $(OUTPUT)

clean:
	rm -f $(OUTPUT)