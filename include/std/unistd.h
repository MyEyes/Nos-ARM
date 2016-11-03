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

#endif