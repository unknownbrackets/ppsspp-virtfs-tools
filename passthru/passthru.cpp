#include <cstdio>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <fcntl.h>
#include <string>
#include <vector>

#include "../common/virtfs-handler.h"

struct Logger {
	LogFunc func;
	void *arg;
};

static std::vector<Logger> g_loggers;

static void LOG(handle_t handle, ErrLevel level, const char *msg)
{
	if (g_loggers.empty())
		return;

	const Logger &logger = g_loggers.back();
	logger.func(logger.arg, handle, level, msg);
}

#ifdef _MSC_VER
#define open _open
#define close _close
#define lseek _lseek
#define read _read
#define O_RDONLY _O_RDONLY
#define O_BINARY _O_BINARY
#endif

EXPORT int Version()
{
	return 2;
}

EXPORT bool Init(LogFunc loggerFunc, void *loggerArg)
{
	Logger logger;
	logger.func = loggerFunc;
	logger.arg = loggerArg;
	g_loggers.push_back(logger);

	LOG(-1, L_DEBUG, "passthru handler initialized");
	return true;
}

EXPORT void Shutdown(void *loggerArg)
{
	LOG(-1, L_DEBUG, "passthru handler shutdown");

	for (size_t i = 0; i < g_loggers.size(); ++i) {
		if (g_loggers[i].arg == loggerArg) {
			g_loggers.erase(g_loggers.begin() + i);
			break;
		}
	}
}

EXPORT handle_t Open(const char *rootPath, const char *filename)
{
	std::string full = std::string(rootPath) + "/" + filename;
	handle_t handle = (handle_t) open(full.c_str(), O_RDONLY | O_BINARY);
	LOG(handle, L_DEBUG, "Opened file");
	return handle;
}

EXPORT void Close(handle_t handle)
{
	LOG(handle, L_DEBUG, "Closed file");
	close((int) handle);
}

EXPORT offset_t Seek(handle_t handle, offset_t offset, FileMove origin)
{
	LOG(handle, L_DEBUG, "Seek within file");
	return lseek((int) handle, (long) offset, (int) origin);
}

EXPORT offset_t Read(handle_t handle, void *data, offset_t size)
{
	offset_t bytes = 0;
	int result = 0;
	do
	{
		bytes += result;
		result = read((int) handle, (char *)data + bytes, (unsigned int) (size - bytes));
	}
	while (result > 0);
	LOG(handle, L_DEBUG, "Read from file");
	return bytes;
}
