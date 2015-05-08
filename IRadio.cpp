#include "IRadio.h"

#include <unistd.h>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <sys/types.h>
#include <signal.h>

IRadio::IRadio()
{
	// force headphones as standard audio output device
	// 1=headphones
	std::system("amixer --quiet -c 0 cset numid=3 1");

	// force HDMI as standard audio output device
	// 2=hdmi
	// std::system("amixer -c 0 cset numid=3 2");

	streamRunning		= false;
	streamNr			= 0;
	infoURL 			= "";
	streamURL 			= "";
	streamURLinfo 		= "";
	streamName			= "";
	currentInterpret	= "";
	currentTitle		= "";
	streamChildPID		= 0;
}
IRadio::~IRadio()
{
	// set standard audio output device back to automatic
	// 0=auto
	std::system("amixer --quiet -c 0 cset numid=3 1");

	stopStream();
}

void IRadio::startStream()
{
	stopStream();

	// get URLs from file
	std::istringstream is( getFileContent( "RPi_Radio_Streams" ) );
    std::vector<std::string> v{};
    auto i = 0;
    while( is )
    {
    	v.resize( v.size()+1 );
    	is >> v.at(i++);
    }
    v.resize( v.size()-1 );

    // detect over- and underflow
    if( streamNr < 0 )
    	streamNr = 0;
    else if( streamNr > v.size()-1 )
    	streamNr = v.size()-1;

    infoURL = v.at( streamNr );

	while( streamURL.empty() )
		getStreamInfos();

	if( (streamChildPID=fork()) == 0 )
	{
		std::string cmd = "mplayer file > /dev/null 2>&1 " + streamURL;
		std::system(cmd.c_str());
		exit(0);
		std::cout << "Child: Should never reach this line of code!\n";
	}
	streamRunning = true;
}
void IRadio::stopStream()
{
	if( streamIsRunning() )
	{
		system("killall --quiet mplayer");
		streamRunning    = false;
		streamURL        = "";
		streamURLinfo    = "";
		streamName       = "";
		currentInterpret = "";
		currentTitle     = "";
	}
}
void IRadio::stopOrResumeStream()
{
	if( streamIsRunning() )
		stopStream();
	else
		startStream();
}

void IRadio::increaseStreamNr()
{
	++streamNr;
	startStream();
}
void IRadio::decreaseStreamNr()
{
	--streamNr;
	startStream();
}

bool IRadio::streamHasChanged()
{
	static unsigned    laststreamNr      = -1;
	static std::string laststreamName    = "";
	static std::string lastInterpret     = "";
	static std::string lastTitle         = "";

	if( (streamNr          != laststreamNr)      ||
		(streamName        != laststreamName)    ||
	    (currentInterpret  != lastInterpret)     ||
	    (currentTitle      != lastTitle) )
	{
		laststreamNr      = streamNr;
		laststreamName    = streamName;
		lastInterpret     = currentInterpret;
		lastTitle         = currentTitle;

		return true;
	}

	return false;
}
bool IRadio::streamIsRunning()
{
	return streamRunning;
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
    std::string fileStr;

	fileStr = getWebPageContent( infoURL, "tunein_info" );

	// get TuneIn content
    fileStr = getInfoOutOfText( fileStr, "TuneIn.payload", "}}}" );

    // find stream name
    streamName = getInfoOutOfText( fileStr, "\"description\":\"", "\"" );

    // find stream URL info
    streamURLinfo = getInfoOutOfText( fileStr, "\"StreamUrl\":\"", "\"" );

    // find song information
    fileStr = getInfoOutOfText( fileStr, "\"SongPlayingTitle\":", "," );
    if( fileStr == "" )
    {
    	currentInterpret = "I";
    	currentTitle	 = "T";
    }
    else
    {
    	std::string strEnd = " - ";
    	std::size_t foundEnd   = fileStr.find( " - " );
    	currentInterpret = fileStr.substr( foundEnd+strEnd.length(), fileStr.length()-foundEnd-strEnd.length()-1 );
    	currentTitle     = fileStr.substr( 1, foundEnd-1 );
    }

    // find real stream URL
  	fileStr = getWebPageContent( streamURLinfo, "streamurl" );
  	streamURL = getInfoOutOfText( fileStr, "\"Url\": \"", "\"" );
}
std::string IRadio::getInfoOutOfText( std::string text, std::string startStr, std::string endStr )
{
    std::string tmpStr;
    std::size_t foundStart, foundEnd;

    foundStart = text.find( startStr );
    tmpStr     = text.substr( foundStart );
    foundEnd   = tmpStr.find( endStr, startStr.length() );
    tmpStr     = tmpStr.substr( startStr.length(), foundEnd-startStr.length() );
    if( tmpStr == "null" )
    	return "";
    return tmpStr;
}
std::string IRadio::getWebPageContent( std::string URL, std::string fileName )
{
	std::string cmd{}, fileStr{};

	// get content from webpage
	cmd = "wget -O " + fileName + " -o log_" + fileName + " " + URL;
	std::system(cmd.c_str());

	fileStr = getFileContent( fileName );

	// remove generated file
	cmd = "rm " + fileName;
	std::system(cmd.c_str());
	cmd = "rm log_" + fileName;
	std::system(cmd.c_str());

	return fileStr;
}
std::string IRadio::getFileContent( std::string fileName )
{
	std::string fileStr{};

	// get path
	char *path = get_current_dir_name();
	std::string sFile(path);
	sFile.append("/").append(fileName);

    // read entire file into string
    do{
    	std::ifstream is(sFile, std::ifstream::binary);
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
    }while( fileStr.empty() );

    return fileStr;
}
