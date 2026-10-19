# Makefile. If you change it, remember than in makefiles multiple spaces
# ARE NOT EQUIVALENT to tabs. The line after a rule starts with a tab!

#Add any executable you want to be created here.
EXECUTABLES = 09_sobel_sqrtf 10_sobel_compiler_hints

#This is the compiler to use
CC = clang

#Optimization level. Override with e.g. make OPT=-O0
OPT = -O3 -ffast-math

#These are the flags passed to the compiler. Change accordingly
CFLAGS = -Wall $(OPT)

#These are the flags passed to the linker. Nothing in our case
LDFLAGS = -lm


# make all will create all executables and link the input images from data/
all: $(EXECUTABLES) | input.grey golden.grey

input.grey golden.grey:
	ln -s ../../data/$@ $@

# This is the rule to create any executable from the corresponding .c 
# file with the same name.
%: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# make clean will remove all executables, jpg files and the 
# output of previous executions.
clean:
	rm -f $(EXECUTABLES) *.jpg output_sobel.grey

# make image will create the output_sobel.jpg from the output_sobel.grey. 
# Remember to change this rule if you change the name of the output file.
image: output_sobel.grey
	convert -depth 8 -size 4096x4096 GRAY:output_sobel.grey output_sobel.jpg 
