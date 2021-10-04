 #include <stdio.h>
  2 #include <string.h>
  3 #include <stdlib.h>
  4 #include <time.h>
  5 
  6 #define MAX_LINE_LEN 132
  7 #define MAX_EVENTS 500
  8 
  9 
 10 void print_events(int from_yy, int from_mm, int from_dd,
 11     int to_yy, int to_mm, int to_dd)
 12 {
 13 
 14 }
 15 
 16 
 17 int main(int argc, char *argv[])
 18 {
 19     int from_y = 0, from_m = 0, from_d = 0;
 20     int to_y = 0, to_m = 0, to_d = 0;
 21     char *filename = NULL;
 22     int i;
 23 
 24     for (i = 0; i < argc; i++) {
 25         if (strncmp(argv[i], "--start=", 8) == 0) {
 26             sscanf(argv[i], "--start=%d/%d/%d", &from_y, &from_m, &from_d);
 27         } else if (strncmp(argv[i], "--end=", 6) == 0) {
 28             sscanf(argv[i], "--end=%d/%d/%d", &to_y, &to_m, &to_d);
 29         } else if (strncmp(argv[i], "--file=", 7) == 0) {
 30             filename = argv[i]+7;
 31         }
 32     }
 33 
 34     if (from_y == 0 || to_y == 0 || filename == NULL) {
 35         fprintf(stderr,
 36             "usage: %s --start=yyyy/mm/dd --end=yyyy/mm/dd --file=icsfile\n",
 37             argv[0]);
 38         exit(1);
 39     }
 40 
 41     /* Starting calling your own code from this point. */
 42 
 43     exit(0);
 44 }
~                 
