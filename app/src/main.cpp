#include "close2gl.h"
#include "opengl_reference.h"

#include <iostream>
#include <thread>

int main(int argc, char **argv) {
  Controls controls;

  std::thread t1(opengl, &controls);

  t1.join();
}
