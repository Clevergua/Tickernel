

#include "tickernelEngine.h"

int main() {
    TickernelDynamicArray array ;
    tickernelCreateDynamicArray(&array, sizeof(int), 1024);
    
    return 0;
}