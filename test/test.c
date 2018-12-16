#include <stdio.h>
#include <stdlib.h>

int ft(char * buf){
    /*int *pt = (int *)buf;
    int i = *pt;*/
    return ((int32_t *)buf)[0];
}

int32_t _btoi(char* b){int32_t res = b[0]<<24 | b[1]<<16 | b[2]<<8 | b[3]; return res;}

int main(int argc, char *argv[])
{
    char buffer[20] = {1,0,0,0,2,0,0,0};

    printf("%d\n", ft(buffer + 4));
    printf("%d\n", _btoi(buffer + 4));

    if (buffer[0] == 1)
      printf("OK\n");

    int i;
    i = buffer[0] ? 1 : 0;
    printf("%d\n", i);

    int *pt_i;
    pt_i = &i;
    printf("%d\n", (*pt_i));


    u_int8_t j= 12;
    int8_t k=13;
    printf("%d\n", j<k);

    return 0;
}
