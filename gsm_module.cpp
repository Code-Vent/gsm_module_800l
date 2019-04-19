#include"gsm_module.h"
#include<assert.h>
//#include<ESP8266mDNS.h>

//#ifndef F
//	#define F
//	#define PROGMEM
//#endif

#define COMMAND_SEND_DELAY 500

SoftwareSerial* GSMmodule::serial_ = 0;
uint8_t GSMmodule::mreset_pin;
String GSMmodule::mbuffer;

enum Reply { OK_, ERROR_, SHUT_OK_, POWER_DOWN, DOWNLOAD };
const char* strings_[] = { "OK","ERROR", "SHUT OK", "NORMAL POWER DOWN", "DOWNLOAD",
"AT+CMGF?\r\n", "AT+CPIN?\r\n", "AT+CSTT?\r\n", "AT+CPAS", "+CMGF:", "+CPIN:", "+CSTT:", "+CPAS:" };


void GSMmodule::sms_setToTextMode(FeedbackInterface & fi)
{

	if (!fi.blocked)
	{
		GSMmodule::mod_read();
		delay(COMMAND_SEND_DELAY);
#ifdef USE_SOftwareSerial 
		assert(serial_);
		serial_->println(F("AT+CMGF=1\r\n"));
#else
		Serial.println(F("AT+CMGF=1\r\n"));
#endif
		fi.blocked = true;
		fi.timerStart = millis();
		return;
	}
	else if ((millis() - fi.timerStart) > 5000)
	{
		fi.blocked = false;
	}
	else
	{
		fi.blocked = !ResponseListener(strings_[Reply::OK_], fi);
	}
}

void GSMmodule::sms_sendSmsNumber(FeedbackInterface & fi, const char * number)
{

	if (!fi.blocked)
	{
		GSMmodule::mod_read();
		delay(COMMAND_SEND_DELAY);
#ifdef USE_SOftwareSerial
		assert(serial_);
		serial_->print(F("AT+CMGS=\""));
		serial_->print(number);
		serial_->println(F("\"\r\n"));
#else
		Serial.print(F("AT+CMGS=\""));
		Serial.print(number);
		Serial.println(F("\"\r\n"));
#endif
		fi.blocked = true;
		fi.timerStart = millis();
		return;
	}
	else if ((millis() - fi.timerStart) > 5000)
	{
		fi.blocked = false;
	}
	else
	{
		fi.blocked = !ResponseListener(">", fi);
	}

}

void GSMmodule::sms_sendSmsText(FeedbackInterface & fi, const char * text)
{
#ifdef USE_SOftwareSerial
	assert(serial_);
	serial_->print(text);
	serial_->print("\r\n");
#else
	Serial.print(text);
	Serial.print("\r\n");
#endif
	delay(2 * COMMAND_SEND_DELAY);
	PromptListener(fi);
}

void GSMmodule::sms_sendEndMark()
{
#ifdef USE_SOftwareSerial
	assert(serial_);
	serial_->print((char)26);
#else
	Serial.print((char)26);
#endif
}

void GSMmodule::sms_readSms(FeedbackInterface & fi, uint8_t index)
{

	if (!fi.blocked)
	{
		GSMmodule::mod_read();
		delay(COMMAND_SEND_DELAY);
#ifdef USE_SOftwareSerial
		assert(serial_);
		serial_->print(F("AT+CMGR="));
		serial_->print(index);
		serial_->println(F("\r\n"));
#else
		Serial.print(F("AT+CMGR="));
		Serial.print(index);
		Serial.println(F("\r\n"));
#endif
		fi.blocked = true;
		fi.timerStart = millis();
		return;
	}
	else if ((millis() - fi.timerStart) > 5000)
	{
		fi.blocked = false;
	}
	else
	{
		fi.blocked = !ResponseListener("AT+CMGR", fi);
	}
}

void GSMmodule::sms_delAllSms(FeedbackInterface &)
{
}

