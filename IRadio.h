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

private:
	bool 		streamRunning;
	unsigned 	streamNr;
	std::string streamName;
	std::string currentInterpret;
	std::string currentTitle;
	int			streamChildPID;
};

#endif /* IRADIO_H_ */
