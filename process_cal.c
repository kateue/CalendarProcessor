
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE_LEN 132
#define MAX_EVENTS 500


/*
 * A struct type you are welcome to use, but are not
 * required to use.
 */

typedef struct event_t
{
	char dt_start[MAX_LINE_LEN];
	char dt_end[MAX_LINE_LEN];
	char summary[MAX_LINE_LEN];
	char location[MAX_LINE_LEN];
	char dt_repeat_until[MAX_LINE_LEN];
} event_t;


event_t entry[MAX_EVENTS];
int num_entries = 0;


/* Copied from timeplay.c 
 * Function dt_format.
 *
 * Given a date-time, creates a more readable version of the
 * calendar date by using some C-library routines. For example,
 * if the string in "dt_time" corresponds to:
 *
 *   20190520T111500
 *
 * then the string stored at "formatted_time" is:
 *
 *   May 20, 2019 (Mon).
 *
 */

void dt_format(char *formatted_time, const char *dt_time, const int len)
{
	struct tm temp_time;
	time_t    full_time;
	//char      temp[5];

	memset(&temp_time, 0, sizeof(struct tm));
	sscanf(dt_time, "%4d%2d%2d", &temp_time.tm_year, &temp_time.tm_mon, &temp_time.tm_mday);
	temp_time.tm_year -= 1900;
	temp_time.tm_mon -= 1;
	full_time = mktime(&temp_time);
	strftime(formatted_time, len, "%B %d, %Y (%a)",
	localtime(&full_time));
}

//Copied from timeplay.c
void dt_increment(char *after, const char *before, int const num_days)
{
	struct tm temp_time, *p_temp_time;
	time_t    full_time;
	char      temp[5];

	memset(&temp_time, 0, sizeof(struct tm));
	sscanf(before, "%4d%2d%2d", &temp_time.tm_year, &temp_time.tm_mon, &temp_time.tm_mday);
	temp_time.tm_year -= 1900;
	temp_time.tm_mon -= 1;
	temp_time.tm_mday += num_days;

	full_time = mktime(&temp_time);
	after[0] = '\0';

	strftime(after, 9, "%Y%m%d", localtime(&full_time));
	strncpy(after + 8, before + 8, MAX_LINE_LEN - 8);
	after[MAX_LINE_LEN - 1] = '\0';
}


int cmpfunc(const void *a, const void *b)
{
	event_t *pa = (event_t *)a;
	event_t *pb = (event_t *)b;

	if (strcmp((*pa).dt_start, (*pb).dt_start) != 0) {
		return strcmp((*pa).dt_start, (*pb).dt_start);
	}
	else {
		return strcmp((*pa).summary, (*pb).summary);
	}
}

//takes in a file 
void readFile(const char *filename, int from_date, int to_date)
{
	FILE *calender;
	char buffer[MAX_LINE_LEN];

	calender = fopen(filename, "r");

/*
BEGIN:VCALENDAR
VERSION:A
BEGIN:VEVENT
DTSTART:20210102T111500
DTEND: 20210102T123000 
RRULE:FREQ=WEEKLY;WKST=MO;UNTIL=20211002T235959;BYDAY=SA 
LOCATION:The Bumptious Barista
SUMMARY:Coffee with Pat END:VEVENT END:VCALENDAR
*/
//reads a line from the specified file and stores it into the string pointed to by buffer
	while (fgets(buffer, MAX_LINE_LEN, calender)) {
  
  //If the line that gets read in buffer and the BEGIN:VEVENT is identical then
		if (strncmp(buffer, "BEGIN:VEVENT", 12) == 0) {
    
    //define variables
			event_t curr_event;
			int start_date, end_date;

//read a line from the file calender and store it in buffer
			fgets(buffer, MAX_LINE_LEN, calender);
      
      //Reads data from buffer and stores them in curr_event.dt_start
			sscanf(buffer, "DTSTART:%s", curr_event.dt_start);
      
      //read a line from the calender and stores it in buffer
			fgets(buffer, MAX_LINE_LEN, calender);
			sscanf(buffer, "DTEND:%s", curr_event.dt_end);

			fgets(buffer, MAX_LINE_LEN, calender);
      
   //if line ends up on RRULE, then store the line in curr_event.dt_repeat_until
			if (strncmp(buffer, "RRULE", 5) == 0) {
				sscanf(buffer, "RRULE:FREQ=WEEKLY;WKST=MO;UNTIL=%15s", curr_event.dt_repeat_until);
				fgets(buffer, MAX_LINE_LEN, calender);
        
        //stores the length of line in buffer into curr_event.location
				strncpy(curr_event.location, &buffer[9], strlen(buffer));
        
        
				curr_event.location[strlen(curr_event.location) - 1] = '\0';
				fgets(buffer, MAX_LINE_LEN, calender);
				strncpy(curr_event.summary, &buffer[8], strlen(buffer));
			  curr_event.summary[strlen(curr_event.summary) - 1] = '\0';
			}
      
      //if line is not identical and there is no RRULE (non repeating event)
			else {
				strncpy(curr_event.dt_repeat_until, curr_event.dt_start, MAX_LINE_LEN);
				strncpy(curr_event.location, &buffer[9], strlen(buffer));
				curr_event.location[strlen(curr_event.location) - 1] = '\0';
				fgets(buffer, MAX_LINE_LEN, calender);
				strncpy(curr_event.summary, &buffer[8], strlen(buffer));
				curr_event.summary[strlen(curr_event.summary) - 1] = '\0';
			}

			sscanf(curr_event.dt_start, "%8d", &start_date);
			sscanf(curr_event.dt_end, "%8d", &end_date);
			if ((start_date >= from_date) && (end_date <= to_date)) {
				entry[num_entries] = curr_event;
				num_entries++;
			}

			/* recording repeated events within the range */
			while (strncmp(curr_event.dt_start, curr_event.dt_repeat_until, 15) < 0) {
				char after[MAX_LINE_LEN];
				dt_increment(after, curr_event.dt_start, 7);
				strncpy(curr_event.dt_start, after, 15);
				dt_increment(after, curr_event.dt_end, 7);
				strncpy(curr_event.dt_end, after, 15);
				if (strncmp(curr_event.dt_start, curr_event.dt_repeat_until, 15) <= 0) {
					sscanf(curr_event.dt_start, "%8d", &start_date);
					sscanf(curr_event.dt_end, "%8d", &end_date);
					if ((start_date >= from_date) && (end_date <= to_date)) {
						entry[num_entries] = curr_event;
						num_entries++;
					}
				}
			}
		}
	}

	fclose(calender);

//sort array
	if (num_entries > 0) {
		qsort(entry, num_entries, sizeof(event_t), cmpfunc);
	}
}

