#ifndef GSM_MODULE_H
#define GSM_MODULE_H

#include<Arduino.h>
#include<SoftwareSerial.h>

#define USE_SOftwareSerial

enum QueryTypes { MSG_FMT = 5, PIN_STATUS, APN_STATUS, PHONE_ACTIVITY, QTYPES_NUM };
enum QueryTags { MSG_FMT_TAG = QTYPES_NUM, PIN_STATUS_TAG, APN_STATUS_TAG, PHONE_ACTIVITY_TAG };

enum HTTPMETHODS { GET, POST, HEAD };


struct Query
{
	QueryTypes type;
};

class FeedbackInterface
{
public:
	virtual void success() = 0;
	virtual void restart_() = 0;
	unsigned long timerStart;
	bool blocked;
	Query query;
};

class GSMmodule
{
public:
	static void sms_setToTextMode(FeedbackInterface&);
	static void sms_sendSmsNumber(FeedbackInterface&, const char* number);
	static void sms_sendSmsText(FeedbackInterface&, const char* text);
	static void sms_sendEndMark();
	static void sms_readSms(FeedbackInterface&, uint8_t index);
	static void sms_delAllSms(FeedbackInterface&);
	static void call_callNumber(FeedbackInterface&, const char* number);
	static void call_answerCall(FeedbackInterface&);
	static void call_hangoffCall(FeedbackInterface&);
	static void gprs_setAccessPointParams(FeedbackInterface&, const String& apn,
		const String& user, const String& password);
	static void gprs_shutDown(FeedbackInterface&);
	static void gprs_startWirelessConnection(FeedbackInterface&);
	static void gprs_getIPAddress(FeedbackInterface&);
	static void gprs_startTCP(FeedbackInterface&, const char* website, int port);
	static void gprs_bearerProfileSetting(FeedbackInterface&, int cmdType, int cid,
		const char* conParamTag, const char* conParamValue);
	static void http_readData(FeedbackInterface&, String&);
	static void http_inputData(FeedbackInterface&, int dataLen, int data = -1);
	static void http_init(FeedbackInterface&);
	static void tcp_init(FeedbackInterface&, const char* url, int port);
	static void tcp_setRequestLength(FeedbackInterface&, int len);
	static void tcp_sendRequest(FeedbackInterface&, const char* req);
	static void tcp_readData(FeedbackInterface&, String&);
	static void http_close(FeedbackInterface&);
	static void http_context(FeedbackInterface&, int cid);
	static void http_url(FeedbackInterface&, const char* url);
	static void http_action(FeedbackInterface&, HTTPMETHODS method);
	static void mod_getSignalQuality(FeedbackInterface&);
	static void mod_setPhoneFunctionality(FeedbackInterface&);
	static void mod_BatteryStrength(FeedbackInterface&);
	static void mod_atMode(FeedbackInterface&);
	static void mod_reset(FeedbackInterface&);
	static void mod_powerDown(FeedbackInterface&);
	static void mod_query(FeedbackInterface&, const char* value);
	static void mod_init(uint8_t reset_pin, SoftwareSerial* = 0);
	static void mod_unsolicited(FeedbackInterface&, const char* tag, const char* value1, const char* value2=0);
	static String mod_read();
	static String mod_getBuffer();
private:
	static bool ResponseListener(const char* cmd_reply, FeedbackInterface&);
	static bool QueryListener(const char* tag, const char* value, FeedbackInterface&);
	static bool PromptListener(FeedbackInterface&);
	static void query_helper(Query&, const char*& q, const char*& tag);
	static bool DataRead_helper(FeedbackInterface&, String&, const char* endOfData);
#ifdef USE_SOftwareSerial
	static SoftwareSerial* serial_;
#endif
	static String mbuffer;
	static uint8_t mreset_pin;
};


#endif
