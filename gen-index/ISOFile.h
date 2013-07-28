// Copyright (c) 2012- PPSSPP Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0 or later versions.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official git repository and contact information can be found at
// https://github.com/hrydgard/ppsspp and http://www.ppsspp.org/.

#pragma once

#include "CommonTypes.h"

#pragma pack(push)
#pragma pack(1)
struct DirectoryEntry
{
	u8 size;
	u8 sectorsInExtendedRecord;
	u32 firstDataSectorLE;	// LBA
	u32 firstDataSectorBE;
	u32 dataLengthLE;				// Size
	u32 dataLengthBE;
	u8 years;
	u8 month;
	u8 day;
	u8 hour;
	u8 minute;
	u8 second;
	u8 offsetFromGMT;
	u8 flags; // 2 = directory
	u8 fileUnitSize;
	u8 interleaveGap;
	u16 volSeqNumberLE;
	u16 volSeqNumberBE;
	u8 identifierLength; //identifier comes right after
	u8 firstIdChar;
};
struct DirectorySector
{
	DirectoryEntry entry;
	char space[2048-sizeof(DirectoryEntry)];
};

struct VolDescriptor
{
	char type;
	char cd001[6];
	char version;
	char sysid[32];
	char volid[32];
	char zeros[8];
	u32 numSectorsLE;
	u32 numSectoreBE;
	char morezeros[32];
	u16 volSetSizeLE;
	u16 volSetSizeBE;
	u16 volSeqNumLE;
	u16 volSeqNumBE;
	u16 sectorSizeLE;
	u16 sectorSizeBE;
	u32 pathTableLengthLE;
	u32 pathTableLengthBE;
	u16 firstLETableSectorLE;
	u16 firstLETableSectorBE;
	u16 secondLETableSectorLE;
	u16 secondLETableSectorBE;
	u16 firstBETableSectorLE;
	u16 firstBETableSectorBE;
	u16 secondBETableSectorLE;
	u16 secondBETableSectorBE;
	DirectoryEntry root;
	char volumeSetIdentifier[128];
	char publisherIdentifier[128];
	char dataPreparerIdentifier[128];
	char applicationIdentifier[128];
	char copyrightFileIdentifier[37];
	char abstractFileIdentifier[37];
	char bibliographicalFileIdentifier[37];
	char volCreationDateTime[17];
	char mreModDateTime[17];
	char volExpireDateTime[17];
	char volEffectiveDateTime[17];
	char one;
	char zero;
	char reserved[512];
	char zeroos[653];
};

#pragma pack(pop)