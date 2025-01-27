#ifndef _TIMEVAL_TOOLS_H_
#define _TIMEVAL_TOOLS_H_

#include <sys/time.h>
#include <string>

#define TIME_NULL timeval{-1, -1}
#define TIME_ZERO timeval{0, 0}

inline bool operator<(const timeval &lhs, const timeval &rhs)
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

inline bool operator>(const timeval &lhs, const timeval &rhs)
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

inline timeval operator+(const timeval &lhs, const timeval &rhs)
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

inline timeval operator-(const timeval &lhs, const timeval &rhs)
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

inline timeval operator*(const timeval &lhs, double rhs)
{
    timeval product;
    product.tv_sec = lhs.tv_sec * rhs;
    product.tv_usec = lhs.tv_usec * rhs;
    if (product.tv_usec >= 1000000)
    {
        product.tv_sec += product.tv_usec / 1000000;
        product.tv_usec %= 1000000;
    }
    return product;
}

inline timeval operator/(const timeval &lhs, double rhs)
{
    timeval quotient;
    quotient.tv_sec = lhs.tv_sec / rhs;
    quotient.tv_usec = lhs.tv_usec / rhs;
    return quotient;
}

inline bool operator==(const timeval &lhs, const timeval &rhs)
{
    return lhs.tv_sec == rhs.tv_sec && lhs.tv_usec == rhs.tv_usec;
}

inline bool operator!=(const timeval &lhs, const timeval &rhs)
{
    return lhs.tv_sec != rhs.tv_sec || lhs.tv_usec != rhs.tv_usec;
}

inline timeval operator+=(timeval &lhs, const timeval &rhs)
{
    lhs.tv_sec += rhs.tv_sec;
    lhs.tv_usec += rhs.tv_usec;
    return lhs;
}

inline int fprint_timeval(FILE *__restrict __stream, const timeval &time)
{
    return fprintf(__stream, "%ld.%06lds", time.tv_sec, time.tv_usec);
}

inline timeval millisec_to_timeval(long msec)
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

#endif