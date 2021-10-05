#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LINE_LEN 132
#define MAX_EVENTS 500


struct event_t {
    char dt_start[MAX_LINE_LEN];
    char dt_end[MAX_LINE_LEN];
    char until[MAX_LINE_LEN];
    char summary[MAX_LINE_LEN];
    char location[MAX_LINE_LEN];
    int repeat;
};

struct event_t entry[MAX_EVENTS];
int numEvents = 0;

/*
 * Function dt_format from timeplay.c
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

void dt_format(char *formatted_date, const char *dt_time, const int len) {
    struct tm temp_time;
    time_t full_time;
    char temp[5];

    memset(&temp_time, 0, sizeof(struct tm));
    sscanf(dt_time, "%4d%2d%2d", &temp_time.tm_year, &temp_time.tm_mon, &temp_time.tm_mday);
    temp_time.tm_year -= 1900;
    temp_time.tm_mon -= 1;
    full_time = mktime(&temp_time);
    strftime(formatted_date, len, "%B %d, %Y (%a)", localtime(&full_time));

}

void dateFormat(char* d, char* src) {
    char formattedDate[MAX_LINE_LEN];
    dt_format(formattedDate, src, MAX_LINE_LEN);
    sprintf(d, "%s\n", formattedDate);
    
    // Add dashes underneath the date matching the length
    for (int i=0; i < strlen(formattedDate); i++) {
        sprintf(d + strlen(d), "-");
    }
    sprintf(d + strlen(d), "\n");
}

void read(char* filename) {
    int event = 0;
    int i=0;
    char* line = NULL;
    size_t length = 0;
    FILE* calendar;
    calendar = fopen(filename, "r");

    while (getline(&line, &length, calendar) != -1) { 
        
        if (strncmp(line, "BEGIN:VEVENT", strlen("BEGIN:VEVENT")) == 0) {
            event = 1;
        }
      
        if (event && strncmp(line, "DTSTART:", strlen("DTSTART:")) == 0) {
            strcpy(entry[i].dt_start, line + strlen("DTSTART:"));
        }
        
        if (event && strncmp(line, "DTEND:", strlen("DTEND:")) == 0) {
            strcpy(entry[i].dt_end, line + strlen("DTEND:"));
        }

        char* f = strstr(line, "UNTIL=");
        if (event && f != NULL) {
            strncpy(entry[i].until, f + strlen("UNTIL="), MAX_LINE_LEN-1);
            entry[i].until[MAX_LINE_LEN] = '\0';
            entry[i].repeat = 1;
        }

        if (event && strncmp(line, "LOCATION:", strlen("LOCATION:")) == 0) {
            strcpy(entry[i].location, line + strlen("LOCATION:"));
        }
       
        if (event && strncmp(line, "SUMMARY:", strlen("SUMMARY:")) == 0) {
            strcpy(entry[i].summary, line + strlen("SUMMARY:"));
        }

        if (strncmp(line, "END:VEVENT", strlen("END:VEVENT")) == 0) {
            event = 0;
            i++;
        }
    }
    numEvents = i;

    fclose(calendar);
}

int compare(const void *one, const void *two) {
    const struct event_t* part1 = (struct event_t*)one;
  
    const struct event_t* part2 = (struct event_t*)two;
  
    return strcmp(part1->DTSTART, part2->DTSTART);
}

void add(char* newDate, char* date, int numWeeks) {

    char day[3];
    strncpy(day, date+6, 2);
    day[2] = '\0';
    char month[3];
    strncpy(month, date+4, 2);
    month[2] = '\0';
    char year[5];
    strncpy(year, date, 4);
    year[4] = '\0';
    int y=atoi(year), m=atoi(month), d=atoi(day);
    struct tm t = { .tm_year=y-1900, .tm_mon=m-1, .tm_mday=d };
    t.tm_mday += 7 * numWeeks;
    mktime(&t);

    sprintf(newDate, "%d%02d%02d%s", t.tm_year+1900, t.tm_mon+1, t.tm_mday, date+8);
    return;
}


void repeating() {
    int num_events_curr = numEvents;
    for (int i=0; i < num_events_curr; i++) {
        if (entry[i].repeat) {
            struct event_t temp = entry[i];
            while (strcmp(temp.dt_start, entry[i].until) <= 0) {
                add(entry[numEvents].dt_start, temp.dt_start, 1);
                add(entry[numEvents].dt_end, temp.dt_end, 1);
                entry[numEvents].repeat = 0;
                strcpy(entry[numEvents].location, entry[i].location);
                strcpy(entry[numEvents].summary, entry[i].summary);
                temp = entry[numEvents];
                numEvents++;
            }

            if (strcmp(entry[numEvents-1].dt_start, entry[i].until) > 0)
                numEvents--;
        }
    }
}


void sort() {
    qsort(entry, numEvents, sizeof(struct event_t), compare);
}

void timeFormat(char* d, char* src) {
    char hourS[3];
    strncpy(hourS, src + 9, 2);
    hourS[2] = '\0';
    int hour = atoi(hourS);

    char* am_or_pm = "AM";
    if (hour >= 12) {
        if (hour > 12)
            hour -= 12;
        am_or_pm = "PM";
    }

    char minS[3];
    strncpy(minS, src + 11, 2);
    minS[2] = '\0';
    int min = atoi(minS);

    sprintf(d, "%2d:%02d %s", hour, min, am_or_pm);
}


void print_events(int from_yy, int from_mm, int from_dd, int to_yy, int to_mm, int to_dd) {
 
    char from_date[MAX_LINE_LEN+1];
    sprintf(from_date, "%d%02d%02d%s", from_yy, from_mm, from_dd, "T000000");
    char to_date[MAX_LINE_LEN+1];
    sprintf(to_date, "%d%02d%02d%s", to_yy, to_mm, to_dd, "T235959");
    
    char curr_date[MAX_LINE_LEN];
    curr_date[0] = '\0';

    for (int i=0; i < numEvents; i++) {
        if (strcmp(entry[i].dt_start, from_date) >= 0 && strcmp(entry[i].dt_start, to_date) <= 0) {
            char formattedDate[MAX_LINE_LEN];
            dateFormat(formattedDate, entry[i].dt_start);
            if (strcmp(curr_date, formattedDate) != 0) {
                if (curr_date[0] != '\0')
                    printf("\n");
                printf("%s", formattedDate);
                strcpy(curr_date, formattedDate);
            }
            char start[MAX_LINE_LEN], end[MAX_LINE_LEN];
            timeFormat(start, entry[i].dt_start);
            timeFormat(end, entry[i].dt_end); 
            entry[i].summary[strlen(entry[i].summary)-1] = '\0';
            entry[i].location[strlen(entry[i].location)-1] = '\0';
            printf("%s to %s: %s {{%s}}\n", start, end, entry[i].summary, entry[i].location);
        }
    }
}


int main(int argc, char *argv[]) {
    int from_y = 0, from_m = 0, from_d = 0;
    int to_y = 0, to_m = 0, to_d = 0;
    char* filename = NULL;
    int i; 

    for (i=0; i < argc; i++) {
        if (strncmp(argv[i], "--start=", 8) == 0) {
            sscanf(argv[i], "--start=%d/%d/%d", &from_y, &from_m, &from_d);
        } else if (strncmp(argv[i], "--end=", 6) == 0) {
            sscanf(argv[i], "--end=%d/%d/%d", &to_y, &to_m, &to_d);
        } else if (strncmp(argv[i], "--file=", 7) == 0) {
            filename = argv[i]+7;
        }
    }

    if (from_y == 0 || to_y == 0 || filename == NULL) {
        fprintf(stderr, 
            "usage: %s --start=yyyy/mm/dd --end=yyyy/mm/dd --file=icsfile\n",
            argv[0]);
        exit(1);
    }

    read(filename);
    repeating();
    sort();
    print_events(from_y, from_m, from_d, to_y, to_m, to_d);

    exit(0);
}
