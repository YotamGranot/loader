/**
 * @file common.h
 * @brief Common definitions and macros for the loader. 
 * @author Yotam Granot
 */

#ifndef __LOADER_COMMON_H__
#define __LOADER_COMMON_H__
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h> 
#include <stdio.h>
#include <unistd.h>
#include <string.h>


#define INVALID_FD (-1)
#define C_STANDARD_FAILURE_VALUE (-1)

#define CLOSE(fd) \
    do { \
        if ((fd) >= 0) { \
            close(fd); \
            (fd) = INVALID_FD; \
        } \
    } while (0)

#define FREE(ptr) \
    do { \
        if ((ptr) != NULL) { \
            free(ptr); \
            (ptr) = NULL; \
        } \
    } while (0)

#define HEAPALLOCZ(size) calloc(1, (size))

#ifdef _DEBUG
#define DEBUG_PRINT(fmt, ...) \
    do { \
        fprintf(stderr, "file %s, line %d, "fmt, __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define DEBUG_PRINT0(fmt) \
    do { \
        fprintf(stderr, "file %s, line %d, "fmt, __FILE__, __LINE__);\
    } while (0)

#else 
#define DEBUG_PRINT(fmt, ...) \
    do { \
        /* No operation in release mode */ \
    } while (0)
#define DEBUG_PRINT0(fmt) \
    do { \
        /* No operation in release mode */ \
    } while (0)

#endif /* _DEBUG */

#ifdef _DEBUG
#define ASSERT(condition) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", #condition, __FILE__, __LINE__); \
            abort(); \
        } \
    } while (0)
#else
#define ASSERT(condition) \
    do { \
        /* No operation in release mode */ \
    } while (0)
#endif /* _DEBUG */

#endif /* __LOADER_COMMON_H__ */