void GSMmodule::call_callNumber(FeedbackInterface & fi, const char * number)
{
	if (!fi.blocked)
	{
		GSMmodule::mod_read();
		delay(COMMAND_SEND_DELAY);
#ifdef USE_SOftwareSerial
		assert(serial_);
		serial_->print(F("ATD"));
		serial_->print(number);
		serial_->print(F(";\r\n"));
#else
		Serial.print(F("ATD"));
		Serial.print(number);
		Serial.print(F(";\r\n"));
#endif
		fi.blocked = true;
		fi.timerStart = millis();
		return;
	}
	else if ((millis() - fi.timerStart) > 5000)
	{
		fi.blocked = false;
	}
	else
	{
		fi.blocked = !ResponseListener(strings_[Reply::OK_], fi);
	}
}

void GSMmodule::call_answerCall(FeedbackInterface &)
{
}

void GSMmodule::call_hangoffCall(FeedbackInterface &fi)
{
	if (!fi.blocked)
	{
		GSMmodule::mod_read();
		delay(COMMAND_SEND_DELAY);
#ifdef USE_SOftwareSerial
		assert(serial_);
		serial_->print(F("ATH\r\n"));
#else
		Serial.print(F("ATH\r\n"));
#endif
		fi.blocked = true;
		fi.timerStart = millis();
		return;
	}
	else if ((millis() - fi.timerStart) > 20000)
	{
		fi.blocked = false;
	}
	else
	{
		fi.blocked = !ResponseListener(strings_[Reply::OK_], fi);
	}
}

void GSMmodule::gprs_setAccessPointParams(FeedbackInterface& fi, const String& apn,
	const String& userName, const String& password)
{
	if (!fi.blocked)
	{
		GSMmodule::mod_read();
		delay(COMMAND_SEND_DELAY);
#ifdef USE_SOftwareSerial 
		assert(serial_);
		serial_->print(F("AT+CSTT=\""));
		serial_->print(apn);
		serial_->print(F("\",\""));
		serial_->print(userName);
		serial_->print(F("\",\""));
		serial_->print(password);
		serial_->print(F("\"\r\n"));
#else
		Serial.print(F("AT+CSTT=\""));
		Serial.print(apn);
		Serial.print(F("\",\""));
		Serial.print(userName);
		Serial.print(F("\",\""));
		Serial.print(password);
		Serial.print(F("\"\r\n"));
#endif
		fi.blocked = true;
		fi.timerStart = millis();
		return;
	}
	else if ((millis() - fi.timerStart) > 6000)
	{
		fi.blocked = false;
	}
	else
	{
		fi.blocked = !ResponseListener(strings_[Reply::OK_], fi);
	}
}

void GSMmodule::gprs_shutDown(FeedbackInterface & fi)
{
	if (!fi.blocked)
	{
		GSMmodule::mod_read();
		delay(COMMAND_SEND_DELAY);
#ifdef USE_SOftwareSerial
		assert(serial_);
		serial_->print(F("AT+CIPSHUT\r\n"));
#else
		Serial.print(F("AT+CIPSHUT\r\n"));
#endif
		fi.blocked = true;
		fi.timerStart = millis();
		return;
	}
	else if ((millis() - fi.timerStart) > 10000)
	{
		fi.blocked = false;
	}
	else
	{
		fi.blocked = !ResponseListener(strings_[Reply::SHUT_OK_], fi);
	}
}

void GSMmodule::gprs_startWirelessConnection(FeedbackInterface &fi)
{

	if (!fi.blocked)
	{
		GSMmodule::mod_read();
		delay(5 * COMMAND_SEND_DELAY);
#ifdef USE_SOftwareSerial
		assert(serial_);
		serial_->print(F("AT+CIICR\r\n"));
#else
		Serial.print(F("AT+CIICR\r\n"));
#endif
		fi.blocked = true;
		fi.timerStart = millis();
		return;
	}
	else if ((millis() - fi.timerStart) > 60000)
	{
		fi.blocked = false;
	}
	else
	{
		fi.blocked = !ResponseListener(strings_[Reply::OK_], fi);
	}
}

