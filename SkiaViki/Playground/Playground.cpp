#include "pch.h"
#include <iostream>
#include <include/core/SkPoint.h>

int main()
{
	std::cout << "Hello Skia!" << std::endl;

	SkPoint p{ 3,4 };
	std::cout << "Length of {3,4} is " << p.length() << std::endl;

	return 0;
}