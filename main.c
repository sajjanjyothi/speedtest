#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <speedtest.h>

/******************** Defines *************************/
#define MAX_RESULT_STRING 255
/******************** const defines *******************/
//const char *URL = "http://www.googlesajjan.com";
const char *URL = "http://www.google.com";

void print_usage(char *argv[])
{
    printf("Usage %s: -n count [-H Header-name: Header-value] \n", argv[0]); 
}

int main(int argc, char *argv[])
{
    int count = 1;
    int idx = 0;
    char data[MAX_RESULT_STRING];
    int opt;
    char *header=NULL;
    eSPEEDTEST ret;

    while ( ( opt = getopt(argc, argv,"n:H:h") ) != -1)
    {
        switch(opt)
        {
            case 'n':
                count = atoi(optarg);
                break;
            case 'H':
                header = optarg;
                break;
            case 'h':
                print_usage(argv);
                exit(0);
                break;
            case 0:
                print_usage(argv);
                exit(-1);
                break;
            default:
                print_usage(argv);
                exit(-1);
                break;
        }
    }

    /* usage of library */
    ret = getBWData(count, URL,header, data);
    if( ret != SPEEDTEST_SUCCESS )
    {
        printf("\n getBWData failed = %d \n", ret);
    }
    
    printf("%s\n", data);

    return 0;
}
