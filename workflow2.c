#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

void file_workflow2(const char *file_name)
{
    char data[] = { '1', '2', '3'};
    #define DATA_SIZE (sizeof(data) / sizeof(data[0]))
    char data_read[DATA_SIZE] = {};
    int fd = -1;
    int result = -1;
   
    printf("\n\n");
    printf("File name: %s\n", file_name);
    printf("Data size: %lu\n", (unsigned long int)DATA_SIZE);
    printf("\n");

    printf("Prepare to writing data file...\n");

    printf("Getting stat...\n");
    struct stat st0 = {}; 
    if (stat(file_name, &st0) == -1) {
        perror("\tGot error");
        return;
    }
    printf("file size: %lld\n", (long long int)st0.st_size);
    
    printf("Opening file...\n");
    fd = open(file_name,
              O_RDWR | O_SYNC,
              S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IROTH);
    if (fd == -1) {
        perror("\tGot error");
        if (errno == ENOSPC || errno == EDQUOT) {
            printf("\tError: ENOSPC/EDQUOT\n");
        }
        return;
    }

    printf("Writing data to file...\n");
    ssize_t len = DATA_SIZE;
    ssize_t ret = 0;
    char *buf = data;
    while (len != 0
        && ((ret = write(fd, buf, len)) != DATA_SIZE)) {
        printf("\tError: Not all data had been written (ret = %zd)\n", ret);
        if (ret == -1) {
            perror("\tGot error");
            if (errno == EINTR) {
                continue;
            }
            if (errno == ENOSPC || errno == EDQUOT) {
               printf("\tError: No Space\n");
                break;
            }
            return;
        }
        len -= ret;
        buf += ret;
        printf("\tError: Not full len wrote\n");
    }
   
    printf("Closing file...\n");
    if (close(fd) == -1) {
        perror("\tGot error");
        return;
    }
    
    printf("Getting stat...\n");
    struct stat st1 = {}; 
    if (stat(file_name, &st1) == -1) {
        perror("\tGot error");
        return;
    }
    printf("file size: %lld\n", (long long int)st1.st_size);
    
    printf("Truncating...\n");
    if (truncate(file_name, DATA_SIZE) == -1) {
        perror("\tGot error");
        return;
    }
    
    printf("Getting stat...\n");
    struct stat st2 = {}; 
    if (stat(file_name, &st2) == -1) {
        perror("\tGot error");
        return;
    }
    printf("file size: %lld\n", (long long int)st2.st_size);
   
    printf("End of file writing\n");
    printf("\n");
   
    return;
}

int main()
{
    printf("Opening create...\n");
    int fd = open("test.txt",
                   O_RDWR | O_CREAT | O_TRUNC | O_SYNC,
                   S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IROTH);
    if (fd == -1) {
        perror("\tGot error");
        return -1;
    }
    printf("Writing data to file...\n");
    ssize_t len = 5;
    char buf[5] = {'a', 'b', 'c', 'd', 'e'};
    int ret = write(fd, buf, len);
    if (ret == -1) {
        perror("\tGot error");
        return -1;
    }
    printf("Closing file...\n");
    if (close(fd) == -1) {
        perror("\tGot error");
        return -1;
    }
    printf("Getting stat...\n");
    struct stat st0 = {}; 
    if (stat("test.txt", &st0) == -1) {
        perror("\tGot error");
        return -1;
    }
    printf("file size: %lld\n", (long long int)st0.st_size);
    
    file_workflow2("test.txt");
   
    printf("Getting stat...\n");
    struct stat st1 = {}; 
    if (stat("test.txt", &st1) == -1) {
        perror("\tGot error");
        return -1;
    }
    printf("file size: %lld\n", (long long int)st1.st_size);

    return 0;
}
