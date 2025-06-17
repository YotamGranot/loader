// hello.c
__attribute__((naked)) void _start() {
    __asm__(
        // write(1, "Hello, world\n", 13)
        "mov $1, %%rax\n"          // syscall number (sys_write)
        "mov $1, %%rdi\n"          // file descriptor (stdout)
        "lea message(%%rip), %%rsi\n" // pointer to message
        "mov $13, %%rdx\n"         // message length
        "syscall\n"

        // exit(0)
        "mov $60, %%rax\n"         // syscall number (sys_exit)
        "xor %%rdi, %%rdi\n"       // status = 0
        "syscall\n"

        "message:\n"
        ".ascii \"Hello, world\\n\"\n"
        :
        :
        : "rax", "rdi", "rsi", "rdx"
    );
}

