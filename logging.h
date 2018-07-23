/*
FILE: Logging.h
PRIMARY AUTHOR: Sweet
*/
#pragma once




// Time the logging thread should wait for
#define WAIT_TIME 8

// How the timestamp should be printed out
#define TIMESTAMP_STYLE "[%d/%m/%Y][%H:%M:%S]: "

// Change this from LOGGING_USE_FILE else to disable file logging
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

// Can assert when bad things happen or throw
//     Change this  from USE_ASSERTS to throw
#define USE_ASSERT

// The code side name for the logging thread
//     this allows for logger::log << statement;
#define LOGGER_VARIABLE_NAME log



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


namespace logger
{
	class LoggerMainClass
	{
		private:
			// The thread the logger will run on
			std::thread mLogMain;

			// Mutex to protect the data when a thread needs to read/write
			//     Prevents other threads from reading/writing
			std::mutex mVault;

			// The running state of this thread
			//     false means exit
			bool mLogging = true;

			// Will close the thread on the next pass
			//     this will log anything in the buffers before closing
			bool mClose = false;

		private:
			// "Public" write buffer
			//    We call this public since other threads write to this buffer
			std::string mWriteBuffer;

			// Buffer of Text about to printed out
			std::string mReadBuffer;


		#ifdef LOGGING_USE_FILE
			std::ofstream mLogFile;

			// Here for future use to allow people to differ console and file outputs
			//std::string mWriteFileBuffer;
			//std::string mReadFileBuffer;
		#endif

		private:
			// logger::printf!
			friend int printf(const char *fmt, ...);

		private:
			// Internal Log that adds the message to the buffers
			void Log_Internal(const char *message);

			// Class that handles the output operator
			//     This is used so the timestamp isn't printed multiple times
			class LoggerAppend
			{
				LoggerMainClass *mainLog = nullptr;

				public:
					LoggerAppend(LoggerMainClass *logger) : mainLog(logger) {}
					
					template <typename T>
					LoggerAppend& operator <<(T& rhs)
					{
						// Use stringstream here as a "file"
						//     It is a string that acts like a file
						std::stringstream ss;

						// Write the rhs into the string
						ss << rhs;

						// Pass the created string to the Log
						mainLog->Log_Internal(ss.str().c_str());

						return *this;
					}

                    template <typename T>
					LoggerAppend& operator <<(const T& rhs)
					{
						// Use stringstream here as a "file"
						//     It is a string that acts like a file
						std::stringstream ss;

						// Write the rhs into the string
						ss << rhs;

						// Pass the created string to the Log
						mainLog->Log_Internal(ss.str().c_str());

						return *this;
					}
			};

		private:
			class OpenFileFailure
			{
				public:
					void what()
					{
						std::cout << "Failed to open log file.\n";
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
				ss << std::put_time(std::localtime(&t), TIMESTAMP_STYLE) << rhs;
				
				// Save the message in the buffer
				Log_Internal(ss.str().c_str());
				
				// Return a internal class so that the timestamp doesn't get printed
				return LoggerAppend(this);
			}

            template <typename T>
			LoggerAppend operator <<(const T& rhs)
			{
				// Use stringstream here as a "file"
				//     It is a string that acts like a file
				std::stringstream ss;

				// Get the current time
				std::time_t t = std::time(nullptr);

				// Write the timestamp and the first part of the message
				ss << std::put_time(std::localtime(&t), TIMESTAMP_STYLE) << rhs;
				
				// Save the message in the buffer
				Log_Internal(ss.str().c_str());
				
				// Return a internal class so that the timestamp doesn't get printed
				return LoggerAppend(this);
			}


		public:
			// Sets up the other thread and starts the logging process
			int Init();

			// Tells the logger to shutdown, will log anything currently in buffers
			void Exit();

	};
	
	int printf(const char *fmt, ...);

	// Global value that represents the main interface of logging
	//     This is the "std::cout"
	extern LoggerMainClass LOGGER_VARIABLE_NAME;

	// To Log:
	//    swt::log << "Hello World: " << MyClassWithOverloads << "\n";
	//    swt::printf("%s", "Hello World\n");
};