void GSMmodule::gprs_getIPAddress(FeedbackInterface & fi)
{

	if (!fi.blocked)
	{
		GSMmodule::mod_read();
		delay(COMMAND_SEND_DELAY);
#ifdef USE_SOftwareSerial
		assert(serial_);
		serial_->print(F("AT+CIFSR\r\n"));
#else
		Serial.print(F("AT+CIFSR\r\n"));
#endif
		fi.blocked = true;
		fi.timerStart = millis();
		return;
	}
	else if ((millis() - fi.timerStart) > 6000)
	{
		fi.blocked = false;
	}
	else
	{
		fi.blocked = !ResponseListener(".", fi);
	}
}

void GSMmodule::gprs_startTCP(FeedbackInterface &, const char * website, int port)
{
}

void GSMmodule::gprs_bearerProfileSetting(FeedbackInterface& fi, int cmdType, int cid,
	const char* conParamTag, const char* conParamValue)
{

	if (!fi.blocked)
	{
		GSMmodule::mod_read();
		delay(COMMAND_SEND_DELAY);
#ifdef USE_SOftwareSerial 
		assert(serial_);
		serial_->print(F("AT+SAPBR="));
		serial_->print(cmdType);
		serial_->print(F(","));
		serial_->print(cid);

		if (conParamTag && conParamValue)
		{
			serial_->print(F(",\""));
			serial_->print(conParamTag);
			serial_->print(F("\",\""));
			serial_->print(conParamValue);
			serial_->print(F("\""));
		}

		serial_->print(F("\r\n"));
#else
		Serial.print(F("AT+SAPBR="));
		Serial.print(cmdType);
		Serial.print(F(","));
		Serial.print(cid);

		if (conParamTag && conParamValue)
		{
			Serial.print(F(",\""));
			Serial.print(conParamTag);
			Serial.print(F("\",\""));
			Serial.print(conParamValue);
			Serial.print(F("\""));
		}

		Serial.print(F("\r\n"));
#endif
		fi.blocked = true;
		fi.timerStart = millis();
		return;
	}
	else if ((millis() - fi.timerStart) > 60000)
	{
		fi.blocked = false;
	}
	else
	{
		fi.blocked = !ResponseListener(strings_[Reply::OK_], fi);
	}

}

void GSMmodule::http_readData(FeedbackInterface & fi, String& data)
{
	if (!fi.blocked)
	{
		GSMmodule::mod_read();
		delay(COMMAND_SEND_DELAY);
#ifdef USE_SOftwareSerial
		assert(serial_);
		serial_->print(F("AT+HTTPREAD\r\n"));
#else
		Serial.print(F("AT+HTTPREAD\r\n"));
#endif
		fi.blocked = true;
		fi.timerStart = millis();
		return;
	}
	else if ((millis() - fi.timerStart) > 120000)
	{
		fi.blocked = false;
	}
	else
	{
		fi.blocked = !DataRead_helper(fi, data, strings_[Reply::OK_]);
		//fi.blocked = !ResponseListener(strings_[Reply::OK_], fi);
		//data += mbuffer;
	}
}

void GSMmodule::http_inputData(FeedbackInterface & fi, int dataLen, int data)
{
	if (!fi.blocked)
	{

#ifdef USE_SOftwareSerial
		assert(serial_);
		if (dataLen > 0 && data != -1)
		{
			serial_->write((uint8_t)data);
			return;
		}
		else if (data != -1)
		{
			ResponseListener(strings_[Reply::OK_], fi);
			return;
		}

		GSMmodule::mod_read();
		delay(COMMAND_SEND_DELAY);
		serial_->print(F("AT+HTTPDATA="));
		serial_->print(dataLen);
		serial_->print(F(","));
		serial_->print(120000);
		serial_->print(F("\r\n"));
#else
		if (dataLen > 0 && data != -1)
		{
			Serial.write((uint8_t)data);
			return;
		}
		else if (data != -1)
		{
			ResponseListener(strings_[Reply::OK_], fi);
			return;
		}

		GSMmodule::mod_read();
		delay(COMMAND_SEND_DELAY);
		Serial.print(F("AT+HTTPDATA="));
		Serial.print(dataLen);
		Serial.print(F(","));
		Serial.print(120000);
		Serial.print(F("\r\n"));
#endif
		fi.blocked = true;
		fi.timerStart = millis();
		return;
	}
	else if ((millis() - fi.timerStart) > 120000)
	{
		fi.blocked = false;
	}
	else
	{
		fi.blocked = !ResponseListener(strings_[Reply::DOWNLOAD], fi);
	}
}

