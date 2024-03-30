//  KN 11.3.2022



#include <mosquittopp.h>
#include <math.h>
#include <string>

class mqtt : public mosqpp::mosquittopp			// mqtt class
{

	public:
		mqtt(const char *host, int port);	 // constructor
		~mqtt(){};
		
				// call back functions
		void on_connect(int rc);
		void on_message(const struct mosquitto_message *message);
		void on_subscribe(int mid, int qos_count, const int *granted_qos);
		bool send_message(std::string message);

};