void print_events(int from_yy, int from_mm, int from_dd, int to_yy, int to_mm, int to_dd) {
   	for (int n = 0; n < num_entries; n++) {
		int start_date, hour_start, minute_start;
		int end_date, hour_end, minute_end;
		char f_time[MAX_LINE_LEN];

		if ((n == 0) || ((n > 0) && (strncmp(entry[n - 1].dt_start, entry[n].dt_start, 8) != 0))) {
			dt_format(f_time, entry[n].dt_start, MAX_LINE_LEN);
			if (n > 0) {
				printf("\n");
			}
			printf("%s\n", f_time);
			for (int m = 0; m < strlen(f_time); m++) {
				printf("-");
			}
			printf("\n");
		}

		sscanf(entry[n].dt_start, "%8dT%2d%2d", &start_date, &hour_start, &minute_start);
		sscanf(entry[n].dt_end, "%8dT%2d%2d", &end_date, &hour_end, &minute_end);

		if (hour_end < 12) {
			printf("%2d:%02d am to %2d:%02d am: %s {{%s}}\n", hour_start, minute_start, hour_end, minute_end, entry[n].summary, entry[n].location);
		} else if (hour_start > 12) {
			printf("%2d:%02d pm to %2d:%02d pm: %s {{%s}}\n", hour_start - 12, minute_start, hour_end - 12, minute_end, entry[n].summary, entry[n].location);
		} else if (hour_start < 12 && hour_end > 12) {
			printf("%2d:%02d am to %2d:%02d pm: %s {{%s}}\n", hour_start, minute_start, hour_end - 12, minute_end, entry[n].summary, entry[n].location);
		} else if (hour_start < 12 && hour_end == 12) {
			printf("%2d:%02d am to %2d:%02d pm: %s {{%s}}\n", hour_start, minute_start, hour_end, minute_end, entry[n].summary, entry[n].location);
		} else if (hour_start == 12 && hour_end > 12) {
			printf("%2d:%02d pm to %2d:%02d pm: %s {{%s}}\n", hour_start, minute_start, hour_end - 12, minute_end, entry[n].summary, entry[n].location);
		} else {
			printf("%2d:%02d pm to %2d:%02d pm: %s {{%s}}\n", hour_start, minute_start, hour_end, minute_end, entry[n].summary, entry[n].location);
		}
	}
}

int main(int argc, char *argv[])
{
	int from_y = 0, from_m = 0, from_d = 0;
	int to_y = 0, to_m = 0, to_d = 0;
	int from_date;
	int to_date;
	char *filename = NULL;

	for (int i = 0; i < argc; i++) {
		if (strncmp(argv[i], "--from=", 7) == 0) {
			sscanf(argv[i], "--from=%d/%d/%d", &from_d, &from_m, &from_y);
		} else if (strncmp(argv[i], "--to=", 5) == 0) {
			sscanf(argv[i], "--to=%d/%d/%d", &to_d, &to_m, &to_y);
		} else if (strncmp(argv[i], "--file=", 7) == 0) {
			filename = argv[i]+7;
		}
	}

	if (from_y == 0 || to_y == 0 || filename == NULL) {
		fprintf(stderr, "usage: %s --from=dd/mm/yyyy --to=dd/mm/yyyy --file=icsfile\n", argv[0]);
		exit(1);
	}

	from_date = from_y * 10000 + from_m * 100 + from_d;
	to_date = to_y * 10000 + to_m * 100 + to_d;

	readFile(filename, from_date, to_date);
	print_events(from_y, from_m, from_d, to_y, to_m, to_d);

	exit(0);
}
