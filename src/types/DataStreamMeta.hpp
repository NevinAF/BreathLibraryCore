#ifndef DATASTREAMMETA_H
#define DATASTREAMMETA_H

#include "..\definitions\IntTypes.hpp"

struct DataStreamMeta
{
public:
	UInt32 putIndex;
	UInt32 capacity;

	unsigned char* data;
};

#endif