#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

/************ Database ********************************/
typedef struct ResultValue
{
    char *ipAddress;
    long httpResponseCode;
    double CURLINFO_NAMELOOKUP_TIME;
    double CURLINFO_CONNECT_TIME;
    double CURLINFO_STARTTRANSFER_TIME;
    double CURLINFO_TOTAL_TIME;
} RESULT;




/******************** Function Prototypes *************/
static int getResultInfo(CURL *curl, RESULT *rInfo);
static int collectData(RESULT *rInfo, int count, const char *URL);
static char *calculateMedian(int count, RESULT *rInfo , char *);

static int getResultInfo(CURL *curl, RESULT *rInfo)
{
    if ( curl_easy_getinfo( curl, CURLINFO_PRIMARY_IP , &(rInfo->ipAddress) ) != CURLE_OK )
    {
        printf("HTTP CURLINFO_PRIMARY_IP not found");
        return -1;
    }
    if ( curl_easy_getinfo( curl, CURLINFO_RESPONSE_CODE , &(rInfo->httpResponseCode) ) != CURLE_OK )
    {
        printf("HTTP response code not found");
        return -1;
    }
    else if ( curl_easy_getinfo( curl, CURLINFO_NAMELOOKUP_TIME , &(rInfo->CURLINFO_NAMELOOKUP_TIME) ) != CURLE_OK )
    {
        printf("CURLINFO_NAMELOOKUP_TIME not found");
        return -1;
    }
    else if ( curl_easy_getinfo( curl, CURLINFO_CONNECT_TIME , &(rInfo->CURLINFO_CONNECT_TIME) ) != CURLE_OK )
    {
        printf("CURLINFO_CONNECT_TIME not found");
        return -1;
    }
    else if ( curl_easy_getinfo( curl, CURLINFO_STARTTRANSFER_TIME , &(rInfo->CURLINFO_STARTTRANSFER_TIME) ) != CURLE_OK )
    {
        printf("CURLINFO_STARTTRANSFER_TIME not found");
        return -1;
    }
    else if ( curl_easy_getinfo( curl, CURLINFO_TOTAL_TIME , &(rInfo->CURLINFO_TOTAL_TIME) ) != CURLE_OK )
    {
        printf("CURLINFO_TOTAL_TIME not found");
        return -1;
    }
    return 0;
}

/*
 * This function can reimplement if we are not using libcurl
 */
static int collectData(RESULT *rInfo, int count, const char *URL)
{
    CURL *curl = curl_easy_init();
    int cntIdx = 0;
    if (curl)
    {
        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_URL, URL);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1);//Hide stdout
        for ( cntIdx = 0; cntIdx < count ; cntIdx ++)
        {
            res = curl_easy_perform(curl);
            if(CURLE_OK == res)
            {
                if ( getResultInfo(curl, rInfo) < -1)
                {
                    curl_easy_cleanup(curl);
                    return -1;
                }
                rInfo ++;
            }
            else
            {
                printf("curl_easy_perform failed, %d", res);
                curl_easy_cleanup(curl);
                return -1;
            }
        } 
        curl_easy_cleanup(curl);
    }

    return 0;
}

static char *calculateMedian(int count, RESULT *rInfo, char *result)
{
    int idx = 0;
    char *s = "SKTEST;%s;%d;%f;%f;%f;%f";
    char *ipAddress;
    long resPonseCode = 0;

    double CURLINFO_NAMELOOKUP_TIME = 0.0;
    double CURLINFO_CONNECT_TIME = 0.0;
    double CURLINFO_STARTTRANSFER_TIME = 0.0;
    double CURLINFO_TOTAL_TIME = 0.0;

    ipAddress = rInfo->ipAddress;
    resPonseCode = rInfo->httpResponseCode;
    for (idx = 0; idx < count ; idx++)
    {
        CURLINFO_NAMELOOKUP_TIME += rInfo->CURLINFO_NAMELOOKUP_TIME;
        CURLINFO_CONNECT_TIME += rInfo->CURLINFO_CONNECT_TIME;
        CURLINFO_STARTTRANSFER_TIME += rInfo->CURLINFO_STARTTRANSFER_TIME;
        CURLINFO_TOTAL_TIME += rInfo->CURLINFO_TOTAL_TIME;
        rInfo ++;
    }
    sprintf(result, s, ipAddress,resPonseCode,(
                                                CURLINFO_NAMELOOKUP_TIME/count),
                                                (CURLINFO_CONNECT_TIME/count),
                                                (CURLINFO_STARTTRANSFER_TIME/count),
                                                (CURLINFO_TOTAL_TIME/count) 
                                                );
    return result;

}


int getBWData(int count, const char *URL, char *OutData)
{
    RESULT *rInfo;
/*
 *  Initialise array with n elements
 */
    rInfo = (RESULT *)malloc(sizeof(RESULT) * count);
    if( collectData(rInfo,count, URL) < 0)
    {
        printf("Data collection failed !!!!!\n");
        free (rInfo);
        return -1;
    }
    if ( calculateMedian(count, rInfo, OutData) < 0)
    {
        free (rInfo);
        return -1;
    }
    return 0; 
}
