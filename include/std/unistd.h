#ifndef __UNISTD_H__
#define __UNISTD_H__
#include <stdint.h>
#include <stddef.h>
#include <lock.h>

typedef uint32_t pid_t;

pid_t fork();

int execv(const char *path, char *const argv[]);
int execve(const char *path, char *const argv[]);
int execvp(const char *path, char *const argv[]);

void* sbrk(size_t size);
void* pmap(void* p_addr, void* v_addr, size_t mem);
uint32_t thread_create(void* stack_start, void* stack_end, void* entry);
uint32_t thread_run(uint32_t tid);


#endif
