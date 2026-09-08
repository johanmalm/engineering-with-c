#include <glib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
//! [0]
	char **tokens = g_strsplit_set("foo bar\tbaz", " \t", -1);
	for (size_t i = 0; tokens[i]; ++i) {
		printf("%s\n", tokens[i]);
	}
	g_strfreev(tokens);
//! [0]
}
