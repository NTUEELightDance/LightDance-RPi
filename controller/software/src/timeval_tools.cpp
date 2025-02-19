#include "timeval_tools.h"
#include <cstdio>

bool operator<(const timeval &lhs, const timeval &rhs)
{
    if (lhs.tv_sec < rhs.tv_sec)
    {
        return true;
    }
    else if (lhs.tv_sec == rhs.tv_sec)
    {
        return lhs.tv_usec < rhs.tv_usec;
    }
    return false;
}

bool operator>(const timeval &lhs, const timeval &rhs)
{
    if (lhs.tv_sec > rhs.tv_sec)
    {
        return true;
    }
    else if (lhs.tv_sec == rhs.tv_sec)
    {
        return lhs.tv_usec > rhs.tv_usec;
    }
    return false;
}

timeval operator+(const timeval &lhs, const timeval &rhs)
{
    timeval sum;
    sum.tv_sec = lhs.tv_sec + rhs.tv_sec;
    sum.tv_usec = lhs.tv_usec + rhs.tv_usec;
    if (sum.tv_usec >= 1000000)
    {
        sum.tv_sec++;
        sum.tv_usec -= 1000000;
    }
    return sum;
}

timeval operator-(const timeval &lhs, const timeval &rhs)
{
    timeval diff;
    diff.tv_sec = lhs.tv_sec - rhs.tv_sec;
    diff.tv_usec = lhs.tv_usec - rhs.tv_usec;
    if (diff.tv_usec < 0)
    {
        diff.tv_sec--;
        diff.tv_usec += 1000000;
    }
    return diff;
}

timeval operator*(const timeval &lhs, double rhs)
{
    unsigned long result_usec = rhs * (lhs.tv_sec * 1000000 + lhs.tv_usec);
    timeval product;
    product.tv_sec = result_usec / 1000000;
    product.tv_usec = result_usec % 1000000;
    return product;
}

timeval operator/(const timeval &lhs, double rhs)
{
    unsigned long result_usec = (lhs.tv_sec * 1000000 + lhs.tv_usec) / rhs;
    timeval quotient;
    quotient.tv_sec = result_usec / 1000000;
    quotient.tv_usec = result_usec % 1000000;
    return quotient;
}

bool operator==(const timeval &lhs, const timeval &rhs)
{
    return lhs.tv_sec == rhs.tv_sec && lhs.tv_usec == rhs.tv_usec;
}

bool operator!=(const timeval &lhs, const timeval &rhs)
{
    return lhs.tv_sec != rhs.tv_sec || lhs.tv_usec != rhs.tv_usec;
}

timeval operator+=(timeval &lhs, const timeval &rhs)
{
    lhs.tv_sec += rhs.tv_sec;
    lhs.tv_usec += rhs.tv_usec;
    return lhs;
}

int fprint_timeval(FILE *__restrict __stream, const timeval &time)
{
    return fprintf(__stream, "%ld.%06lds", time.tv_sec, time.tv_usec);
}

timeval millisec_to_timeval(long msec)
{
    if(msec < 0)
    {
        return TIME_NULL;
    }
    timeval time;
    time.tv_sec = msec / 1000;
    time.tv_usec = (msec % 1000) * 1000;
    return time;
}