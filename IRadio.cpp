#include "IRadio.h"
#include <cstdlib>
#include <sstream>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

IRadio::IRadio()
{
	// force headphones as standard audio output device
	// 1=headphones
	std::system("amixer -c 0 cset numid=3 1");

	// force HDMI as standard audio output device
	// 2=hdmi
	// std::system("amixer -c 0 cset numid=3 2");

	streamRunning		= false;
	streamNr			= 0;
	streamName			= "";
	currentInterpret	= "";
	currentTitle		= "";
	streamChildPID		= 0;
}

IRadio::~IRadio()
{
	// set standard audio output device back to automatic
	// 0=auto
	std::system("amixer -c 0 cset numid=3 1");

	stopStream();
}

void IRadio::startStream()
{
	stopStream();

	switch( streamNr )
	{
		case 0:
			// rockXradio
			if( (streamChildPID=fork()) == 0 )
			{
				std::system("mplayer http://www.reliastream.com/cast/tunein.php/rockxradio/playlist.pls");
				exit(0);
			}
			streamName    = "rockXradio";
			streamRunning = true;
			break;

		case 1:
			// Hard Drivin’ Radio
			if( (streamChildPID=fork()) == 0 )
			{
				std::system("mplayer http://listen.djcmedia.com/harddrivinradiohigh");
				exit(0);
			}
			streamName    = "Hard Drivin’ Radio";
			streamRunning = true;
			break;

		case 2:
			// HDR Country
			if( (streamChildPID=fork()) == 0 )
			{
				std::system("mplayer http://listen.djcmedia.com/allhdrcountryhigh");
				exit(0);
			}
			streamName    = "HDR Country";
			streamRunning = true;
			break;
		default:
			// do nothing
			streamName    = "ERROR";
			streamNr	  = 0;
			break;
	}
}

void IRadio::stopStream()
{
	if( streamRunning == true )
	{
		system("killall mplayer");
		streamRunning = false;
	}
}

void IRadio::increaseStreamNr()
{
	if(streamNr<2)
		++streamNr;
	startStream();
}

void IRadio::decreaseStreamNr()
{
	if(streamNr>0)
		--streamNr;
	startStream();
}

unsigned IRadio::getStreamNr()
{
	return streamNr;
}
