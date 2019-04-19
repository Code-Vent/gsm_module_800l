#include"protocols.h"

//#ifndef F
//#define F
//#define PROGMEM
//#endif

class SMSSendProtocol : public ProtocolObject
{
	enum SMSSendProtocolStates { SEND, TEXT_MODE, TEXT_MODE_OK, SEND_NUMBER_OK, SEND_OK };
public:
	SMSSendProtocol(const char* number, const char* message);
	virtual ~SMSSendProtocol();
	virtual bool done();
	virtual bool close();
	virtual void success();
	virtual void restart_();
private:
	String number_;
	String message_;
};

class CallNumberProtocol : public ProtocolObject
{
	enum CallNumberProtocolStates {
		CALL, CALL_STATUS, CALL_INPROGRESS
	};
public:
	CallNumberProtocol(const char* number);
	virtual ~CallNumberProtocol();
	virtual bool done();
	virtual bool close();
	virtual void success();
	virtual void restart_();
private:
	String number_;
};

class GPRSProtocol : public ProtocolObject
{
	enum GPRSProtocolStates {
		GPRS_, CHECK_APN_PARAMS, APN_PARAMS_OK, WIRELESS_CONN_OK, SAVE_CONN_TYPE,
		SAVE_APN, SAVE_PASSWORD, SAVE_USER_NAME, OPEN_BEARER, GPRS_READY_
	};
public:
	GPRSProtocol(const char* apn, const char* pin, const char* password);
	virtual ~GPRSProtocol();
	virtual bool done();
	virtual bool close();
	virtual void success();
	virtual void restart_();
private:
	const char* get_IP_address();
	String apn_;
	String user_;
	String password_;
	char* ip_address;
	bool apn_set;
};


class HttpProtocol : public ProtocolObject
{
	enum HttpProtocolStates {
		SESSION_BEGIN, SET_CONTEXT, SET_URL, SET_DATA, PUSH_DATA,
		RUN_METHOD, UNSOLICITED, READ_SESSION_DATA, SESSION_END
	};
public:
	HttpProtocol(const char* url, HTTPMETHODS, void* dat_ptr, int dat_len);
	virtual ~HttpProtocol();
	virtual bool done();
	virtual bool close();
	virtual void success();
	virtual void restart_();
private:
	String url_;
	String https_req;
	unsigned char* data_;
	int len;
	HTTPMETHODS method;
};


class TcpProtocol : public ProtocolObject
{
	enum TcpProtocolStates {
		SESSION_BEGIN, CONNECT_OK, SET_REQ_LENGTH, SEND_REQUEST, SET_DATA, PUSH_DATA,
		RUN_METHOD, REQUEST_STATUS, READ_SESSION_DATA, SESSION_END
	};
public:
	TcpProtocol(const char* host, const char* url, const char* service, int port, const char* method,
		void* dat_ptr, int dat_len);
	virtual ~TcpProtocol();
	virtual bool done();
	virtual bool close();
	virtual void success();
	virtual void restart_();
private:
	String url_;
	String service_;
  String host_;
	String method;
	String request;
	int port;
	unsigned char* data_;
	int len;
};

class ModemInitProtocol : public ProtocolObject
{
	enum ModemInitProtocolState { RESET, AT, AT_OK, CFUN, CFUN_OK, READY };
public:
	ModemInitProtocol();
	virtual ~ModemInitProtocol();
	virtual bool done();
	virtual bool close();
	virtual void success();
	virtual void restart_();
};

ProtocolObject * ProtocolObjectFactory::makeSMSSendProtocol(const char * number, const char * message) const
{
	return new SMSSendProtocol(number, message);
}

ProtocolObject * ProtocolObjectFactory::makeCallNumberProtocol(const char * number) const
{
	return new CallNumberProtocol(number);
}

ProtocolObject * ProtocolObjectFactory::makeHttpProtocol(const char * url, HTTPMETHODS m,
	 void* dat_ptr, int dat_len) const
{
	return new HttpProtocol(url, m, dat_ptr, dat_len);
}

ProtocolObject * ProtocolObjectFactory::makeTcpProtocol(const char* host, const char * url, const char * service, int port, const char * method, void * dat_ptr, int dat_len) const
{
	return new TcpProtocol(host, url, service, port, method, dat_ptr, dat_len);
}

ProtocolObject * ProtocolObjectFactory::makeGPRSProtocol(const char * apn, const char * pin, const char * password) const
{
	return new GPRSProtocol(apn, pin, password);
}

