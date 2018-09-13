#include <stdlib.h>
#include <curl/curl.h>
#include <speedtest.h>

/************ Database ********************************/
typedef struct ResultValue
{
    char *ipAddress;
    long httpResponseCode;
    double *CURLINFO_NAMELOOKUP_TIME;
    double *CURLINFO_CONNECT_TIME;
    double *CURLINFO_STARTTRANSFER_TIME;
    double *CURLINFO_TOTAL_TIME;
} RESULT;

/******************** Function Prototypes , All internal functions are made as static for not exposing it to the extrenal world*************/
static int getResultInfo(CURL *curl, RESULT *rInfo, int count);
static int collectData(RESULT *rInfo, int count, char *header, const char *URL);
static char *calculateMedian(int count, RESULT *rInfo , char *);

static int getResultInfo(CURL *curl, RESULT *rInfo, int count)
{
    if ( curl_easy_getinfo( curl, CURLINFO_PRIMARY_IP , &(rInfo->ipAddress) ) != CURLE_OK )
    {
        LOG_MSG("HTTP CURLINFO_PRIMARY_IP not found");
        return -1;
    }
    if ( curl_easy_getinfo( curl, CURLINFO_RESPONSE_CODE , &(rInfo->httpResponseCode) ) != CURLE_OK )
    {
        LOG_MSG("HTTP response code not found");
        return -1;
    }
    else if ( curl_easy_getinfo( curl, CURLINFO_NAMELOOKUP_TIME , ( ( rInfo->CURLINFO_NAMELOOKUP_TIME) + count ) ) != CURLE_OK )
    {
        LOG_MSG("CURLINFO_NAMELOOKUP_TIME not found");
        return -1;
    }
    else if ( curl_easy_getinfo( curl, CURLINFO_CONNECT_TIME ,  ( (rInfo->CURLINFO_CONNECT_TIME) + count )  ) != CURLE_OK )
    {
        LOG_MSG("CURLINFO_CONNECT_TIME not found");
        return -1;
    }
    else if ( curl_easy_getinfo( curl, CURLINFO_STARTTRANSFER_TIME , ( (rInfo->CURLINFO_STARTTRANSFER_TIME) + count ) ) != CURLE_OK )
    {
        LOG_MSG("CURLINFO_STARTTRANSFER_TIME not found");
        return -1;
    }
    else if ( curl_easy_getinfo( curl, CURLINFO_TOTAL_TIME , ( ( rInfo->CURLINFO_TOTAL_TIME) + count ) ) != CURLE_OK )
    {
        LOG_MSG("CURLINFO_TOTAL_TIME not found");
        return -1;
    }
    return 0;
}


static int collectData(RESULT *rInfo, int count, char *header, const char *URL)
{
    struct curl_slist *headerList = NULL;
    CURL *curl = curl_easy_init();
    int cntIdx = 0;

    if (curl)
    {
        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_URL, URL);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1);//Hide stdout
      
        if(header)
        {
            headerList = curl_slist_append(headerList, header);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
        }
    
        for ( cntIdx = 0; cntIdx < count ; cntIdx ++)
        {
            res = curl_easy_perform(curl);
            if(CURLE_OK == res)
            {
                if ( getResultInfo(curl, rInfo, cntIdx) )
                {
                    LOG_MSG("getResultInfo failed\n");
                    curl_slist_free_all(headerList);
                    curl_easy_cleanup(curl);
                    return -1;
                }
            }
            else
            {
                LOG_MSG("curl_easy_perform failed, curl return code %d", res);
                curl_slist_free_all(headerList);
                curl_easy_cleanup(curl);
                return -1;
            }
        } 
        curl_easy_cleanup(curl);
        curl_slist_free_all(headerList);
    }

    return 0;
}

/* qsort compare values */
static int compareValues(const void *val1, const void *val2)
{

    return *((double*)val1) > *((double*)val2);
}

/* Sort values and get median out of it */
static double getMedian(double *values, int count)
{
    /* Sort the values in ascending order */
    qsort(values, count, sizeof(double),compareValues);

#ifdef SPEED_TEST_DIAG
    int idx = 0;
    printf("\n========================\n");
    for ( idx = 0 ; idx < count ; idx ++)
    {
        printf("%f\n", values[idx]);
    }
    printf("\n========================\n");
#endif 

    if( ( count % 2 ) == 0 ) 
    {
        return((values[count/2] + values[count /2 - 1]) / 2.0);
    } 
    else 
    {
        return values[count/2];
    }
}

