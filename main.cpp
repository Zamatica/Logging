


#include "logging.h"


int main()
{
	logger::log.Init();

	logger::log << "It should work.";

	logger::printf("Hello: %d\n", 42);

	logger::log.Exit();
}


