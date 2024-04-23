#ifndef _BASE64_H_
#define _BASE64_H_

#include <vector>
#include "..\definitions\IntTypes.hpp"

namespace Base64
{
	const char *encode(const std::vector<unsigned char>& buf);
	const char *encode(const unsigned char* buf, unsigned int bufLen);
	UInt32 decode(char* encoded_string, size_t encoded_size, unsigned char*& ret_ptr);
};

#endif