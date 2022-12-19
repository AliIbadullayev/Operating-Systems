#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
  
void help(){
    fprintf(stderr, "Usage: ./user structure_ID PID\n "
                    "0 - page_struct\n "
                    "1 - thread_struct\n");
}
  
int main(int argc, char *argv[]){
    if (argc != 3) help();
    if (argc < 3){
        fprintf(stderr, "Not enough arguments \n" );
        return 0;
    }
    if (argc > 3){
        fprintf(stderr, "Too many arguments \n" );
        return 0;
    }
  
    char *p;
    errno = 0;
    long structure_ID = strtol(argv[1], &p, 10);
    if (*p != '\0' || errno != 0){
        fprintf(stderr, "Provided structure_ID must be number.\n");
        help();
        return 0;
    }
    errno = 0;
    long PID = strtol(argv[2], &p, 10);
    if (*p != '\0' || errno != 0){
        fprintf(stderr, "Provided PID must be number.\n");
        help();
        return 0;
    }
 
  
    if (structure_ID !=0 && structure_ID !=1){
        fprintf(stderr, "Provided structure ID is not supported.\n");
        help();
        return 0;
    }
  
    if(PID<0){
        fprintf(stderr, "PID must be positive \n" );
        help();
        return 0;
    }
  
    char inbuf[4096];
    char outbuf[4096];
    int fd = open("/proc/example", O_RDWR);
    sprintf(inbuf, "%s %s", argv[1], argv[2]);
  
    write(fd, inbuf, 17);
    lseek(fd, 0, SEEK_SET);
    read(fd, outbuf, 4096);
  
    if (structure_ID == 0){
        printf("page_struct: \n\n");
    } else {
        printf("thread_struct: \n\n");
    }
    puts(outbuf);
    return 0;
}
