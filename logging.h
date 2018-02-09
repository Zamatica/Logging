/*
FILE: Logging.h
PRIMARY AUTHOR: Sweet

Copyright � 2017 DigiPen (USA) Corporation.
*/
#pragma once




// Time the logging thread should wait for
#define WAIT_TIME 8

// Change this to ANYTHING else to disable file logging
#define LOGGING_USE_FILE

// Files are optional!
//   This is because files is slower than just the console
#ifdef LOGGING_USE_FILE

	// The file you would like to log to
	//     If you want to do multiple you have to make changes
	#define PATH_TO_FILE "log.txt"

	// Message to display to the console if the file failed to open.
	//     Will assert in debug mode! Remove it if you don't like it
	#define FILE_ERROR_CONSOLE_MSG "File did not open! Turning off file logging.\n"

#endif



// assert
#include <cassert>

// va_start, va_end, va_list
#include <cstdarg>

// std::cout 
#include <iostream>

// std::ofstream
#include <fstream>

// std::stringstream
#include <sstream>

// std::string
#include <string>

// std::put_time
#include <iomanip>

// std::thread
#include <thread>

// std::mutex
#include <mutex>


namespace Logger
{
	class LoggerMainClass
	{
		private:
			// The running state of this thread
			//     false means exit
			static bool mLogging;

			// Mutex to protect the data when a thread needs to read/write
			//     Prevents other threads from reading/writing
			static std::mutex mVault;

		private:
			// "Public" write buffer
			//    We call this public since other threads write to this buffer
			static std::string mWriteBuffer;

			// Buffer of Text about to printed out
			static std::string mReadBuffer;


		#ifdef LOGGING_USE_FILE
			static std::ofstream mLogFile;

			static std::string mWriteFileBuffer;
			static std::string mReadFileBuffer;
		#endif

		private:
			// printf!
			friend int printf(const char *fmt, ...);

		private:
			// Internal Log that adds the message to the buffers
			static void Log_Internal(const char *message);

			// Class that handles the output operator
			//     This is used so the timestamp isn't printed multiple times
			class LoggerAppend
			{
				public:
					template <typename T>
					LoggerReturn& operator <<(T& rhs)
					{
						// Use stringstream here as a "file"
						//     It is a string that acts like a file
						std::stringstream ss;

						// Write the rhs into the string
						ss << rhs;

						// Pass the created string to the Log
						Logger::Log_Internal(ss.str().c_str());

						return *this;
					}
			};


		public:
			template <typename T>
			LoggerAppend operator <<(T& rhs)
			{
				// Use stringstream here as a "file"
				//     It is a string that acts like a file
				std::stringstream ss;

				// Get the current time
				std::time_t t = std::time(nullptr);

				// Write the timestamp and the first part of the message
				ss << std::put_time(std::localtime(&t), "[%d/%m/%Y][%H:%M]: ") << rhs;
				
				// Save the message in the buffer
				Logger::Log_Internal(ss.str().c_str());
				
				// Return a internal class so that the timestamp doesn't get printed
				return LoggerAppend();
			}


		public:
			// Sets up the other thread and starts the logging process
			static int Init();

	};
	
	int printf(const char *fmt, ...);

	// Global value that represents the main interface of logging
	//     This is the "std::cout"
	LoggerMainClass log;

	// To Log:
	//    swt::log << "Hello World: " << MyClassWithOverloads << "\n";
	//    swt::printf("%s", "Hello World\n");
}





