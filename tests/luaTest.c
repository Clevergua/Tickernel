#include <tickernelLuaBinding.h>

void aaa(int *a)
{
    a = malloc(sizeof(int));
}
int main()
{
    int *a = NULL;
    aaa(a);
    assert(a != NULL); // This assertion will fail because 'a' is still NULL
   
    return 0;
}