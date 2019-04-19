/* this library is writing by  Oyeyiola Hamid.
*      oyeyiolahamid@yahoo.com
*  Designed to work with the GSM Sim800l,maybe work with SIM900L
*
*
*
*    Created on: March 25, 2019
*        Author: Oyeyiola Hamid
*
*
*/


#include <SoftwareSerial.h> //is necesary for the library!! 
#include"gsm_module.h"
#include"protocols.h"

#define RESET_PIN 2


SoftwareSerial ser(13, 15);
void printData(String& data);
GprsInfo gprs = { "web.gprs.mtnnigeria.net","web","web" };
HttpInfo http = { "www.facebook.com", HTTPMETHODS::GET, 0,0 };
SendSMS* sms;
WebClient* client;


void printData(String& data)
{
	Serial.println(data);
}

void setup() {
	pinMode(13, INPUT);
	pinMode(15, OUTPUT);
	Serial.begin(9600);
	GSMmodule::mod_init(RESET_PIN, &ser);
	sms = new SendSMS("08069073724", "Hello from Sim800L.");
	client = new WebClient(gprs, http, printData, HTTPTYPES::HTTPS);
	delay(3000);
}


void loop() {

	ProtocolSuite::Status status;

	while ((status = client->run()) == ProtocolSuite::Status::RUNNING)yield();
	if (status == ProtocolSuite::Status::FINISHED)
	{
		//Serial.println("message sent!");  
	}

	while ((status = client->close()) == ProtocolSuite::Status::CLOSING)yield();

	if (status == ProtocolSuite::Status::CLOSED)
	{
		Serial.println("CLOSED");
	}

}
