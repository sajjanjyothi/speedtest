/* public header file for the speedtest library */
#ifndef __SPEED_TEST_H__
#define __SPEED_TEST_H__
/**
 * @brief get the bandwidth data for a given URL
 * Public API exposed from library
 * 
 */

// #define SPEED_TEST_DEBUG 1
// #define SPEED_TEST_DIAG 1

#ifdef SPEED_TEST_DEBUG
    #define LOG_MSG(...) fprintf(stderr, __VA_ARGS__)
#else
    #define LOG_MSG(...)
#endif //SPEED_DEBUG

typedef enum
{
    SPEEDTEST_SUCCESS = 0,
    SPEEDTEST_FAILED = -3,
    SPEEDTEST_DATACOLLECT_FAIL,
    SPEEDTEST_CALC_FAIL
}eSPEEDTEST;

eSPEEDTEST getBWData(int count, const char *URL, char *header, char *OutData);
#endif //__SPEED_TEST_H__