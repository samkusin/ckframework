/**
 *  @file   file.c
 *  @author Samir Sinha
 *  @brief  Contains APIs for file I/O
 *  @date   11/24/2015
 *  @copyright Cinekine, all rights reserved under The MIT License.
 */

#include "file.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <aio.h>

#define CKIO_HANDLE_FLAG_ASYNC      1
#define CKIO_HANDLE_FLAG_EOF        2
#define CKIO_HANDLE_FLAG_ERROR      4
#define CKIO_HANDLE_FLAG_CANCELED   8
#define CKIO_HANDLE_STATUS_MASK     ( CKIO_HANDLE_FLAG_EOF + \
                                      CKIO_HANDLE_FLAG_ERROR + \
                                      CKIO_HANDLE_FLAG_CANCELED )

typedef struct ckio_handle
{
    int fd;
    unsigned int flags;
    struct aiocb aio;
    ssize_t lastresult;
}
ckio_handle;

static ckio_handle s_io_handles[8] = {
    { -1, },
    { -1, },
    { -1, },
    { -1, },
    { -1, },
    { -1, },
    { -1, },
    { -1, }
};

static const size_t s_num_handles = sizeof(s_io_handles) / sizeof(ckio_handle);

ckio_handle* ckio_open
(
    const char* path,
    int access
)
{
    ckio_handle* handle;
    int ioaccess;
    int i;
    
    for (i = 0; i < s_num_handles; ++i) {
        if (s_io_handles[i].fd < 0) {
            break;
        }
    }
    if (i >= s_num_handles)
        return NULL;
    handle = &s_io_handles[i];
    ioaccess = 0;
    if ((access & kCKIO_ReadWrite) == kCKIO_ReadWrite) {
        ioaccess = O_RDONLY;
    }
    else if ((access & kCKIO_ReadFlag) != 0) {
        ioaccess = O_RDONLY;
    }
    else if ((access & kCKIO_WriteFlag) != 0) {
        ioaccess = O_WRONLY | O_CREAT;
    }
    
    handle->fd = open(path, ioaccess);
    if (handle->fd < 0)
        return NULL;
    
    handle->lastresult = 0;
    handle->flags = 0;
    handle->aio.aio_fildes = -1;
    
    if ((access & kCKIO_Async) != 0) {
        handle->flags |= CKIO_HANDLE_FLAG_ASYNC;
    }
    
    return handle;
}

void ckio_close(ckio_handle* handle)
{
    if (handle && handle->fd >= 0) {
        close(handle->fd);
        handle->fd = -1;
    }
}

void ckio_cancel(ckio_handle* handle)
{
    if (handle && handle->fd >= 0) {
        if ((handle->flags & CKIO_HANDLE_FLAG_ASYNC)!=0) {
            aio_cancel(handle->fd, &handle->aio);
            handle->flags |= CKIO_HANDLE_FLAG_CANCELED;
        }
    }
}

size_t ckio_read
(
    ckio_handle* handle,
    uint8_t* buffer,
    size_t size
)
{
    if (!handle || handle->fd < 0)
        return 0;
    
    /* clear all error flags */
    handle->flags &= ~(CKIO_HANDLE_STATUS_MASK);
    
    if ((handle->flags & CKIO_HANDLE_FLAG_ASYNC) != 0) {
        memset(&handle->aio, 0, sizeof(handle->aio));
        handle->aio.aio_fildes = handle->fd;
        handle->aio.aio_nbytes = size;
        handle->aio.aio_offset = 0;
        handle->aio.aio_buf = buffer;
        handle->lastresult = 0;
        
        if (aio_read(&handle->aio) < 0) {
            handle->flags |= CKIO_HANDLE_FLAG_ERROR;
            handle->aio.aio_fildes = -1;
        }
    }
    else {
        handle->lastresult = read(handle->fd, buffer, size);
        if (!handle->lastresult) {
            handle->flags |= CKIO_HANDLE_FLAG_EOF;
        }
        else if (handle->lastresult < 0) {
            handle->flags |= CKIO_HANDLE_FLAG_ERROR;
        }
    }
    return handle->lastresult;
}

ckio_status ckio_get_status
(
    ckio_handle* handle,
    size_t* result
)
{
    if (!handle || handle->fd < 0)
        return kCKIO_Error;
    
    if ((handle->flags & CKIO_HANDLE_FLAG_ERROR) != 0)
        return kCKIO_Error;
    else if ((handle->flags & CKIO_HANDLE_FLAG_EOF) != 0)
        return kCKIO_EOF;
    else if ((handle->flags & CKIO_HANDLE_FLAG_CANCELED) != 0)
        return kCKIO_Canceled;
    
    if (handle->aio.aio_fildes >= 0) {
        /* asynchronous IO */
        int res;
        res = aio_error(&handle->aio);
        if (res == EINPROGRESS) {
            return kCKIO_Pending;
        }
        else {
            handle->lastresult = aio_return(&handle->aio);
            handle->aio.aio_fildes = -1;
        
            if (res == ECANCELED) {
                handle->flags |= CKIO_HANDLE_FLAG_CANCELED;
                return kCKIO_Canceled;
            }
            else if (res != 0) {
                handle->flags |= CKIO_HANDLE_FLAG_ERROR;
                return kCKIO_Error;
            }
        }
    }    
    if (result) {
        *result = handle->lastresult;
    }

    return kCKIO_Success;
}

size_t ckio_get_info(ckio_handle* handle, ckio_info* info)
{
    struct stat status;
    int res;
    
    res = fstat(handle->fd, &status);
    if (res < 0)
        return 0;
    
    if (info) {
        info->os_handle = handle->fd;
    }
    
    return status.st_size;
}