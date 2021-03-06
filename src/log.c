#include <assert.h>
#include <time.h>
#include <stdarg.h>
#include <syslog.h>
#include <stdlib.h>

#include "log.h"

static const char *gh_log_path = NULL;

static FILE *gh_log_file;
static int gh_syslog_enabled;

void gh_log_open(const char *path)
{
	FILE *new_log;

	if (path == NULL) {
		gh_syslog_enabled = 0;
		gh_log_file = NULL;
		return;
	}

	if (!strcmp(path, "syslog")) {
		openlog(NULL, LOG_PID, LOG_LOCAL7);
		gh_syslog_enabled = 1;
		return;
	}

	new_log = fopen(path, "a");

	if (new_log == NULL) {
		fprintf(stderr, "Failed to open log file at '%s'\n", path);
		return;
	}

	if (gh_log_file)
		fclose(gh_log_file);

	if (gh_syslog_enabled) {
		closelog();
		gh_syslog_enabled = 0;
	}

	gh_log_file = new_log;
	gh_log_path = path;
}

void gh_log_reopen(void)
{
	if (gh_log_path != NULL)
		gh_log_open(gh_log_path);
}

void gh_log_write(const char *message, ...)
{
	va_list vl;
	va_start(vl, message);

	if (gh_syslog_enabled) {
		vsyslog(LOG_INFO, message, vl);
	} else if (gh_log_file) {
		/* TODO: add syslog-like extra args here */
		vfprintf(gh_log_file, message, vl);
		fflush(gh_log_file);
	} else {
		vfprintf(stderr, message, vl);
		fflush(stderr);
	}

	va_end(vl);
}

void gh_log_die(void)
{
	exit(1);
}

static const char *_app_instance = NULL;

const char *gh_log_instance(void)
{
	return _app_instance;
}

void gh_log_set_instance(const char *instance)
{
	_app_instance = instance;
}
