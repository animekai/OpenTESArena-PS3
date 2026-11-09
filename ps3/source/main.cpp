#include <cstdio>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int i = 0;
    while (true)
    {
        printf("etapa1 minimo vivo %d\n", i++);
        sleep(1);
    }
    return 0;
}
