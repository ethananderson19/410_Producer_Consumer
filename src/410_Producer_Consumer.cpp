//============================================================================
// Name        : 410_Producer_consumer
// Author      : 
// Version     :
// Copyright   : Steal this code!
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <iostream>

using namespace std;
mutex m;				//mutual exclusion
condition_variable cv; 	//for signaling
bool bDone = false;			//used by producer to indicate we are done
int gCount = 0;
void producer(int numbcounts) {
	for (int j = 0; j < numbcounts; j++) {
		{
			//notice its unique
			unique_lock<mutex> lck(m);
			//produce something
			gCount++;
			cout << "Produced one, gCount=" << gCount << endl;
		}
		//tell consumer to consume (one way comms from producer to consumer)
		cv.notify_all();			
	}
	//one last lock to tell everyone we are done
	{
		unique_lock<mutex> lck(m);
		bDone = true;
		cout << "Producer DONE!!" << endl;
	}
	
	//this will wake the consumer 
	cv.notify_all();
}
void consumer(int id) {
	//run until we break out
	while (true){
		unique_lock<mutex> lck(m);
		//has producer produced or is finished?
		while (gCount==0 && !bDone)
		{
			cout << "   Consumer: " << id<<" waiting......." << endl;
			cv.wait(lck);
		}
		//any work to do?
		if (gCount>0){
			//do it
			gCount--;
			cout << "   Consumer: " << id<<" consumed one, gCount=" << gCount << endl;			
		}	
		//finished? Only when the producer is done and gCount == 0
		if (gCount==0 && bDone){
			break;	
		}
	}
	//make sure the cout prints properly
	unique_lock<mutex> lck(m);
	cout << "  Consumer: " << id << " exiting" << endl;
}
int main() {
	cout << "The initial value of gCount is " << gCount << endl; //
	thread t_producer(producer, 1000);
	thread t_consumer1(consumer, 1);
	thread t_consumer2(consumer, 2);
	t_producer.join();
	t_consumer1.join();
	t_consumer2.join();
	cout << endl << "The final value of gCount is " << gCount << endl; //
	return 0;
}
