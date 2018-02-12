


#include "logging.h"


int main()
{
	logger::log.Init();

	logger::log << "Test\n";

	logger::log.Exit();
}


