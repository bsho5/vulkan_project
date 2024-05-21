#include <exception>
#include <iostream>
#include <cstdlib>
#include "app.hpp"

void call();

int main(){
call();
  lve::App app{};

  try {
  app.run();
  } catch (const std:: exception &e) {
  std::cerr <<e.what()<< "\n";
  return EXIT_FAILURE;
  }
  return  EXIT_SUCCESS;
}

void call(){}