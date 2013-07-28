#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>

#include "CommonTypes.h"
#include "BlockDevices.h"
#include "ISOFile.h"

#if _MSC_VER
#define snprintf _snprintf
#endif

enum ProcessResult
{
	RESULT_SUCCESS = 0,
	RESULT_INPUT_READ_FAIL = 1,
	RESULT_INPUT_PARSE_FAIL = 2,
	RESULT_OUTPUT_WRITE_FAIL = 3,
};

void usage(const char *argv0);
ProcessResult process(const char *iso, const char *output);

int main(int argc, char *argv[])
{
	const char *iso;
	const char *output = ".ppsspp-index.lst";

	switch (argc)
	{
	case 3:
		output = argv[2];
		// Intentional fall-through.
	case 2:
		iso = argv[1];
		break;

	case 0:
		usage("-");
		return 1;

	default:
		usage(argv[0]);
		return 1;
	}

	ProcessResult result = process(iso, output);
	switch (result)
	{
	case RESULT_SUCCESS:
		break;

	case RESULT_INPUT_READ_FAIL:
		fprintf(stderr, "ERROR: Unable to open input file %s\n", iso);
		break;

	case RESULT_INPUT_PARSE_FAIL:
		fprintf(stderr, "ERROR: File does not appear to be a valid iso: %s\n", iso);
		break;

	case RESULT_OUTPUT_WRITE_FAIL:
		fprintf(stderr, "ERROR: Unable to write index to: %s\n", output);
		break;
	}

	return (int)result;
}

void usage(const char *argv0)
{
	fprintf(stderr, "%s FILE.iso [.ppsspp-index.lst]\n", argv0);
	fprintf(stderr, "\n");
	fprintf(stderr, "Generates an index file suitable for running an ISO as a directory.\n");
	fprintf(stderr, "\n");
}

void descend(BlockDevice *blockDevice, u32 startsector, u32 dirsize, std::ofstream &out, const std::string &path)
{
	for (u32 secnum = startsector, endsector = dirsize/2048 + startsector; secnum < endsector; ++secnum)
	{
		u8 theSector[2048];
		blockDevice->ReadBlock(secnum, theSector);

		for (int offset = 0; offset < 2048; )
		{
			DirectoryEntry &dir = *(DirectoryEntry *)&theSector[offset];
			u8 sz = theSector[offset];

			// Nothing left in this sector.  There might be more in the next one.
			if (sz == 0)
				break;

			const int IDENTIFIER_OFFSET = 33;
			if (offset + IDENTIFIER_OFFSET + dir.identifierLength > 2048)
			{
				fprintf(stderr, "ERROR: Directory entry crosses sectors, corrupt iso?");
				return;
			}

			offset += dir.size;

			// Current dir, skip.
			if (dir.identifierLength == 1 && (dir.firstIdChar == '\x00' || dir.firstIdChar == '.'))
				continue;
			// Parent dir, skip.
			else if (dir.identifierLength == 1 && dir.firstIdChar == '\x01')
				continue;

			bool isFile = (dir.flags & 2) ? false : true;
			std::string name = std::string((char *)&dir.firstIdChar, dir.identifierLength);

			if (!isFile)
			{
				if (dir.firstDataSectorLE == startsector)
					fprintf(stderr, "ERROR: Recursive directory entry, broken iso.");
				else
					descend(blockDevice, dir.firstDataSectorLE, dir.dataLengthLE, out, path + name + "/");
			}
			else
			{
				char temp[1024];
				snprintf(temp, 1024, "0x%08x %s%s\n", dir.firstDataSectorLE, path.c_str(), name.c_str());
				out << temp;
			}
		}
	}
}

ProcessResult process(const char *iso, const char *output)
{
	BlockDevice *blockDevice = constructBlockDevice(iso);
	if (!blockDevice)
		return RESULT_INPUT_READ_FAIL;

	std::ofstream out;
	out.open(output);

	if (!out)
	{
		delete blockDevice;
		return RESULT_OUTPUT_WRITE_FAIL;
	}

	VolDescriptor desc;
	blockDevice->ReadBlock(16, (u8*)&desc);

	if (memcmp(desc.cd001, "CD001", 5))
	{
		fprintf(stderr, "ERROR: Block 16 does not start with CD001.\n");
		delete blockDevice;
		return RESULT_INPUT_PARSE_FAIL;
	}

	u32 rootSector = desc.root.firstDataSectorLE;
	u32 rootSize = desc.root.dataLengthLE;

	descend(blockDevice, rootSector, rootSize, out, "");

	delete blockDevice;
	return RESULT_SUCCESS;
}
