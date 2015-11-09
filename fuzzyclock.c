/* cc -std=c99 -pedantic -Wall -O2 -D_POSIX_C_SOURCE=200809L */
#include "fuzzyclock.h"

typedef enum {
	FULL_PAST = 0,
	QUARTER_PAST,
	HALF,
	QUARTER_BEFORE,
    FULL
} clock_state_t;

typedef struct {
	int hour;
	int minute;
} fuzzy_clock_t;

static inline int
clock_increment_hour(int hour)
{
	/* Doesn't check for over/underflows, I know... */
	hour++;

	if (hour > 23) return 0;
	else return hour;
}

static char *
clock_get_hour_name(int hour)
{
	char *hournames[] = {
		"one",
		"two",
		"three",
		"four",
		"five",
		"six",
		"seven",
		"eight",
		"nine",
		"ten",
		"eleven",
		"twelve"
	};

	if (hour == 0) {
		return strdup(hournames[11]);
	}
	else if (hour > 0 && hour <= 12) {
		return strdup(hournames[hour-1]);
	}
	else if (hour > 12 && hour <= 23) {
		return strdup(hournames[hour-13]);
	}
	else {
		return NULL;
	}
}

static clock_state_t
clock_get_state(fuzzy_clock_t *clock)
{
	int minute = clock->minute;

	if (minute >= 0 && minute < 7) {
		return FULL_PAST;
	}
	else if (minute >= 7 && minute <= 23) {
		return QUARTER_PAST;
	}
	else if (minute > 23 && minute <= 38) {
		return HALF;
	}
	else if (minute > 38 && minute <= 53) {
		return QUARTER_BEFORE;
	}
	else {
		return FULL;
	}
}

static char *
clock_get_fuzzy_hour(fuzzy_clock_t *clock)
{
	clock_state_t state = clock_get_state(clock);

	if (state >= QUARTER_BEFORE) {
		return clock_get_hour_name(clock_increment_hour(clock->hour));
	}
	else {
		return clock_get_hour_name(clock->hour);
	}
}

char *
fuzzytime(struct tm *tm)
{
	struct tm *timeinfo;
	clock_state_t state;
	char timestring[64];

	if (!tm) {
		time_t rawtime;

		time(&rawtime);
		timeinfo = localtime(&rawtime);
	}
	else {
		timeinfo = tm;
	}

	fuzzy_clock_t clock = {
		.hour = timeinfo->tm_hour,
		.minute = timeinfo->tm_min
	};
	state = clock_get_state(&clock);

	switch(state) {
	case QUARTER_PAST:
		snprintf(timestring, 64, "quarter past %s", clock_get_fuzzy_hour(&clock));
		break;
	case HALF:
		snprintf(timestring, 64, "half past %s", clock_get_fuzzy_hour(&clock));
		break;
	case QUARTER_BEFORE:
		snprintf(timestring, 64, "quarter before %s", clock_get_fuzzy_hour(&clock));
		break;
	case FULL_PAST:
	case FULL:
		snprintf(timestring, 64, "%s o'clock", clock_get_fuzzy_hour(&clock));
		break;
	}

	return strdup(timestring);
}
