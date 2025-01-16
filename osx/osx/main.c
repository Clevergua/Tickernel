#include "AppDelegate.h"

int main(int argc, const char * argv[]) {
    uint32_t count = 0;
    vkEnumerateInstanceLayerProperties(&count, NULL);
    printf("%d!!",count);
    
    return 0;
}