void GSMmodule::http_init(FeedbackInterface & fi)
{
	if (!fi.blocked)
	{
		GSMmodule::mod_read();
		delay(COMMAND_SEND_DELAY);
#ifdef USE_SOftwareSerial
		assert(serial_);
		serial_->print(F("AT+HTTPINIT\r\n"));
#else
		Serial.print(F("AT+HTTPINIT\r\n"));
#endif
		fi.blocked = true;
		fi.timerStart = millis();
		return;
	}
	else if ((millis() - fi.timerStart) > 12000)
	{
		fi.blocked = false;
	}
	else
	{
		fi.blocked = !ResponseListener(strings_[Reply::OK_], fi);
	}
}

void GSMmodule::tcp_init(FeedbackInterface &fi, const char* url, int port)
{
	if (!fi.blocked)
	{
		GSMmodule::mod_read();
		delay(COMMAND_SEND_DELAY);
#ifdef USE_SOftwareSerial
		assert(serial_);
		serial_->print(F("AT+CIPSTART=\"TCP\",\""));
		serial_->print(url);
		serial_->print(F("\","));
		serial_->print(port);
		serial_->print(F("\r\n"));
#else
		Serial.print(F("AT+CIPSTART=\"TCP\",\""));
		Serial.print(url);
		Serial.print(F("\","));
		Serial.print(port);
		Serial.print(F("\r\n"));
#endif
		fi.blocked = true;
		fi.timerStart = millis();
		return;
	}
	else if ((millis() - fi.timerStart) > 160000)
	{
		fi.blocked = false;
	}
	else
	{
		fi.blocked = !ResponseListener(strings_[Reply::OK_], fi);
	}
}

void GSMmodule::tcp_setRequestLength(FeedbackInterface &fi, int len)
{
	if (!fi.blocked)
	{
		GSMmodule::mod_read();
		delay(COMMAND_SEND_DELAY);
#ifdef USE_SOftwareSerial
		assert(serial_);
		serial_->print(F("AT+CIPSEND="));
		serial_->print(len);
		serial_->print(F("\r\n"));
    serial_->print(char(26));
#else
		Serial.print(F("AT+CIPSEND="));
		Serial.print(len);
		Serial.print(F("\r\n"));
#endif
		fi.blocked = true;
		fi.timerStart = millis();
		return;
	}
	else if ((millis() - fi.timerStart) > 160000)
	{
		fi.blocked = false;
	}
	else
	{
		fi.blocked = !ResponseListener(">", fi);
	}
}

void GSMmodule::tcp_sendRequest(FeedbackInterface &fi, const char * req)
{
	if (!fi.blocked)
	{
		GSMmodule::mod_read();
		delay(COMMAND_SEND_DELAY);
#ifdef USE_SOftwareSerial
		assert(serial_);
		serial_->print(req);
		serial_->print(F("\r\n"));
#else
		Serial.print(req);
		Serial.print(F("\r\n"));
#endif
		fi.blocked = true;
		fi.timerStart = millis();
		return;
	}
	else if ((millis() - fi.timerStart) > 160000)
	{
		fi.blocked = false;
	}
	else
	{
		fi.blocked = !ResponseListener("SEND OK", fi);
	}
}

void GSMmodule::tcp_readData(FeedbackInterface &fi, String & data)
{
	DataRead_helper(fi, data, "CLOSED");
}