ProtocolObject * ProtocolObjectFactory::makeModemInitProtocol() const
{
	return new ModemInitProtocol();
}

ProtocolObjectFactory* ProtocolObjectFactory::getInstance()
{
	// TODO: insert return statement here
	static ProtocolObjectFactory theInstance;

	return &theInstance;
}

ProtocolObjectFactory::ProtocolObjectFactory()
{
}

SMSSendProtocol::SMSSendProtocol(const char * number, const char * message)
	:ProtocolObject(SEND, SEND_OK)
{
	number_ = number;
	message_ = message;
	curr_state = initial_state;
}

SMSSendProtocol::~SMSSendProtocol()
{
}

bool SMSSendProtocol::done()
{
	switch (curr_state)
	{
	case SEND:
		GSMmodule::sms_setToTextMode(*this);
		break;
	case TEXT_MODE:
		this->query.type = QueryTypes::MSG_FMT;
		GSMmodule::mod_query(*this, "1");
		break;
	case TEXT_MODE_OK:
		GSMmodule::sms_sendSmsNumber(*this, number_.c_str());
		break;
	case SEND_NUMBER_OK:
		GSMmodule::sms_sendSmsText(*this, message_.c_str());
		break;
	}
	return curr_state == final_state;
}

bool SMSSendProtocol::close()
{
	if (!closed)
	{
		GSMmodule::sms_sendEndMark();
		closed = true;
	}
	return closed;
}

void SMSSendProtocol::success()
{
	if (curr_state == SEND)
	{
		curr_state = TEXT_MODE;
	}
	else if (curr_state == TEXT_MODE)
	{
		curr_state = TEXT_MODE_OK;
	}
	else if (curr_state == TEXT_MODE_OK)
	{
		curr_state = SEND_NUMBER_OK;
	}
	else if (curr_state == SEND_NUMBER_OK)
	{
		GSMmodule::sms_sendEndMark();
		curr_state = final_state;
	}
	else
	{
		closed = true;
	}
}

void SMSSendProtocol::restart_()
{
	curr_state = initial_state;
}

CallNumberProtocol::CallNumberProtocol(const char * number)
	:ProtocolObject(CALL, CALL_INPROGRESS)
{
	number_ = number;
	curr_state = initial_state;
}

CallNumberProtocol::~CallNumberProtocol()
{
}

bool CallNumberProtocol::done()
{
	switch (curr_state)
	{
	case CALL:
		GSMmodule::call_callNumber(*this, number_.c_str());
		break;
	case CALL_STATUS:
		this->query.type = QueryTypes::PHONE_ACTIVITY;
		GSMmodule::mod_query(*this, "4");
		break;
	}
	return curr_state == final_state;
}

bool CallNumberProtocol::close()
{
	GSMmodule::call_hangoffCall(*this);
	return closed;
}

void CallNumberProtocol::success()
{
	if (curr_state == CALL)
	{
		curr_state = CALL_STATUS;
	}
	else if (curr_state == CALL_STATUS)
	{
		curr_state = final_state;
	}
	else
	{
		closed = true;
	}
}

void CallNumberProtocol::restart_()
{
	curr_state = initial_state;
}

HttpProtocol::HttpProtocol(const char * url, HTTPMETHODS m, void* dat_ptr, int dat_len)
	:ProtocolObject(SESSION_BEGIN, SESSION_END)
{
	url_ = url;
	method = m;
	data_ = (unsigned char*)dat_ptr;
	len = dat_len;
	curr_state = initial_state;
	result.reserve(255);
}

HttpProtocol::~HttpProtocol()
{
}

bool HttpProtocol::done()
{
	switch (curr_state)
	{
	case SESSION_BEGIN:
		GSMmodule::http_init(*this);
		break;
	case SET_CONTEXT:
		GSMmodule::http_context(*this, 1);
		break;
	case SET_URL:
		GSMmodule::http_url(*this, url_.c_str());
		break;
	case SET_DATA:
		GSMmodule::http_inputData(*this, len);
		break;
	case PUSH_DATA:
		if (len)
			GSMmodule::http_inputData(*this, --len, *data_++);
		else
			GSMmodule::http_inputData(*this, len, '#');
		break;
	case RUN_METHOD:
		GSMmodule::http_action(*this, method);
		break;
	case UNSOLICITED:
		GSMmodule::mod_unsolicited(*this, "+HTTPACTION:", "200");
		break;
	case READ_SESSION_DATA:
		GSMmodule::http_readData(*this, result);
		break;
	}
	return curr_state == final_state;
}

