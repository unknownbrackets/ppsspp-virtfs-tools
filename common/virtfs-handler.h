#pragma once

#ifdef _WIN32
#define EXPORT extern "C" __declspec(dllexport)
#elif __GNUC__ >= 4
#define EXPORT extern "C" __attribute__((visibility("default")))
#else
#define EXPORT
#endif

enum ErrLevel
{
	L_NOTICE = 1,
	L_ERROR = 2,
	L_WARNING = 3,
	L_INFO = 4,
	L_DEBUG = 5,
	L_VERBOSE = 6,
};

enum FileMove
{
	FILEMOVE_BEGIN = 0,
	FILEMOVE_CURRENT = 1,
	FILEMOVE_END = 2
};

typedef long long offset_t;
typedef int handle_t;
typedef void (*LogFunc)(handle_t fd, ErrLevel level, const char *msg);
