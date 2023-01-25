#include <cstdio>
#include <random>

int main()
{
    srand(time(nullptr));
    printf("Hello, World! %d", rand() % 10 + 1);
    return 0;
}