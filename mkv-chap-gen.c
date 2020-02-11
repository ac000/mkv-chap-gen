/*
 * mkv-chap-gen.c - Generate a simple chapter format file for Matroska
 *
 * Copyright (c) 2016, 2020	Andrew Clayton <andrew@digital-domain.net>
 *
 * Licensed under the GNU General Public License V2
 * See COPYING
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "jsmn.c"

#define TC_LEN		  32
#define NR_TKNS		 512
#define BUF_SIZE	4096

static void ms_to_timecode(int ms, char *timecode)
{
	int hours;
	int minutes;
	int seconds;
	int millis;

	millis = ms % 1000;
	seconds = (ms - millis)/1000 % 60;
	minutes = (ms - millis)/1000 / 60 % 60;
	hours = (ms - millis)/1000 / 60 / 60;

	snprintf(timecode, TC_LEN, "%02d:%02d:%02d.%03d", hours, minutes,
		 seconds, millis);
}

static void parse_json(const char *json)
{
	jsmn_parser parser;
	jsmntok_t *tokens;
	int err;
	int i = 1;
	int chapters = -1;
	int chapters_found = 0;
	int start_time = 0;

	if (!json)
		return;

	jsmn_init(&parser);
	tokens = malloc(sizeof(jsmntok_t) * NR_TKNS);
	do {
		err = jsmn_parse(&parser, json, strlen(json), tokens,
				 NR_TKNS * i);
		if (err == JSMN_ERROR_NOMEM) {
			i++;
			tokens = realloc(tokens,
					 sizeof(jsmntok_t) * NR_TKNS * i);
		} else if (err < 0) {
			printf("mkv-chap-gen::jsmn_parse: error: ");
			switch (err) {
			case JSMN_ERROR_INVAL:
				printf("bad token, JSON string is corrupted\n");
				break;
			case JSMN_ERROR_PART:
				printf("JSON string is too short, expecting more JSON data\n");
				break;
			}
			exit(EXIT_FAILURE);
		}
	} while (err < 0);

	for (i = 1; i <= err; i++) {
		jsmntok_t tkn;

		memcpy(&tkn, &tokens[i], sizeof(jsmntok_t));
		if (strncmp(json + tkn.start, "chapters", 8) == 0) {
			chapters = 0;
		} else if (tkn.type == JSMN_ARRAY && chapters == 0) {
			chapters = tkn.size;
		} else if (strncmp(json + tkn.start, "msecs", 5) == 0 &&
			   chapters > 0 && chapters_found < chapters) {
			char stime[12];
			char tc[TC_LEN];

			chapters_found++;
			ms_to_timecode(start_time, tc);
			printf("CHAPTER%02d=%s\n", chapters_found, tc);
			printf("CHAPTER%02dNAME=Chapter %d\n", chapters_found,
			       chapters_found);

			if (chapters_found == chapters)
				break;

			i++;
			memcpy(&tkn, &tokens[i], sizeof(jsmntok_t));
			snprintf(stime, tkn.end - tkn.start + 1, "%s",
				 json + tkn.start);
			start_time += atoi(stime);
		}
	}
}

int main(void)
{
	char *json = NULL;
	ssize_t bytes_read;
	size_t total = 0;
	size_t allocd = 0;

	do {
		json = realloc(json, allocd + BUF_SIZE);
		allocd += BUF_SIZE;
		bytes_read = read(STDIN_FILENO, json + total, BUF_SIZE);
		total += bytes_read;
	} while (bytes_read > 0);
	json[total] = '\0';

	parse_json(json);
	free(json);

	exit(EXIT_SUCCESS);
}