bool HttpProtocol::close()
{
	//GSMmodule::http_close(*this);
	//return closed;
	return true;
}

void HttpProtocol::success()
{
	if (curr_state == SESSION_BEGIN)
	{
		curr_state = SET_CONTEXT;
	}
	else if (curr_state == SET_CONTEXT)
	{
		curr_state = SET_URL;
	}
	else if (curr_state == SET_URL)
	{
		if (method == HTTPMETHODS::POST)
			curr_state = SET_DATA;
		else
			curr_state = RUN_METHOD;
	}
	else if (curr_state == SET_DATA)
	{
		curr_state = PUSH_DATA;
	}
	else if (curr_state == PUSH_DATA)
	{
		curr_state = RUN_METHOD;
	}
	else if (curr_state == RUN_METHOD)
	{
		curr_state = UNSOLICITED;
	}
	else if (curr_state == UNSOLICITED)
	{
		if (method == HTTPMETHODS::POST)
			curr_state = final_state;
		else
			curr_state = READ_SESSION_DATA;
	}
	else if (curr_state == READ_SESSION_DATA)
	{
		curr_state = final_state;
	}
	else
	{
		closed = true;
		//Serial.println("Session Closed Successfully");
	}
}

void HttpProtocol::restart_()
{
	if (curr_state != final_state)
		curr_state = initial_state;
}

GPRSProtocol::GPRSProtocol(const char * apn, const char * user, const char * password)
	:ProtocolObject(GPRS_, GPRS_READY_)
{
	apn_ = apn;
	user_ = user;
	password_ = password;
	curr_state = initial_state;
	apn_set = false;
	ip_address = 0;
}

GPRSProtocol::~GPRSProtocol()
{
	if (ip_address)
		delete[] ip_address;
}

bool GPRSProtocol::done()
{
	switch (curr_state)
	{
	case GPRS_:
		if (!apn_set)
			GSMmodule::gprs_setAccessPointParams(*this, apn_, user_, password_);
		else
			curr_state = APN_PARAMS_OK;
		break;
	case CHECK_APN_PARAMS:
		this->query.type = QueryTypes::APN_STATUS;
		GSMmodule::mod_query(*this, apn_.c_str());
		break;
	case APN_PARAMS_OK:
		GSMmodule::gprs_startWirelessConnection(*this);
		break;
	case WIRELESS_CONN_OK:
		GSMmodule::gprs_getIPAddress(*this);
		break;
	case SAVE_CONN_TYPE:
		GSMmodule::gprs_bearerProfileSetting(*this, 3, 1, "CONTYPE", "GPRS");
		break;
	case SAVE_APN:
		GSMmodule::gprs_bearerProfileSetting(*this, 3, 1, "APN", apn_.c_str());
		break;
	case SAVE_PASSWORD:
		GSMmodule::gprs_bearerProfileSetting(*this, 3, 1, "PWD", password_.c_str());
		break;
	case SAVE_USER_NAME:
		GSMmodule::gprs_bearerProfileSetting(*this, 3, 1, "USER", user_.c_str());
		break;
	case OPEN_BEARER:
		GSMmodule::gprs_bearerProfileSetting(*this, 1, 1, 0, 0);
		break;
	}
	return curr_state == final_state;
}

bool GPRSProtocol::close()
{
	GSMmodule::gprs_shutDown(*this);
	return closed;
}

void GPRSProtocol::success()
{

	if (curr_state == GPRS_)
	{
		curr_state = CHECK_APN_PARAMS;
	}
	else if (curr_state == CHECK_APN_PARAMS)
	{
		curr_state = APN_PARAMS_OK;
		apn_set = true;
	}
	else if (curr_state == APN_PARAMS_OK)
	{
		curr_state = WIRELESS_CONN_OK;
	}
	else if (curr_state == WIRELESS_CONN_OK)
	{
		//Get IP Address
		ip_address = new char[GSMmodule::mod_getBuffer().length() + 1];
		strcpy(ip_address, GSMmodule::mod_getBuffer().c_str());
		curr_state = SAVE_CONN_TYPE;
	}
	else if (curr_state == SAVE_CONN_TYPE)
	{
		curr_state = SAVE_APN;
	}
	else if (curr_state == SAVE_APN)
	{
		curr_state = SAVE_PASSWORD;
	}
	else if (curr_state == SAVE_PASSWORD)
	{
		curr_state = SAVE_USER_NAME;
	}
	else if (curr_state == SAVE_USER_NAME)
	{
		curr_state = OPEN_BEARER;
	}
	else if (curr_state == OPEN_BEARER)
	{
		curr_state = final_state;
	}
	else
	{
		closed = true;
	}
}

