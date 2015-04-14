#ifndef IRADIO_H_
#define IRADIO_H_

#include <sstream>
#include <string>

class IRadio
{
public:
	IRadio();
	virtual ~IRadio();

	void startStream();
	void stopStream();

	void increaseStreamNr();
	void decreaseStreamNr();

	unsigned getStreamNr();
	std::string getStreamName();
	std::string getInterpret();
	std::string getTitle();
	void getStreamInfos();

private:
	std::string getWebPageContent( std::string URL, std::string fileName );

	bool 		streamRunning;
	unsigned 	streamNr;
	std::string streamURL;
	std::string streamURLinfo;
	std::string infoURL;
	std::string streamName;
	std::string currentInterpret;
	std::string currentTitle;
	int			streamChildPID;
};

#endif /* IRADIO_H_ */
