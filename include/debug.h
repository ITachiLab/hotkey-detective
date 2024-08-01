/*!
 * \file    debug.h
 * \brief   Header file with debug utitlies.
 * \author  Itachi
 * \date    2024-07-12
 */

#ifndef DEBUG_H
#define DEBUG_H

#ifndef _DEBUG
#define debugPrint(fmt, ...)
#define allocateConsole()
#endif

#ifdef _DEBUG

/*!
 * \brief Otuput formatted message to the console.
 *
 * @param fmt format string (printf-like style)
 */
#define debugPrint(fmt, ...)    \
  do {                          \
    fprintf(stderr,             \
            "%s:%d:%s(): " fmt, \
            __FILE__,           \
            __LINE__,           \
            __func__,           \
            __VA_ARGS__);       \
  } while (0)

/*!
 * \brief Allocate console, should be called once during application start.
 */
#define allocateConsole()            \
  do {                               \
    AllocConsole();                  \
    freopen("CONIN$", "r", stdin);   \
    freopen("CONOUT$", "w", stdout); \
    freopen("CONOUT$", "w", stderr); \
  } while (0)
#endif

#endif  // DEBUG_H
