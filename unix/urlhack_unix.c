#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "urlhack.h"

<<<<<<< HEAD
void urlhack_launch_url(const char* app, const char *url)
{
	const size_t max = 4000;
=======
enum { max = 4000 };
void urlhack_launch_url(const char* app, const wchar_t *url)
{
>>>>>>> upstream/master
	char buf[max];
	const char *browser = NULL;

	if (app)
		browser = app;
	if (NULL == browser || 0 == strlen(browser))
		browser = "xdg-open";

<<<<<<< HEAD
	snprintf(buf, max, "%s %s", browser, url);

=======
	strncat(buf, browser, max - 1);
	strcat(buf, " ");
	wcstombs(buf + strlen(buf), url, max - strlen(buf) - 2);
>>>>>>> upstream/master
	if (!system(buf))
		printf("couldn't run browser: %s", buf);
}

int urlhack_is_ctrl_pressed()
{
	// TODO
	return 1;
}


void rtfm(const char *error)
{
	// TODO
}



