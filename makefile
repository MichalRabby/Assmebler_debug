MainAssembler: build/MainAssembler.o build/main_functions.o build/error_handler.o build/funcs.o build/label_nodes.o build/row_nodes.o build/macro_nodes.o build/data_nodes.o build/label_handler.o build/define_handler.o build/extern_handler.o build/dstring_handler.o build/parse_command.o build/command_nodes.o build/file_handler.o
	gcc -g -ansi -Wall -pedantic build/MainAssembler.o build/main_functions.o build/error_handler.o build/funcs.o build/label_nodes.o build/row_nodes.o build/macro_nodes.o build/data_nodes.o build/label_handler.o build/define_handler.o build/extern_handler.o build/dstring_handler.o build/parse_command.o build/command_nodes.o build/file_handler.o -o MainAssembler

build/MainAssembler.o: src/mainAssembler.c include/assembler.h | build
	gcc -ansi -Wall -pedantic -c src/mainAssembler.c -o build/MainAssembler.o

build/error_handler.o: lib/error_handler.c | build
	gcc -ansi -Wall -pedantic -c lib/error_handler.c -o build/error_handler.o

build/funcs.o: lib/funcs.c | build
	gcc -ansi -Wall -pedantic -c lib/funcs.c -o build/funcs.o

build/main_functions.o: lib/main_functions.c | build
	gcc -ansi -Wall -pedantic -c lib/main_functions.c -o build/main_functions.o

build/label_nodes.o: data_structures/label_nodes.c | build
	gcc -ansi -Wall -pedantic -c data_structures/label_nodes.c -o build/label_nodes.o

build/row_nodes.o: data_structures/row_nodes.c | build
	gcc -ansi -Wall -pedantic -c data_structures/row_nodes.c -o build/row_nodes.o

build/macro_nodes.o: data_structures/macro_nodes.c | build
	gcc -ansi -Wall -pedantic -c data_structures/macro_nodes.c -o build/macro_nodes.o

build/data_nodes.o: data_structures/data_nodes.c | build
	gcc -ansi -Wall -pedantic -c data_structures/data_nodes.c -o build/data_nodes.o

build/command_nodes.o: data_structures/command_nodes.c | build
	gcc -ansi -Wall -pedantic -c data_structures/command_nodes.c -o build/command_nodes.o

build/label_handler.o: lib/label_handler.c | build
	gcc -ansi -Wall -pedantic -c lib/label_handler.c -o build/label_handler.o

build/define_handler.o: lib/define_handler.c | build
	gcc -ansi -Wall -pedantic -c lib/define_handler.c -o build/define_handler.o

build/extern_handler.o: lib/extern_handler.c | build
	gcc -ansi -Wall -pedantic -c lib/extern_handler.c -o build/extern_handler.o

build/dstring_handler.o: lib/dstring_handler.c | build
	gcc -ansi -Wall -pedantic -c lib/dstring_handler.c -o build/dstring_handler.o

build/parse_command.o: lib/parse_command.c | build
	gcc -ansi -Wall -pedantic -c lib/parse_command.c -o build/parse_command.o

build/file_handler.o: lib/file_handler.c | build
	gcc -ansi -Wall -pedantic -c lib/file_handler.c -o build/file_handler.o

build:
	mkdir -p build

clean:
	rm -rf build/*.o MainAssembler
