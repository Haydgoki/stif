#include <stdio.h>
#include <stdlib.h>

int ft(char * buf){
    int *pt = (int *)buf;
    int i = &pt;
    return i;
}


int main(int argc, char *argv[])
{
    char buffer[20] = {0};
    printf("%d", ft(buffer));
    return 0;
}