void GSMmodule::http_close(FeedbackInterface & fi)
{
	if (!fi.blocked)
	{
		GSMmodule::mod_read();
		delay(3 * COMMAND_SEND_DELAY);
#ifdef USE_SOftwareSerial
		assert(serial_);
		serial_->print(F("AT+HTTPTERM\r\n"));
#else
		Serial.print(F("AT+HTTPTERM\r\n"));
#endif
		fi.blocked = true;
		fi.timerStart = millis();
		return;
	}
	else if ((millis() - fi.timerStart) > 12000)
	{
		fi.blocked = false;
	}
	else
	{
		fi.blocked = !ResponseListener(strings_[Reply::OK_], fi);
	}
}

void GSMmodule::http_context(FeedbackInterface & fi, int cid)
{
	if (!fi.blocked)
	{
		GSMmodule::mod_read();
		delay(COMMAND_SEND_DELAY);
#ifdef USE_SOftwareSerial
		assert(serial_);
		serial_->print(F("AT+HTTPPARA="));
		serial_->print(F("\"CID\","));
		serial_->print(cid);
		serial_->print(F("\r\n"));
#else
		Serial.print(F("AT+HTTPPARA="));
		Serial.print(F("\"CID\","));
		Serial.print(cid);
		Serial.print(F("\r\n"));
#endif
		fi.blocked = true;
		fi.timerStart = millis();
		return;
	}
	else if ((millis() - fi.timerStart) > 6000)
	{
		fi.blocked = false;
	}
	else
	{
		fi.blocked = !ResponseListener(strings_[Reply::OK_], fi);
	}
}

void GSMmodule::http_url(FeedbackInterface & fi, const char * url)
{
	if (!fi.blocked)
	{
		GSMmodule::mod_read();
		delay(COMMAND_SEND_DELAY);
#ifdef USE_SOftwareSerial
		assert(serial_);
		serial_->print(F("AT+HTTPPARA="));
		serial_->print(F("\"URL\",\""));
		serial_->print(url);
		serial_->print(F("\"\r\n"));
#else
		Serial.print(F("AT+HTTPPARA="));
		Serial.print(F("\"URL\",\""));
		Serial.print(url);
		Serial.print(F("\"\r\n"));
#endif
		fi.blocked = true;
		fi.timerStart = millis();
		return;
	}
	else if ((millis() - fi.timerStart) > 10000)
	{
		fi.blocked = false;
	}
	else
	{
		fi.blocked = !ResponseListener(strings_[Reply::OK_], fi);
	}
}

void GSMmodule::http_action(FeedbackInterface& fi, HTTPMETHODS method)
{
	if (!fi.blocked)
	{
		GSMmodule::mod_read();
		delay(COMMAND_SEND_DELAY);
#ifdef USE_SOftwareSerial
		assert(serial_);
		serial_->print(F("AT+HTTPACTION="));
		serial_->print(method);
		serial_->print(F("\r\n"));
#else
		Serial.print(F("AT+HTTPACTION="));
		Serial.print(method);
		Serial.print(F("\r\n"));
#endif
		fi.blocked = true;
		fi.timerStart = millis();
		return;
	}
	else if ((millis() - fi.timerStart) > 10000)
	{
		fi.blocked = false;
	}
	else
	{
		fi.blocked = !ResponseListener(strings_[Reply::OK_], fi);
	}
}

void GSMmodule::mod_getSignalQuality(FeedbackInterface &)
{
}

void GSMmodule::mod_setPhoneFunctionality(FeedbackInterface & fi)
{

	if (!fi.blocked)
	{
		GSMmodule::mod_read();
		delay(COMMAND_SEND_DELAY);
#ifdef USE_SOftwareSerial
		assert(serial_);
		serial_->print(F("AT+CFUN=1\r\n"));
#else
		Serial.print(F("AT+CFUN=1\r\n"));
#endif
		fi.blocked = true;
		fi.timerStart = millis();
		return;
	}
	else if ((millis() - fi.timerStart) > 3000)
	{
		fi.blocked = false;
	}
	else
	{
		fi.blocked = !ResponseListener(strings_[Reply::OK_], fi);
	}
}

void GSMmodule::mod_BatteryStrength(FeedbackInterface &)
{
}

