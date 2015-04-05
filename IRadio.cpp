#include "IRadio.h"

#include <unistd.h>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
#include <string>
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
			streamURL = "http://www.reliastream.com/cast/tunein.php/rockxradio/playlist.pls";
			infoURL   = "http://tunein.com/radio/rockXradio-s125675/";
			break;
		case 1:
			// Hard Drivin Radio
			streamURL = "http://listen.djcmedia.com/harddrivinradiohigh";
			infoURL   = "http://tunein.com/radio/HDRN---Hard-Drivin-Radio-s116601/";
			break;
		case 2:
			// HDR Country
			streamURL = "http://listen.djcmedia.com/allhdrcountryhigh";
			infoURL   = "http://tunein.com/radio/HDRN---All-HDR-Country-Radio-s141757/";
			break;
	}

	if( (streamChildPID=fork()) == 0 )
	{
		std::string cmd = "mplayer " + streamURL;
		std::system(cmd.c_str());
		exit(0);
	}
	streamRunning = true;
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

std::string IRadio::getStreamName()
{
	return streamName;
}

std::string IRadio::getInterpret()
{
	return currentInterpret;
}

std::string IRadio::getTitle()
{
	return currentTitle;
}

void IRadio::getStreamInfos()
{
	std::string file_name = "index.html";
	char *path = get_current_dir_name();
	std::string sFile(path);
	sFile.append("/").append(file_name);

	// delete file that might exist
	std::string cmd = "rm " + file_name;
	std::system(cmd.c_str());

	// get html file from tuneIn
	cmd = "wget " + infoURL;
	std::system(cmd.c_str());

    // read entire file into string
    std::ifstream is(sFile, std::ifstream::binary);
    std::string fileStr;
    if(is)
    {
        is.seekg(0, is.end);
        int length = is.tellg();
        is.seekg(0, is.beg);

        fileStr.resize(length, ' ');
        char* begin = &*fileStr.begin();

        is.read(begin, length);
        is.close();
    }

    std::string strStart, strEnd;
    std::size_t foundStart, foundEnd;

    strStart   = "<meta property=\"og:title\" content=\"";
    foundStart = fileStr.find( strStart ) + 35;
    fileStr    = fileStr.substr( foundStart );

    strEnd     = "\" />";
    foundEnd   = fileStr.find( strEnd );
    std::string streamName = fileStr.substr( 0, foundEnd );			// save stream name

    strStart   = "<div class=\"now-playing\">Now Playing:</div>";
    strEnd     = "<div id=\"stationNowPlaying\">";
    foundStart = fileStr.find( strStart );
    foundEnd   = fileStr.find( strEnd );
    fileStr    = fileStr.substr( foundStart, foundEnd-foundStart );
    strStart   = "behavior=\"scrolling\">";
    foundStart = fileStr.find( strStart ) + 21;
    fileStr    = fileStr.substr( foundStart );
    strEnd     = "</div>";
    foundEnd   = fileStr.find( strEnd );
    fileStr    = fileStr.substr( 0, foundEnd );

    strEnd     = " - ";
    foundEnd   = fileStr.find( strEnd );
    currentTitle = fileStr.substr( 0, foundEnd );					// save current track

    strStart   = " - ";
    foundStart = fileStr.find( strStart ) + 3;
    currentInterpret = fileStr.substr( foundStart );				// save current interpreter
}