void GPRSProtocol::restart_()
{
	if (curr_state < WIRELESS_CONN_OK)
		curr_state = initial_state;
}

const char * GPRSProtocol::get_IP_address()
{
	char* s = ip_address;
	while (s && isdigit(*s) == 0)++s;
	return s;
}

ModemInitProtocol::ModemInitProtocol()
	:ProtocolObject(RESET, READY)
{
	curr_state = initial_state;
}

ModemInitProtocol::~ModemInitProtocol()
{
}

bool ModemInitProtocol::done()
{
	switch (curr_state)
	{
	case RESET:
		GSMmodule::mod_reset(*this);
		break;
	case AT:
		GSMmodule::mod_atMode(*this);
		break;
	case AT_OK:
		GSMmodule::mod_setPhoneFunctionality(*this);
		break;
	case CFUN_OK:
		this->query.type = QueryTypes::PIN_STATUS;
		GSMmodule::mod_query(*this, "READY");
		break;
	}
	return curr_state == final_state;
}

bool ModemInitProtocol::close()
{
	GSMmodule::mod_powerDown(*this);
	return closed;
}

void ModemInitProtocol::success()
{
	if (curr_state == RESET)
	{
		curr_state = AT;
	}
	else if (curr_state == AT)
	{
		curr_state = AT_OK;
	}
	else if (curr_state == AT_OK)
	{
		curr_state = CFUN_OK;
	}
	else if (curr_state == CFUN_OK)
	{
		curr_state = final_state;
	}
	else
	{
		closed = true;
	}
}

void ModemInitProtocol::restart_()
{
	curr_state = AT;//initial_state;
}

ProtocolObject::ProtocolObject(const int initial, const int final_)
	:initial_state(initial), final_state(final_)
{
	blocked = false;
	closed = false;
}

ProtocolObject::~ProtocolObject()
{
}

TcpProtocol::TcpProtocol(const char* host, const char * url, const char * service, int prt, const char * meth, void * dat_ptr, int dat_len)
	:ProtocolObject(SESSION_BEGIN, SESSION_END)
{
	url_ = url;
	method = meth;
	service_ = service;
  host_ = host;
	port = prt;
	request = meth;
	request += " ";
	request += url;
	request += " ";
	request += service;
	data_ = (unsigned char*)dat_ptr;
	len = dat_len;
	curr_state = initial_state;
	result.reserve(255);
}

TcpProtocol::~TcpProtocol()
{
}

bool TcpProtocol::done()
{
	switch (curr_state)
	{
	case SESSION_BEGIN:
		GSMmodule::tcp_init(*this, host_.c_str(), port);
		break;
  case CONNECT_OK:
    GSMmodule::mod_unsolicited(*this, "CONNECT", "OK", "ALREADY");
    break;
	case SET_REQ_LENGTH:
		GSMmodule::tcp_setRequestLength(*this, request.length());
		break;
	case SEND_REQUEST:
		GSMmodule::tcp_sendRequest(*this, request.c_str());
		break;
	case REQUEST_STATUS:
		GSMmodule::mod_unsolicited(*this, service_.c_str(), "200");
    //Serial.println("Here...");
		break;
	case READ_SESSION_DATA:
		GSMmodule::tcp_readData(*this, result);
		break;
	}
	return curr_state == final_state;
}

bool TcpProtocol::close()
{
	return closed;
}

void TcpProtocol::success()
{
	if (curr_state == SESSION_BEGIN)
	{
		curr_state = CONNECT_OK;
	}
  else if(curr_state == CONNECT_OK)
  {
    curr_state = SET_REQ_LENGTH;
  }
	else if (curr_state == SET_REQ_LENGTH)
	{
		curr_state = SEND_REQUEST;
	}
	else if (curr_state == SEND_REQUEST)
	{
		curr_state = REQUEST_STATUS;
    //Serial.println("Request OK.");
    //curr_state = READ_SESSION_DATA;
	}
	else if (curr_state == REQUEST_STATUS)
	{
		curr_state = READ_SESSION_DATA;
	}
	else if (curr_state == READ_SESSION_DATA)
	{
		curr_state = final_state;
		closed = true;
	}
	else
	{
		closed = true;
	}
}

void TcpProtocol::restart_()
{
	curr_state = initial_state;
}