static char *calculateMedian(int count, RESULT *rInfo, char *result)
{
    int idx = 0;
    char *fmt = "SKTEST;%s;%d;%f;%f;%f;%f";
    char *ipAddress;
    long responseCode = 0;

    double CURLINFO_NAMELOOKUP_TIME = 0.0;
    double CURLINFO_CONNECT_TIME = 0.0;
    double CURLINFO_STARTTRANSFER_TIME = 0.0;
    double CURLINFO_TOTAL_TIME = 0.0;

    ipAddress = rInfo->ipAddress;
    responseCode = rInfo->httpResponseCode;
    CURLINFO_NAMELOOKUP_TIME = getMedian(rInfo->CURLINFO_NAMELOOKUP_TIME, count);
    CURLINFO_CONNECT_TIME = getMedian(rInfo->CURLINFO_CONNECT_TIME,count);
    CURLINFO_STARTTRANSFER_TIME = getMedian(rInfo->CURLINFO_STARTTRANSFER_TIME, count);
    CURLINFO_TOTAL_TIME = getMedian(rInfo->CURLINFO_TOTAL_TIME, count);

    sprintf(result, fmt, ipAddress,responseCode,CURLINFO_NAMELOOKUP_TIME,
                                                CURLINFO_CONNECT_TIME,
                                                CURLINFO_STARTTRANSFER_TIME,
                                                CURLINFO_TOTAL_TIME);
    return result;

}

static int freeRInfo(RESULT *rInfo)
{
    if(!rInfo) abort();

    if(rInfo->CURLINFO_NAMELOOKUP_TIME) 
    {
        free(rInfo->CURLINFO_NAMELOOKUP_TIME);
        rInfo->CURLINFO_NAMELOOKUP_TIME = NULL;
    }
    if(rInfo->CURLINFO_CONNECT_TIME) 
    {
        free(rInfo->CURLINFO_CONNECT_TIME);
        rInfo->CURLINFO_CONNECT_TIME = NULL;
    }
    if(rInfo->CURLINFO_STARTTRANSFER_TIME) 
    {
        free(rInfo->CURLINFO_STARTTRANSFER_TIME);
        rInfo->CURLINFO_STARTTRANSFER_TIME = NULL;
    }
    if(rInfo->CURLINFO_TOTAL_TIME) 
    {
        free(rInfo->CURLINFO_TOTAL_TIME);
        rInfo->CURLINFO_TOTAL_TIME = NULL;
    }

    free(rInfo);

    return 0;
}

static RESULT * allocResult( int count)
{
    RESULT *rInfo = NULL;

    rInfo = (RESULT *)calloc(1, sizeof(RESULT) );

    if(!rInfo) abort();

    rInfo->CURLINFO_NAMELOOKUP_TIME = (double *) malloc(sizeof(double)*count);
    rInfo->CURLINFO_CONNECT_TIME = (double *) malloc(sizeof(double)*count);
    rInfo->CURLINFO_STARTTRANSFER_TIME = (double *) malloc(sizeof(double)*count);
    rInfo->CURLINFO_TOTAL_TIME = (double *) malloc(sizeof(double)*count);

    return rInfo;
}


eSPEEDTEST getBWData(int count, const char *URL, char *header, char *OutData)
{
    RESULT *rInfo;
    
    rInfo = allocResult(count);

    LOG_MSG("header value [%s] \n", header);
    if (rInfo)
    {
        if (collectData(rInfo, count, header, URL) < 0)
        {
            LOG_MSG("Data collection failed !!!!!\n");
            freeRInfo(rInfo);
            return SPEEDTEST_DATACOLLECT_FAIL;
        }
        if (calculateMedian(count, rInfo, OutData) < 0)
        {
            freeRInfo(rInfo);
            LOG_MSG("calculateMedian failed !!!!!\n");
            return SPEEDTEST_CALC_FAIL;
        }
        freeRInfo(rInfo);
    }
    else
    {
        LOG_MSG("\n Out of memory check your free memory please !!!!\n");
        return SPEEDTEST_FAILED;
    }

    return SPEEDTEST_SUCCESS; 
}