void GSMmodule::mod_atMode(FeedbackInterface & fi)
{

	if (!fi.blocked)
	{
		GSMmodule::mod_read();
		delay(COMMAND_SEND_DELAY);
#ifdef USE_SOftwareSerial
		assert(serial_);
		serial_->print(F("AT\r\n"));
#else
		Serial.print(F("AT\r\n"));
#endif
		fi.blocked = true;
		fi.timerStart = millis();
		return;
	}
	else if ((millis() - fi.timerStart) > 2000)
	{
		fi.blocked = false;
	}
	else
	{
		fi.blocked = !ResponseListener(strings_[Reply::OK_], fi);
	}
}

void GSMmodule::query_helper(Query& qry, const char*& q, const char*& tag)
{
	switch (qry.type)
	{
	case QueryTypes::MSG_FMT:
		q = strings_[MSG_FMT];
		tag = strings_[MSG_FMT_TAG];
		break;
	case QueryTypes::PIN_STATUS:
		q = strings_[PIN_STATUS];
		tag = strings_[PIN_STATUS_TAG];
		break;
	case QueryTypes::APN_STATUS:
		q = strings_[APN_STATUS];
		tag = strings_[APN_STATUS_TAG];
		break;
	case QueryTypes::PHONE_ACTIVITY:
		q = strings_[PHONE_ACTIVITY];
		tag = strings_[PHONE_ACTIVITY_TAG];
		break;
	}
}

bool GSMmodule::DataRead_helper(FeedbackInterface & fi, String &data, const char* endOfData)
{
	static char buf[128];
	int bytesRead = 0;

#ifdef USE_SOftwareSerial
	assert(serial_);
	if (serial_->available())
	{
		bytesRead = serial_->readBytes(buf, 127);
		buf[bytesRead] = 0;
		data = "";//+= buf;
		if (strstr(buf, endOfData))
		{
			fi.success();
			//Serial.println(data);
			Serial.println(buf);
			return true;
		}
		Serial.println(buf);
	}
#else
	if (Serial.available())
	{
		bytesRead = Serial.readBytes(buf, 127);
		buf[bytesRead] = 0;
		data += buf;
		if (strstr(buf, endOfData))
		{
			fi.success();
			return true;
		}
	}
#endif
	/*if (mbuffer.indexOf(strings_[Reply::ERROR_]) != -1)
	{
	fi.restart_();
	return true;
	}
	else if (mbuffer.indexOf(cmd_reply) != -1)
	{
	fi.success();
	return true;
	}*/

	return false;

}

void GSMmodule::mod_reset(FeedbackInterface& fi)
{
	static unsigned char flag = 5;

	if (!fi.blocked)
	{
		fi.blocked = true;
		fi.timerStart = millis();
		if (flag == 5)
		{
			digitalWrite(mreset_pin, LOW);
			flag = 1;
		}
		else
		{
			digitalWrite(mreset_pin, HIGH);
			flag = 5;
		}
		return;
	}
	else if ((millis() - fi.timerStart) > 1000 * flag)
	{
		fi.blocked = false;
		if (flag == 5)
			fi.success();
	}

}

void GSMmodule::mod_powerDown(FeedbackInterface &fi)
{
	if (!fi.blocked)
	{
		GSMmodule::mod_read();
		delay(COMMAND_SEND_DELAY);
#ifdef USE_SOftwareSerial
		assert(serial_);
		serial_->print(F("AT+CPOWD=1\r\n"));
#else
		Serial.print(F("AT+CPOWD=1\r\n"));
#endif
		fi.blocked = true;
		fi.timerStart = millis();
		return;
	}
	else if ((millis() - fi.timerStart) > 5000)
	{
		fi.blocked = false;
	}
	else
	{
		fi.blocked = !ResponseListener(strings_[Reply::POWER_DOWN], fi);
	}
}

