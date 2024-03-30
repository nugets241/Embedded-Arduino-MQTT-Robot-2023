
///////////////////////////  22.3.2022  KN /////////////////////////////

#include <cstdio>
#include <cstring>
#include <string>
#include <functional> // c++11
#include <unistd.h>
#include <iostream> // due to: cout

#include <fstream>
#include <time.h>

#include "mqtt.h"

#include "mqtt_receiv.h"

#include <jsoncpp/json/json.h>

using namespace std;

// Global variables for console position

int distR, distC;
int direR, direC;
int consR, consC;

// Funtions for changing color and position

void setText(int c, string t)
{
	cout << "\033[1;" << c << "m" << t << "\033[0m" << endl;
}

void setCursor(int x, int y)
{
	cout << "\033[" << y << ";" << x << "H";
}

void cmdCursor()
{
	cout << "\033[13;23H\033[0;44m                      ";
	cout << "\033[13;21H\033[0;44m> ";
	setCursor(23, 13);
}

//////////////////////////////////////////////////////////////////////////
//////////
//////////    MQTT
//////////
/////////////////////////////////////////////////////////////////////////

mqtt::mqtt(const char *host, int port)
{

	int keepalive = 600;

	printf("****   MQTT start connect ****** \n");

	connect(host, port, (int)keepalive); // connect to mqtt broker

	loop_start(); // stay on mqtt loop
};

/////////// send MQTT message /////////////////////////////////////////////

bool mqtt::send_message(std::string message)

{
	// Send message - depending on QoS, mosquitto lib managed re-submission this the thread
	//
	// * NULL : Message Id (int *) this allow to latter get status of each message
	// * topic : topic to be used
	// * lenght of the message
	// * message
	// * qos (0,1,2)
	// * retain (boolean) - indicates if message is retained on broker or not
	// Should return MOSQ_ERR_SUCCESS

	if (message.find(':') != string::npos)
	{
		size_t pos = message.find(":");
		string key = message.substr(0, pos);
		int value = std::stoi(message.substr(pos + 1));

		message = "{\"" + key + "\":" + to_string(value) + '}';
		setCursor(consC + 2, consR + 1);
		setText(44, "                      ");
		setCursor(consC + 2, consR + 1);
		setText(44, message);
		setCursor(consC, consR);
		cout << "\033[0;44m> ";
	}

	int ret = publish(NULL, out_topic, message.length(), message.c_str(), 1, false);

	// printf(" Message published \n");

	return (ret == MOSQ_ERR_SUCCESS);
}

////////// ON CONNECT ///////////////////////////////////////////////////
void createUI();

void mqtt::on_connect(int rc)
{
	cout << "\033[0m";
	createUI();

	printf("****   MQTT Connected with code= %d  *****\n", rc);
	if (rc == 0)
	{

		// Only attempt to subscribe on a successful connect.

		subscribe(NULL, in_topic);

		printf("****   MQTT subscription to topic = ");
		printf(in_topic);
		printf("    ****\n");
	}
}

//////////////////////////  ON SUBSCRIBE ///////////////////////////////////////

void mqtt::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
	printf("****   MQTT Topic subscription succeeded.   ****\n");
	printf("PRESS ENTER!\n");
	setCursor(consC, consR);
	cout << "\033[0;44m> ";
}

//////////////////////////// Message received //////////////////////////////////

void mqtt::on_message(const struct mosquitto_message *message) // on message callback

