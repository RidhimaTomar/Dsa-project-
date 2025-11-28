#include <stdio.h>
#include <stdlib.h>
#include "globals.h"

time_t parse_time_string(const char* time_str) {
    struct tm tm_info = {0};
    time_t now = time(NULL);
    struct tm* now_tm = localtime(&now);
    
    tm_info.tm_year = now_tm->tm_year;
    tm_info.tm_mon = now_tm->tm_mon;
    tm_info.tm_mday = now_tm->tm_mday;
    
    sscanf(time_str, "%d:%d", &tm_info.tm_hour, &tm_info.tm_min);
    
    return mktime(&tm_info);
}

void get_current_date(char* date_str) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(date_str, 32, "%Y-%m-%d", tm_info);
}
