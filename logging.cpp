/*
FILE: Logging.cpp
PRIMARY AUTHOR: Sweet

Copyright � 2017 DigiPen (USA) Corporation.
*/
#include "logging.h"

namespace Logger
{
	std::string Logger::mWriteBuffer;
	std::string Logger::mReadBuffer;


	#ifdef LOGGING_USE_FILE
		std::ofstream Logger::mLogFile;

		std::string Logger::mWriteFileBuffer;
		std::string Logger::mReadFileBuffer;
	#endif



	bool Logger::mLogging = true;
	std::mutex Logger::mVault;



	int LoggerMainClass::Init()
	{
			static const std::thread LogMain = std::thread(

				// "Main" lambda function
				[]()
				{
					#ifdef LOGGING_USE_FILE
						mLogFile.open(PATH_TO_FILE, std::ofstream::out | std::ofstream::app);

						// Check if the file did not open
						if (!mLogFile.is_open())
						{
							// Tell you asap in debug mode
							assert(!"File did not open.");

							// Printing out an error message
							std::cout << FILE_ERROR_CONSOLE_MSG;
						}
					#endif
					
					// Check if we are still running
					while (mLogging)
					{
						// Check if we read to write more to the screen
						if (mWriteBuffer.size())
						{
							// Lock the Data so no one  can write, or wait for when you can have it
							mVault.lock();

							// Copy over the text into another buffer, so other threads avoid IO wait time from std::cout
							mReadBuffer = mWriteBuffer;

							// Clear the write buffer so it doesn't show up again
							mWriteBuffer.clear();

							// Unlock so other threads can write into the logging.
							mVault.unlock();

							// IO Time! Print out the text to the screen.
							std::cout << mReadBuffer;
	#ifdef LOGGING_USE_FILE
							// Write to a file
							mLogFile << mReadBuffer;
	#endif

							// Clear what was printed out to the screen, so it doesn't show up again
							//    Similar to not clearing a frame buffer on a screen
							mReadBuffer.clear();
						}
					std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME));
					}

			});
	}


	int printf(const char *fmt, ...)
	{
		char buffer[2048];
		va_list args;
		va_start(args, fmt);
	
		int ret = std::vsnprintf(buffer, sizeof(buffer), fmt, args);
		
		va_end(args);

		LoggerMainClass::Log_Internal(buffer);

		return ret;
	}


	void LoggerMainClass::Log_Internal(const char *message)
	{
		// Save the message into the public write Buffer
		mWriteBuffer += message;
	}
}