{
	// printf("\n ===================  Message received  ================================");

	std::string mqtt_message;

	mqtt_message = (char *)message->payload;

	Json::Reader reader;

	Json::Value obj;

	int number;

	string str;

	if (reader.parse(mqtt_message, obj))
	{
		if (obj["Lid"].isInt())
		{
			number = obj["Lid"].asInt();

			setCursor(distC, distR + 1);
			setText(44, "                     ");
			setCursor(distC, distR + 1);
			setText(44, to_string(number) + " cm");
		}
		else if (obj["Dist"].isInt())
		{
			number = obj["Dist"].asInt();

			setCursor(distC, distR + 3);
			setText(44, "                     ");
			setCursor(distC, distR + 3);
			setText(44, to_string(number) + " cm");
		}
		else if (obj["Com"].isInt())
		{
			number = obj["Com"].asInt();

			setCursor(direC, direR + 1);
			setText(44, "                     ");
			setCursor(direC, direR + 1);
			setText(44, to_string(number) + "°");
		}
		else if (obj["Dire"].isInt())
		{
			number = obj["Dire"].asInt();

			setCursor(direC, direR + 3);
			setText(44, "                     ");
			setCursor(direC, direR + 3);
			setText(44, to_string(number) + "°");
		}
		else if (obj["State"].isString())
		{
			str = obj["State"].asString();

			setCursor(consC + 2, consR + 3);
			setText(44, "                      ");
			setCursor(consC + 2, consR + 3);
			setText(44, str);
		}
	}
	else
	{
		setCursor(consC, consR + 11);
		printf("\n --- Error in JSON parsing ---- \n");
	}

	setCursor(consC, consR);
	cout << "\033[0;44m> ";

	// printf("\n  Topic = %s", message->topic); // print message topic

	// printf("  Message = %s\n", mqtt_message.c_str());

	// printf("\n");
} ////////////////////////////         end message received ////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///    UI CODE
///
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void createUI()
{
	cout << "\033[2J"; // clearing the view
	int c1 = 3, r1 = 2;
	int c2 = 35, r2 = 2;
	int c3 = 18, r3 = r1 + 9;

	distR = r1 + 2;
	distC = c1 + 4;

	direR = r2 + 2;
	direC = c2 + 4;

	consR = r3 + 2;
	consC = c3 + 3;

	// UI for Distance
	setCursor(c1, r1++);
	setText(46, "  Distance                 ");
	setCursor(c1, r1++);
	setText(43, "                           ");
	setCursor(c1, r1++);
	setText(43, "  \033[0;44m                       \033[0;43m  ");
	setCursor(c1, r1++);
	setText(43, "  \033[0;44m                       \033[0;43m  ");
	setCursor(c1, r1++);
	setText(43, "  \033[0;44m                       \033[0;43m  ");
	setCursor(c1, r1++);
	setText(43, "  \033[0;44m                       \033[0;43m  ");
	setCursor(c1, r1);
	setText(43, "                           ");

	// Distance value
	setCursor(distC, distR);
	setText(44, "Current dist: ");
	setCursor(distC, distR + 2);
	setText(44, "Target dist: ");

	// UI for Compass
	setCursor(c2, r2++);
	setText(46, "  Compass                  ");
	setCursor(c2, r2++);
	setText(43, "                           ");
	setCursor(c2, r2++);
	setText(43, "  \033[0;44m                       \033[0;43m  ");
	setCursor(c2, r2++);
	setText(43, "  \033[0;44m                       \033[0;43m  ");
	setCursor(c2, r2++);
	setText(43, "  \033[0;44m                       \033[0;43m  ");
	setCursor(c2, r2++);
	setText(43, "  \033[0;44m                       \033[0;43m  ");
	setCursor(c2, r2);
	setText(43, "                           ");

	// Distance value
	setCursor(direC, direR);
	setText(44, "Current dire: ");
	setCursor(direC, direR + 2);
	setText(44, "Target dire: ");

	setCursor(c3, r3++);
	setText(46, "  Car drive command          ");
	setCursor(c3, r3++);
	setText(43, "                             ");
	setCursor(c3, r3++);
	setText(43, "  \033[0;44m                         \033[0;43m  ");
	setCursor(c3, r3++);
	setText(43, "  \033[0;44m                         \033[0;43m  ");
	setCursor(c3, r3++);
	setText(43, "  \033[0;44m                         \033[0;43m  ");
	setCursor(c3, r3++);
	setText(43, "  \033[0;44m                         \033[0;43m  ");
	setCursor(c3, r3);
	setText(43, "                             ");

	setCursor(consC, consR);
	setText(44, "> ");
	setCursor(consC + 2, consR + 2);
	setText(44, "Current state: ");

	setCursor(consC, consR + 7);
}
