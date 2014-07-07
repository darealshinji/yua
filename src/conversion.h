#ifndef __Conversion_H__
#define __Conversion_H__

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;
typedef int bool;

#define false (0)
#define true (1)

#ifdef _MSC_VER
#define strncasecmp _strnicmp
#endif

#endif
