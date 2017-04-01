#define SYSCALL(id, r) __asm__("swi "#id"\n bx lr\n":"=0"(r))
#define SYSCALL1(id, p1, r) __asm__("swi "#id"\n bx lr\n":"=r0"(r):"0"(p1));
#define SYSCALL2(id, p1, p2, r) __asm__("mov r1, %2\nswi "#id"\n bx lr\n":"=r0"(r):"0"(p1), "r"(p2));