CC=g++
LD=g++
CC_OPTIONS_BASE=-std=c++20 -Iinclude/ -Wall -Wextra -Wpedantic -Wno-unknown-pragmas -Werror -pedantic -pedantic-errors
LD_OPTIONS=-lstdc++ -lm
SRC=$(shell find src/ -type f -name "*.cpp")
OBJ=$(SRC:.cpp=.o)

ifeq ($(DEBUG), 1)
	CC_OPTIONS=-DDEBUG -g $(CC_OPTIONS_BASE)
else
	CC_OPTIONS=$(CC_OPTIONS_BASE)
endif

.PHONY: cbd clean submodules-clone doc

%.o: %.cpp
	${CC} -c $^ -o $@ ${CC_OPTIONS}

%.e: %.cpp
	${CC} -E $^ $@ ${CC_OPTIONS} > $@

cbd: $(OBJ)
	${LD} ${LD_OPTIONS} $^ -o cbd

clean:
	rm -f cbd
	rm -f ${OBJ}
	rm -rf output/
	rm -f core.*
	rm -f doc.html
	rm -rf latex/ html/

submodules-clone:
	git submodule init
	git submodule update -f

install:
	sudo install ./cbd /usr/local/bin/cbd
	sudo install ./cbd-iterative /usr/local/bin/cbd-iterative
	sudo mkdir -p /usr/local/lib/cbd/
	sudo install -m 0755 ./scripts/delete_trivial_adversarial_sequences.sh /usr/local/lib/cbd/delete_trivial_adversarial_sequences.sh
	sudo install -m 0755 ./scripts/reduce_generated_inputs_for_oracle.sh /usr/local/lib/cbd/reduce_generated_inputs_for_oracle.sh
	sudo install -m 0755 ./scripts/generate_sha256sums_of_generated_concrete_values.sh /usr/local/lib/cbd/generate_sha256sums_of_generated_concrete_values.sh

doc:
	rm -rf html/ latex/ doc.html
	doxygen doxygen.conf
	ln -s html/index.html doc.html