/**
 *  @file   file.h
 *  @author Samir Sinha
 *  @brief  Contains APIs for file I/O
 *  @date   11/24/2015
 *  @copyright Cinekine, all rights reserved under The MIT License.
 */

#ifndef CINEK_IO_FILE_HPP
#define CINEK_IO_FILE_HPP

#include <stddef.h>
#include <stdint.h>

typedef struct ckio_handle ckio_handle;

typedef enum
{
    kCKIO_ReadFlag  = 0x01,
    kCKIO_WriteFlag = 0x02,
    kCKIO_ReadWrite = kCKIO_ReadFlag | kCKIO_WriteFlag,
    kCKIO_Async     = 0x04
}
ckio_access;

typedef enum
{
    kCKIO_Success   = 0,
    kCKIO_Pending   = 1,
    kCKIO_Canceled  = 2,
    kCKIO_EOF       = 3,
    kCKIO_Error     = -1
}
ckio_status;

typedef struct
{
    uintptr_t os_handle;
}
ckio_info;

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Opens a file for access based on the access flags.
 *
 *  @param  path    The path of the file resource
 *  @param  access  Bitflags defining file access.  To use the Async IO APIs,
 *                  applications must pass kCKIOAsync bitflag.
 *  @return A handle to the opened resource
 */
ckio_handle* ckio_open
(
    const char* path,
    int access
);
/**
 *  Closes a file opened via ckio_open.
 *
 *  @param  handle  The handle opened via ckio_open
 */
void ckio_close(ckio_handle* handle);
/**
 *  Cancels a pending request (for asynchronous I/O.)
 *
 *  @param  handle  The handle opened via ckio_open
 */
void ckio_cancel(ckio_handle* handle);
/**
 *  Reads contents of a file into the supplied buffer.
 *
 *  @param  handle  The IO file handle
 *  @param  buffer  The buffer to read contents into
 *  @param  size    The size of the input buffer
 *  @return Amount of bytes read.  For async file acces, 0 will always be
 *          returned.  To detect EOF, use ckio_get_status.
 */
size_t ckio_read
(
    ckio_handle* handle,
    uint8_t* buffer,
    size_t size
);
/**
 *  Query the status of an IO operation.  For synchronous IO, the return values
 *  are from the most recent operation.  For asynchronous IO, the return values
 *  are for the current operation.
 *
 *  @param  handle
 */
ckio_status ckio_get_status
(
    ckio_handle* handle,
    size_t* result
);
/**
 *  Returns the size and other information about the opened file.
 *
 *  @param  handle  The opened file handle
 *  @param  info    (Optional) Pointer to a structure receiving additional file
 *                  information. NULL if extra information is not needed.
 */
size_t ckio_get_info(ckio_handle* handle, ckio_info* info);

#ifdef __cplusplus
}
#endif


#endif /* asyncfile_hpp */
