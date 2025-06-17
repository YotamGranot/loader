/**
 * @file loader_status.h
 * @brief Header file for loader status management.
 * @author Yotam Granot
 * @date 2025-06-16
 */

#ifndef __LOADER_STATUS_H__
#define __LOADER_STATUS_H__
/* *** Enums ********************** */
enum loader_status
{
    LOADER_STATUS_UNINITIALIZED = -1,
    LOADER_STATUS_OK = 0,
    LOADER_STATUS_INVALID_PARAMETER,
    LOADER_STATUS_ERROR,
    LOADER_STATUS_NOT_FOUND,
    LOADER_STATUS_INVALID_FORMAT,
    LOADER_STATUS_UNSUPPORTED,
    LOADER_STATUS_TIMEOUT,
    LOADER_STATUS_BUSY
};
/* *** Macros ********************** */
#define LOADER_STATUS_IS_OK(status) ((status) == LOADER_STATUS_OK)
#define LOADER_STATUS_IS_ERROR(status) (!LOADER_STATUS_IS_OK(status))

#endif /* __LOADER_STATUS_H__ */
