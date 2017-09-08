Earl Honeycutt (ehoneyc@g.clemson.edu)

This project is divided into two parts:

1) leakcount.c and memory_shim.c

leakcount.c and memory_shim.c work together to intercept the library implementations of malloc() and free() for a child process while adding additional functionality in the form of a built-in basic memory leak counter.

2) sctracer.c

sctracer.c utilizes ptrace() to trace all of the system calls made by an exec'd shell command. Further, it writes all tracked system calls into a file designated on the command line at execution.

KNOWN PROBLEMS:

leakcount will fail under the stated conditions in the assignment documentation (_exit, abort, closed stderr, etc.)

definitely some flaws with how the single argument shell command was handled in sctracer, but I couldn't get it functional with a system() call

probably not the best method of handling the "double amount of calls" problem

DESIGN (References):

https://phoxis.org/2011/04/27/c-language-constructors-and-destructors-with-gcc/
http://man7.org/linux/man-pages/man2/ptrace.2.html
http://www.linuxjournal.com/article/6100
http://syscalls.kernelgrok.com/
https://linux.die.net/man/1/strace
https://linux.die.net/man/3/execvpe
