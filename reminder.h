// reminder.h
#ifndef REMINDER_H
#define REMINDER_H

#include <time.h>  // Needed for time_t

void addReminder(char* text, time_t timestamp, int priority, int userId);
void removeTopReminder();

#endif
