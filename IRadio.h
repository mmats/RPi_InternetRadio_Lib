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
	void stopOrResumeStream();

	void increaseStreamNr();
	void decreaseStreamNr();

	bool streamHasChanged();
	bool streamIsRunning();
	unsigned getStreamNr();
	std::string getStreamName();
	std::string getInterpret();
	std::string getTitle();
	void getStreamInfos();

private:
	std::string getInfoOutOfText( std::string text, std::string startStr, std::string endStr );
	std::string getWebPageContent( std::string URL, std::string fileName );
	std::string getFileContent( std::string fileName );

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