void GSMmodule::mod_query(FeedbackInterface& fi, const char *value)
{
	const char *query, *tag;

	query_helper(fi.query, query, tag);

	if (!fi.blocked)
	{
		GSMmodule::mod_read();
		delay(COMMAND_SEND_DELAY);
#ifdef USE_SOftwareSerial
		assert(serial_);
		serial_->print(query);
#else
		Serial.print(query);
#endif
		fi.blocked = true;
		fi.timerStart = millis();
		return;
	}
	else if ((millis() - fi.timerStart) > 5000)
	{
		fi.blocked = false;
	}
	else
	{
		fi.blocked = !QueryListener(tag, value, fi);
	}
}

void GSMmodule::mod_init(uint8_t reset_pin, SoftwareSerial *s)
{
#ifdef USE_SOftwareSerial
	assert(s);
	serial_ = s;
	serial_->begin(9600);
#else
	Serial.begin(9600);
#endif
	mbuffer.reserve(255);
	mreset_pin = reset_pin;
	pinMode(reset_pin, OUTPUT);
	digitalWrite(reset_pin, HIGH);
	return;
}

void GSMmodule::mod_unsolicited(FeedbackInterface& fi, const char* tag, const char* value1, const char* value2)
{

	if (mbuffer.indexOf(strings_[Reply::ERROR_]) != -1)
	{
		//fi.restart_();
		//return;
	}
	else if (mbuffer.indexOf(tag) != -1)
	{
		if (mbuffer.indexOf(value1) != -1 || (value2 && (mbuffer.indexOf(value2) != -1)))
		{
			fi.success();
		}
		else
		{
			//fi.restart_();
		}
		//return;
	}

#ifdef USE_SOftwareSerial
	assert(serial_);
	if (serial_->available())
	{
		mbuffer = serial_->readString();
		Serial.println(mbuffer);
	}
#else
	if (Serial.available())
	{
		mbuffer = Serial.readString();
	}
#endif

}

String GSMmodule::mod_read()
{
	String str = "";
#ifdef USE_SOftwareSerial
	assert(serial_);
	if (serial_->available())
	{
		str = serial_->readString();
	}
#else
	if (Serial.available())
	{
		str = Serial.readString();
	}
#endif
	return str;
}


String GSMmodule::mod_getBuffer()
{
	return mbuffer;
}

bool GSMmodule::ResponseListener(const char *cmd_reply, FeedbackInterface& fi)
{
#ifdef USE_SOftwareSerial
	assert(serial_);
	if (serial_->available())
	{
		mbuffer = serial_->readString();
		Serial.println(mbuffer);
	}
#else
	if (Serial.available())
	{
		mbuffer = Serial.readString();
	}
#endif
	if (mbuffer.indexOf(strings_[Reply::ERROR_]) != -1)
	{
		fi.restart_();
		return true;
	}
	else if (mbuffer.indexOf(cmd_reply) != -1)
	{
		fi.success();
		return true;
	}

	return false;
}

bool GSMmodule::QueryListener(const char *tag, const char *option, FeedbackInterface & fi)
{
	String sub_str;

#ifdef USE_SOftwareSerial
	assert(serial_);
	if (serial_->available())
	{
		mbuffer = serial_->readString();
		Serial.println(mbuffer);
	}
#else
	if (Serial.available())
	{
		mbuffer = Serial.readString();
	}
#endif

	if (mbuffer.indexOf(strings_[Reply::ERROR_]) != -1)
	{
		fi.restart_();
		return true;
	}
	else if (mbuffer.indexOf(tag) != -1)
	{
		sub_str = mbuffer.substring(mbuffer.indexOf(tag) + 6);
		if (sub_str.indexOf(option) != -1)
		{
			fi.success();
			return true;
		}
		else
		{
			fi.restart_();
			return true;
		}
	}

	return false;
}

bool GSMmodule::PromptListener(FeedbackInterface & fi)
{
#ifdef USE_SOftwareSerial
	assert(serial_);
	if (serial_->available())
	{
		mbuffer = serial_->readString();
		Serial.println(mbuffer);
	}
#else
	if (Serial.available())
	{
		mbuffer = Serial.readString();
	}
#endif
	if (mbuffer.indexOf(">") != -1)
	{
		fi.success();
	}
	else
	{
		fi.restart_();
	}
	return true;
}
