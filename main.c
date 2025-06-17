#include <stdio.h>
#include "elf/elf.h"

int main(void)
{
  ELF_load("/tmp/example.elf");
  return 0;
}
