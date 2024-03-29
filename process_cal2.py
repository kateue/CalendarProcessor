#!/usr/bin/env python3

import argparse
import datetime as dt

#reads file line by line, and checks for repeating events
def read(filename):
    check = False
    calendar = []
    file = open(filename, mode='r')
    for line in file:
    
        if line == "BEGIN:VEVENT\n":
            check = True
            calendar.append({'REPEAT': False})
       
        if check and line[:8] == "DTSTART:":
            calendar[-1]['DTSTART'] = line[8:-1]

        if check and line[:6] == "DTEND:":
            calendar[-1]['DTEND'] = line[6:-1]

        if check and line[:9] == "LOCATION:":
            calendar[-1]['LOCATION'] = line[9:-1]

        if check and line[:8] == "SUMMARY:":
            calendar[-1]['SUMMARY'] = line[8:-1]

        if check and "UNTIL" in line:
            until_idx = line.find("UNTIL")
            calendar[-1]['UNTIL'] = line[until_idx+6 : until_idx+21]
            calendar[-1]['REPEAT'] = True

        if line[:-1] == "END:VEVENT":
            check = False
    
    file.close()
    return calendar

#deals with repeating events through weekly incrememts
def repeating(calendar):
    numEvents = []
    for event in calendar:
        if event['REPEAT'] == True:
            # Get the dates on which the event occurs
            repeat = []
            dt_until = dt.datetime.strptime(event['UNTIL'][:8], '%Y%m%d')
            tmp = dt.datetime.strptime(event['DTSTART'][:8], '%Y%m%d') + dt.timedelta(weeks=1)
            while tmp <= dt_until:
                repeat.append(tmp)
                tmp += dt.timedelta(weeks=1)

            # Create a new event for each repeating date
            for date in repeat:
                numEvents.append({
                    'DTSTART': date.strftime('%Y%m%d') + event['DTSTART'][8:],
                    'DTEND': date.strftime('%Y%m%d') + event['DTEND'][8:],
                    'REPEAT': event['REPEAT'],
                    'UNTIL': event['UNTIL'],
                    'LOCATION': event['LOCATION'],
                    'SUMMARY': event['SUMMARY']
                })
    calendar.extend(numEvents)
    return calendar


def dateFormat(date):
    '''
    Takes a datetime object and formats it properly for output,
    adding dashes underneath the formatted date.
    '''

    formatted_date = date.strftime('%B %d, %Y (%a)') + '\n'
    for _ in range(len(formatted_date)-1):
        formatted_date += '-'  # add dashes
    return formatted_date


def print(calendar, from_dt, to_dt):
    '''
    Main output function
    '''

    current_date = None
    for event in calendar:
        event_date = dt.datetime.strptime(event['DTSTART'][:8], '%Y%m%d')
        
        # Check if event falls within specified date range
        if event_date >= from_dt and event_date <= to_dt:

            # Check if the event date is different from previous event (i.e. print new date line)
            if event_date != current_date:
                if current_date is not None:
                    print()
                print(dateFormat(event_date))
                current_date = event_date
            
            start_time = dt.datetime.strptime(event['DTSTART'][9:], '%H%M%S')
            end_time = dt.datetime.strptime(event['DTEND'][9:], '%H%M%S')
            start_time_str = start_time.strftime('%-I:%M %p')
            end_time_str = end_time.strftime('%-I:%M %p')

            # Add initial space if single digit hour
            if start_time_str[1] == ':':
                start_time_str = ' ' + start_time_str
            if end_time_str[1] == ':':
                end_time_str = ' ' + end_time_str
            
            event_str = f"{start_time_str} to {end_time_str}: {event['SUMMARY']} "
            event_str += "{{" + event['LOCATION'] + "}}"
            print(event_str)
            

def main():
    '''
    Parses command line arguments for file name, start date, and end date.
    Gets all events in the date range using helper functions and prints 
    out in proper format.
    '''

    parser = argparse.ArgumentParser()
    parser.add_argument('--file', type=str, help='file to be processed')
    parser.add_argument('--start', type=str, help='start of date range')
    parser.add_argument('--end', type=str, help='end of data range')
    args = parser.parse_args()

    need_arg = False
    if not args.file:
        print("Need --file=<ics filename>")
        need_arg = True
    if not args.start:
        print("Need --start=dd/mm/yyyy")
        need_arg = True
    if not args.end:
        print("Need --end=dd/mm/yyyy")
        need_arg = True
    if need_arg:
        return
    
    start_date = dt.datetime.strptime(args.start, '%Y/%m/%d')
    end_date = dt.datetime.strptime(args.end, '%Y/%m/%d')

    all_events = read(args.file)
    all_events = repeating(all_events)
    all_events.sort(key= lambda event: event['DTSTART'])

    print(all_events, start_date, end_date)


if __name__ == "__main__":
    main()
