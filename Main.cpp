#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include "mySemaphore.h"

//#include <semaphore>

#define CONSUMERS_COUNTS 12
#define PRODUCER_SLEEP_TIME 200ms
#define CONSUMERS_SLEEP_TIME 2000ms


//OUTPUT HELPER 
// # The symbol means that this output from the producer
// * The symbol means that this output from the consumer

namespace SharedMembers
{
	std::queue<int> buffer; 
	//std::mutex mx;
	bool file_EOF_status(false);
	semaphore MySem;
}

void producer(std::string file_path)
{
	//std::cout << "Producer Thread ID = " << std::this_thread::get_id() << std::endl;
	std::string num;
	std::fstream File(file_path);

	using namespace std::literals;

	if (File.is_open())
	{
		while (std::getline(File,num))
		{
			std::this_thread::sleep_for(PRODUCER_SLEEP_TIME); // producer's run time for pushing one number to the buffer 
			std::cout << "######## -> " << "Producer pushed number -> " << std::stoi(num) << " to buffer." << std::endl;
			SharedMembers::buffer.push(std::stoi(num));
			SharedMembers::MySem.release();
		}
		if (File.eof())
		{
			std::cout << "######## -> " << "The producer wrote all numbers from the file to the buffer." << std::endl;
			SharedMembers::file_EOF_status = true;
		}
		File.close();
	}
	else
	{
		throw std::string("Unable to open file");
	}
}


void consumer()
{
	//std::cout << "Consumer Thread ID = " << std::this_thread::get_id() << std::endl;

	using namespace std::literals;

	while (!(SharedMembers::file_EOF_status && SharedMembers::buffer.empty()))
	{
		
		//SharedMembers::mx.lock();

		if (!SharedMembers::buffer.empty())
		{
			SharedMembers::MySem.acquire();
			if (CONSUMERS_COUNTS - SharedMembers::buffer.size() >= 0 && CONSUMERS_COUNTS - SharedMembers::buffer.size() <= CONSUMERS_COUNTS)
			{
				std::atomic<std::ostream> A();
				std::cout << "******** -> " << CONSUMERS_COUNTS - SharedMembers::buffer.size() << "  Consumers waiting for work" << std::endl;
			}
			
			std::this_thread::sleep_for(CONSUMERS_SLEEP_TIME);
			std::cout << "******** -> " << "Consumer by THREAD ID -> " << std::this_thread::get_id() << "  works and prints number -> " << SharedMembers::buffer.front()*2 << std::endl;
			SharedMembers::buffer.pop();
		}
		else
		{
			std::this_thread::sleep_for(CONSUMERS_SLEEP_TIME);
			//std::cout << "Consumer by THREAD ID -> " << std::this_thread::get_id() << " is waiting because the buffer is empty" << std::endl;
		}

		//SharedMembers::mx.unlock();
	}

	std::cout << "EOF & buffer is empty | works done" << std::endl;
	exit(0);
}


int main()
{
	std::cout << "Consumers: " << CONSUMERS_COUNTS << std::endl;
	std::cout << "Producer: 1" << std::endl;

	try
	{
		std::thread producer_thread(&producer, "numbers.txt");
		std::thread consumer_threads[CONSUMERS_COUNTS];
		
		for (int i = 0; i < CONSUMERS_COUNTS; ++i)
		{
			consumer_threads[i] = std::thread(&consumer);
		}
	
		
		producer_thread.join();

		for (int i = 0; i < CONSUMERS_COUNTS; ++i)
		{
			if(consumer_threads[i].joinable())
				consumer_threads[i].join();
		}
	}
	catch (std::string&  ex)
	{
		std::cout << ex;
	}
}