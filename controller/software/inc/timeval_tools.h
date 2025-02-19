// these are the custom functions for timeval struct

#ifndef _TIMEVAL_TOOLS_H_
#define _TIMEVAL_TOOLS_H_

#include <sys/time.h>
#include <string>

#define TIME_NULL timeval{-1, -1}
#define TIME_ZERO timeval{0, 0}

bool operator<(const timeval &lhs, const timeval &rhs);
bool operator>(const timeval &lhs, const timeval &rhs);
timeval operator+(const timeval &lhs, const timeval &rhs);
timeval operator-(const timeval &lhs, const timeval &rhs);
timeval operator*(const timeval &lhs, double rhs);
timeval operator/(const timeval &lhs, double rhs);
bool operator==(const timeval &lhs, const timeval &rhs);
bool operator!=(const timeval &lhs, const timeval &rhs);
timeval operator+=(timeval &lhs, const timeval &rhs);
int fprint_timeval(FILE *__restrict __stream, const timeval &time);
timeval millisec_to_timeval(long msec);

#endif
