#Compile the launch.c by linking it with the lib_simpleloader.so
all:
	gcc -m32 launch.c -o ../bin/launch ../bin/lib_simpleloader.so -Wl,-rpath=../bin
#Provide the command for cleanup
clean:
	-@rm -f ../bin/launch
