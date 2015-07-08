#include "util.h"

/*
TODO

give exit and return error number some define
to clarify error information

*/

static void report(const char *prefix, const char *err, va_list params)
{
	char msg[2014];
	vsnprintf(msg,sizeof(msg),err,params);
	fprintf(stderr," %s%s\n", prefix, msg);
}

static NORETURN void die_builtin(const char *err, va_list params)
{
	report(" Fatal: ", err, params);
	exit(128);
}

static void error_builtin(const char *err, va_list params)
{
	report(" Error: ", err, params);
}

static void warn_builtin(const char *warn, va_list params)
{
	report(" Warning: ", warn, params);
}

static void info_builtin(const char *info, va_list params)
{
	report(" Info: ", info, params);
}


void die(const char *err, ...)
{
	va_list params;

	va_start(params,err);
	die_builtin(err, params);
	va_end(params);
}

int pr_err(const char *err, ...)
{
	va_list params;

	va_start(params, err);
	error_builtin(err, params);
	va_end(params);
	return -1;
}

void pr_warning(const char *warn, ...)
{
	va_list params;

	va_start(params, warn);
	warn_builtin(warn, params);
	va_end(params);
}

void pr_info(const char *info, ...)
{
	va_list params;

	va_start(params, info);
	info_builtin(info, params);
	va_end(params);
}

void die_perror(const char *s)
{
	perror(s);
	exit(1);
}

