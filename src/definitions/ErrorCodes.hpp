#ifndef ERRORCODES_H
#define ERRORCODES_H

enum ErrorCodes
{
	OK = 0,
	InvalidParameter = 1,
	InvalidState = 2,
	InvalidOperation = 3,
	InvalidFormat = 4,
	InvalidHandle = 5,
	InternalAssertionFailure = 6,
	IndexIsOutOfBounds = 7,
	// ...
};

#endif