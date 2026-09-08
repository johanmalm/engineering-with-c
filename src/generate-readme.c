// SPDX-License-Identifier: MIT
#define _POSIX_C_SOURCE 200809L
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UNSPECIFIED (-1)

int get_indentation(const char *line)
{
	int indentation = 0;
	while (*line == '\t') {
		++line;
		++indentation;
	}
	return indentation;
}

void process_code_line(const char *line, const char *id)
{
	static int indentation = UNSPECIFIED;
	static bool is_inside = false;
	char pattern[32] = {0};
	snprintf(pattern, sizeof(pattern), "//! [%s]", id);
	if (!strcmp(line, pattern)) {
		is_inside = !is_inside;
		indentation = UNSPECIFIED;
		printf("```\n");
		return;
	}
	if (is_inside) {
		/*
		 * Get the size of the indentation of the first line of the code
		 * snippet.
		 */
		if (indentation == UNSPECIFIED) {
			indentation = get_indentation(line);
		}
		/*
		 * Move the whole code-snippet to the left by the size of the
		 * indentation we just calculated
		 */
		printf("%s\n", line + indentation);
	}
}

void print_code_snippet(const char *filename, const char *id)
{
	if (!filename || !id) {
		fprintf(stderr, "warn: no filename or id\n");
		return;
	}
	char *line = NULL; size_t len = 0;
	FILE *file = fopen(filename, "r");
	if (!file) {
		fprintf(stderr, "warn: cannot read file '%s'\n", filename);
		return;
	}
	while (getline(&line, &len, file) != -1) {
		char *p = strrchr(line, '\n');
		if (p) {
			*p = '\0';
		}
		process_code_line(line, id);
	}
	free(line);
}

void spawn(const char *command)
{
	GError *error = NULL;
	g_spawn_command_line_sync(command, NULL, NULL, NULL, &error);
	if (error) {
		g_warning("cannot spawn: '%s'", error->message);
	}
}

void process_markup(char *line)
{
	g_auto(GStrv) tokens = g_strsplit(line, ":", -1);
	if (!tokens) {
		return;
	}

	guint len = g_strv_length(tokens);

	if (len < 1) {
		return;
	}

	/*
	 * We handle
	 * - exec:command
	 * - code:filename:id
	 */
	if (!g_ascii_strcasecmp(tokens[0], "code") && len == 3) {
		fprintf(stderr, "c");
		print_code_snippet(tokens[1], tokens[2]);
	} else if (!g_ascii_strcasecmp(tokens[0], "exec") && len == 2) {
		fprintf(stderr, "e");
		fflush(stdout);
		spawn(tokens[1]);
		fflush(stdout);
	}
}

int main(int argc, char **argv)
{
	char *line = NULL; size_t len = 0;
	while (getline(&line, &len, stdin) != -1) {
		char *p = strrchr(line, '\n');
		if (p) {
			*p = '\0';
		}

		/*
		 * Lines beginning with @ are special in that they can get code
		 * from other files or spawn other processes to generate output
		 * for README.md
		 */
		if (line[0] == '@') {
			process_markup(line + 1);
		} else {
			printf("%s\n", line);
		}
	}
	free(line);
	fprintf(stderr, "\n");
}
