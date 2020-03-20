//
// Created by hyhu on 2020/2/18.
//

#ifndef HTTPDEMO_HTTP_TIME_H
#define HTTPDEMO_HTTP_TIME_H

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

static const char * wdays[7] = {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",
};

static const char * months[12] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
};

static void make_http_time (time_t t, char * buf, int buf_len)
{
    int bytes;
    struct tm g = {0};
    gmtime_r (& t, & g);
    if (g.tm_wday >= 7 ||
        g.tm_mon >= 12) {
        fprintf (stderr, "g.tm_wday = %d, g.tm_mon = %d.\n",
                 g.tm_wday, g.tm_mon);
        exit (EXIT_FAILURE);
    }
    bytes = snprintf (buf, buf_len,
                      "%.3s, %02d %.3s %4d %02d:%02d:%02d GMT",
                      wdays[g.tm_wday], g.tm_mday, months[g.tm_mon],
                      g.tm_year + 1900, g.tm_hour, g.tm_min, g.tm_sec);
    if (bytes >= buf_len) {
        fprintf (stderr, "bytes = %d, buf_len = %d.\n",
                 bytes, buf_len);
        exit (EXIT_FAILURE);
    }
}


static void parse_http_time (const char * buf, time_t * t_ptr)
{
    struct tm g = {0};
    char M[4];
    time_t t;
    int i;

    sscanf(buf, "%*[a-zA-Z,] %d %3s %d %d:%d:%d",
           & g.tm_mday, M, & g.tm_year,
           & g.tm_hour, & g.tm_min, & g.tm_sec);
    for (i = 0; i < 12; i++) {
        if (strncmp (M, months[i], 3) == 0) {
            g.tm_mon = i;
            break;
        }
    }
    g.tm_year -= 1900;
    t = timegm (& g);
    * t_ptr = t;
}

const char * p = "Thu, 03 Feb 1994 00:20:00 GMT";

#endif //HTTPDEMO_HTTP_TIME_H
