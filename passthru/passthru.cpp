#include <cstdio>
#include <io.h>
#include <fcntl.h>
#include <string>

#include "../common/virtfs-handler.h"

static LogFunc g_logger;

#ifdef _MSC_VER
#define open _open
#define close _close
#define lseek _lseek
#define read _read
#define O_RDONLY _O_RDONLY
#endif

EXPORT bool Init(LogFunc logger)
{
	g_logger = logger;
	g_logger(-1, L_DEBUG, "passthru handler initialized");
	return true;
}

EXPORT void Shutdown()
{
	g_logger(-1, L_DEBUG, "passthru handler shutdown");
}

EXPORT handle_t Open(const char *rootPath, const char *filename)
{
	std::string full = std::string(rootPath) + "/" + filename;
	handle_t handle = (handle_t) open(full.c_str(), O_RDONLY, 0);
	g_logger(handle, L_DEBUG, "Opened file");
	return handle;
}

EXPORT void Close(handle_t handle)
{
	g_logger(handle, L_DEBUG, "Closed file");
	close((int) handle);
}

EXPORT offset_t Seek(handle_t handle, offset_t offset, FileMove origin)
{
	g_logger(handle, L_DEBUG, "Seek within file");
	return lseek((int) handle, (long) offset, (int) origin);
}

EXPORT offset_t Read(handle_t handle, void *data, offset_t size)
{
	g_logger(handle, L_DEBUG, "Read from file");
	return read((int) handle, data, (unsigned int) size);
}
