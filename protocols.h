#ifndef PROTOCOLS_H_
#define PROTOCOLS_H_

#include"gsm_module.h"


class ProtocolObject : public FeedbackInterface
{
public:
	virtual bool done() = 0;
	virtual bool close() = 0;
	virtual ~ProtocolObject();
	String result;
protected:
	ProtocolObject(const int initial_state, const int final_state);
	const int initial_state;
	const int final_state;
	int curr_state;
	bool closed;
};


class ProtocolObjectFactory
{
public:

	virtual ProtocolObject* makeSMSSendProtocol(const char* number, const char* message) const;
	virtual ProtocolObject* makeCallNumberProtocol(const char * number) const;
	virtual ProtocolObject* makeHttpProtocol(const char* url, HTTPMETHODS, void* dat_ptr = 0, int dat_len = 0) const;
	virtual ProtocolObject* makeTcpProtocol(const char* host, const char* url, const char* service, int port, const char* method, void* dat_ptr = 0, int dat_len = 0) const;
	virtual ProtocolObject* makeGPRSProtocol(const char* apn, const char* pin,
		const char* password) const;
	virtual ProtocolObject* makeModemInitProtocol()const;
	static ProtocolObjectFactory* getInstance();
protected:
	ProtocolObjectFactory();
};


#endif
