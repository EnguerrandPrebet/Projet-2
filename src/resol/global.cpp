#include "global.hpp"

#include <iostream>

using namespace std;

Option Global::option; // déclaration explicite

static ostream& debug_stream = cout;
static ostream& warning_stream = cout;
static ostream& error_stream = cerr;

class null_buffer : public streambuf
{
public:
  int overflow(int c) { return c; }
};

class null_ostream : private null_buffer, public ostream
{
public:
	null_ostream() : std::ostream( this ) {}
	null_buffer* rdbuf() const { return (null_buffer*) this; }
};

std::ostream& Global::DEBUG()
{
	return debug_stream;
}

ostream& Global::DEBUG(unsigned int required)
{
	static null_ostream cnull;

	if (required <= Global::option.debug)
	{
		return debug_stream;
	}
	else
		return cnull;
}

std::ostream& Global::WARNING()
{
	warning_stream << "Warning: ";

	return warning_stream;
}

std::ostream& Global::ERROR()
{
	error_stream << "Error: ";

	return error_stream;
}
