#include <stdio.h>
#include <unistd.h>

void wait_for_gdb_to_attach() {
  while (1) {
    sleep(1); // sleep for 1 second
  }
}

int main(void) {
  wait_for_gdb_to_attach();
  printf("Content-Type: text/plain;charset=us-ascii\n\n");
  printf("Hello!");
  return 0;
}
