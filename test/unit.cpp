#include "mathfunctions.h"
#include <assert.h>

int main (int argc, char *argv[]) {

  assert(rangemap(10,0,10,500,100) == 100.0f);

  assert(rangemap(10,0,10,100,500) == 500.0f);
  
  assert(rangemap(10,0,10,500,100) >= 100.0f);

  assert(rangemap(5,0,10,0,500) == 250.0f);
  
  return 0;
}
