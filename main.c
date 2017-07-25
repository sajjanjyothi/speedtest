#include <stdio.h>
#include <unistd.h>
/******************** const defines *******************/
const char *URL = "http://www.google.com";

int main(int argc, char *argv[])
{
    int count = 1;
    int idx = 0;
    char data[255];
    int opt;
    char *header="";/* I haven't used this header at the moment*/

    while ( ( opt = getopt(argc, argv,"n:H") ) != -1)
    {
        switch(opt)
        {
            case 'n':
                count = atoi(optarg);
                break;
            case 'H':
                header = optarg;
                break;
            default:
                printf("Usage %s: -n count [-H Header-name: Header-value] \n");
                exit(-1);
                break;
        }
    }
    /* usage of library */
    getBWData(count, URL, data);
    
    printf("%s\n", data);

    return 0;
}
