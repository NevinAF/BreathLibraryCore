#ifndef INTTYPES_HPP
#define INTTYPES_HPP

#ifndef SInt16_defined
#   define SInt16_defined
typedef signed short SInt16;
#endif

#ifndef UInt16_defined
#   define UInt16_defined
typedef unsigned short UInt16;
#endif

#ifndef UInt8_defined
#   define UInt8_defined
typedef unsigned char UInt8;
#endif

#ifndef SInt8_defined
#   define SInt8_defined
typedef signed char SInt8;
#endif

#if PLATFORM_ARCH_64
#   if PLATFORM_LINUX
#       ifndef SInt32_defined
#           define SInt32_defined
typedef signed int SInt32;
#       endif
#       ifndef UInt32_defined
#           define UInt32_defined
typedef unsigned int UInt32;
#       endif
#       ifndef UInt64_defined
#           define UInt64_defined
typedef unsigned long UInt64;
#       endif
#       ifndef SInt64_defined
#           define SInt64_defined
typedef signed long SInt64;
#       endif
#   elif PLATFORM_OSX
#       ifndef SInt32_defined
#           define SInt32_defined
typedef signed int SInt32;
#       endif
#       ifndef UInt32_defined
#           define UInt32_defined
typedef unsigned int UInt32;
#       endif
#       ifndef UInt64_defined
#           define UInt64_defined
typedef unsigned long long UInt64;
#       endif
#       ifndef SInt64_defined
#           define SInt64_defined
typedef signed long long SInt64;
#       endif
#   elif PLATFORM_WIN
#       ifndef SInt32_defined
#           define SInt32_defined
typedef signed long SInt32;
#       endif
#       ifndef UInt32_defined
#           define UInt32_defined
typedef unsigned long UInt32;
#       endif
#       ifndef UInt64_defined
#           define UInt64_defined
typedef unsigned long long UInt64;
#       endif
#       ifndef SInt64_defined
#           define SInt64_defined
typedef signed long long SInt64;
#       endif
#   else // Android, ...
#       ifndef SInt32_defined
#           define SInt32_defined
typedef signed int SInt32;
#       endif
#       ifndef UInt32_defined
#           define UInt32_defined
typedef unsigned int UInt32;
#       endif
#       ifndef UInt64_defined
#           define UInt64_defined
typedef unsigned long long UInt64;
#       endif
#       ifndef SInt64_defined
#           define SInt64_defined
typedef signed long long SInt64;
#       endif
#   endif
#else
#       ifndef SInt32_defined
#           define SInt32_defined
typedef signed int SInt32;
#       endif
#       ifndef UInt32_defined
#           define UInt32_defined
typedef unsigned int UInt32;
#       endif
#       ifndef UInt64_defined
#           define UInt64_defined
typedef unsigned long long UInt64;
#       endif
#       ifndef SInt64_defined
#           define SInt64_defined
typedef signed long long SInt64;
#       endif
#endif

#endif // INTTYPES_HPP