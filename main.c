#include <stdio.h>

int main(void)
{
#ifdef _DEBUG
  printf("Hello, World IN debug\n");
#endif
#ifdef NDEBUG
  printf("Hello, World IN release\n");
#endif
  return 0;
}
