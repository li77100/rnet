#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <stdarg.h>

// inline void log(const char* buf, ...) {
// 	va_list vv;
// 	va_start(vv, buf);
// 	printf(buf, vv);
// 	va_end(vv);
// }
#define log printf

#endif