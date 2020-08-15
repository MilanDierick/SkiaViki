#include <iostream>
#include <SkPoint.h>

using namespace std;

int main() {
  cout << "Hello Skia!" << endl;
 
  SkPoint p {3,4};
  cout << "Length of {3,4} is " << p.length() << endl;
 
  return 0;
}