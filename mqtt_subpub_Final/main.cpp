

//////////////////////////////////////  11.3.2022 //////////////////////////////
///
///	g++ main.cpp mqtt_receiv.cpp -std=c++0x -pthread -lmosquitto -lmosquittopp -ljsoncpp
///
///////////////////////////////////////////////////////////////////////////////

#include <signal.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <unistd.h>

using namespace std;

#include "mqtt_receiv.h"

///////////////////////////////////////// conficuration ///////////////////////

#include "mqtt.h"

//////////////////////////////////// main /////////////////

int main(int argc, char *argv[])
{
	printf("Raspberry Pi MQTT receive demo\n");

	printf("\n       Subscribed to listen MQTT messages \n");

	mqtt mqtt_obj(host_addr, port_number); // object mqtt_obj

	while (1) // keep mqtt send theread running

	{
		string message;
		cout << endl;
		cout << "\033[13;23H\033[0;44m                      ";
		cout << "\033[13;21H\033[0;44m> ";
		cin >> message;
		mqtt_obj.send_message(message);
		cout << "\033[A";
		cout << "\033[0m";
	}

	// while (1) // keep mqtt send theread running

	// {
	// 	string message;
	// 	cout << "\033[13;21H\033[0;44m>\033[0m";
	// 	cin >> message;
	// 	cout << "\033[A";
	// 	mqtt_obj.send_message(message);
	// 	usleep(1000000);
	// }

	return 0;
}
