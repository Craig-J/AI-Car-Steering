#include "logger.h"
namespace agn
{
	// Static member initialization
	Log::ERRORVERBOSITY Log::error_vebosity_ = Log::ERRORVERBOSITY::ALL;
	Log::OUTPUTVERBOSITY Log::output_vebosity_ = Log::OUTPUTVERBOSITY::ALL;
	bool Log::timestamps_ = false;
	bool Log::log_to_file_ = false;
	bool Log::log_to_console_ = true;
}