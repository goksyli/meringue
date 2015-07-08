#ifndef UTIL_H
#define UTIL_H


#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
#include <sys/param.h>
#include <sys/types.h>
#include <linux/types.h>


#ifdef __GNUC__
#define NORETURN __attribute__((__noreturn__))
#else
#define NORETURN
#ifndef __attribute__
#define __attribute__(x)
#endif
#endif

extern void die(const char *err, ...) NORETURN __attribute__((format (printf, 1, 2)));
extern void die_perror(const char *s) NORETURN;
extern int pr_err(const char *err, ...) __attribute__((format (printf, 1, 2)));
extern void pr_warning(const char *err, ...) __attribute__((format (printf, 1, 2)));
extern void pr_info(const char *err, ...) __attribute__((format (printf, 1, 2)));



#endif

