/**
 *  @file   file.c
 *  @author Samir Sinha
 *  @brief  Contains APIs for file I/O
 *  @date   11/24/2015
 *  @copyright Cinekine, all rights reserved under The MIT License.
 */

#include "file.h"

#include <Windows.h>

#define CKIO_HANDLE_FLAG_ASYNC              1
#define CKIO_HANDLE_FLAG_EOF                2
#define CKIO_HANDLE_FLAG_ERROR              4
#define CKIO_HANDLE_FLAG_CANCELED           8
#define CKIO_HANDLE_FLAG_ASYNC_ACTIVE       256

#define CKIO_HANDLE_STATUS_MASK     ( CKIO_HANDLE_FLAG_EOF + \
                                      CKIO_HANDLE_FLAG_ERROR + \
                                      CKIO_HANDLE_FLAG_CANCELED + \
                                      CKIO_HANDLE_FLAG_ASYNC_ACTIVE)

typedef struct ckio_handle
{
    HANDLE hFile;
    unsigned int flags;
    OVERLAPPED overlapped;
    DWORD lastresult;
}
ckio_handle;

static ckio_handle s_io_handles[8] = {
    { INVALID_HANDLE_VALUE, },
    { INVALID_HANDLE_VALUE, },
    { INVALID_HANDLE_VALUE, },
    { INVALID_HANDLE_VALUE, },
    { INVALID_HANDLE_VALUE, },
    { INVALID_HANDLE_VALUE, },
    { INVALID_HANDLE_VALUE, },
    { INVALID_HANDLE_VALUE, }
};

static const size_t s_num_handles = sizeof(s_io_handles) / sizeof(ckio_handle);

ckio_handle* ckio_open
(
    const char* path,
    int access
)
{
    ckio_handle* handle;
    DWORD dwFlags;
    DWORD dwShareMode;
    DWORD dwCreationDisp;
    DWORD dwAccess;
    int i;
    
    for (i = 0; i < s_num_handles; ++i) {
        if (s_io_handles[i].hFile == INVALID_HANDLE_VALUE) {
            break;
        }
    }
    if (i >= s_num_handles)
        return NULL;
    handle = &s_io_handles[i];

    dwFlags = FILE_ATTRIBUTE_NORMAL;
    dwCreationDisp = 0;
    dwShareMode = 0;        /* Exclusive access by default. */
    dwAccess = 0;

    if (access == kCKIO_Async) {
        dwFlags |= FILE_FLAG_OVERLAPPED;
    }

    if ((access & kCKIO_ReadWrite) == kCKIO_ReadWrite) {
        dwAccess = GENERIC_READ | GENERIC_WRITE;
        dwCreationDisp = OPEN_ALWAYS;
    }
    else if ((access & kCKIO_ReadFlag) != 0) {
        dwShareMode = FILE_SHARE_READ;
        dwAccess = GENERIC_READ;
        dwCreationDisp = OPEN_EXISTING;
    }
    else if ((access & kCKIO_WriteFlag) != 0) {
        dwAccess = GENERIC_WRITE;
        dwCreationDisp = CREATE_ALWAYS;
    }
    
    handle->hFile = CreateFile(path, dwAccess, dwShareMode, NULL, dwCreationDisp, dwFlags, NULL);
    if (handle->hFile == INVALID_HANDLE_VALUE)
        return NULL;

    handle->flags = 0;
    handle->lastresult = 0;
    
    if ((access & kCKIO_Async) != 0) {
        handle->flags |= CKIO_HANDLE_FLAG_ASYNC;
    }
    
    return handle;
}

void ckio_close(ckio_handle* handle)
{
    if (handle->hFile != INVALID_HANDLE_VALUE) {
        if ((handle->flags & CKIO_HANDLE_FLAG_ASYNC_ACTIVE)) {
            /* all IO are cancelled on close. */
            CancelIoEx(handle->hFile, NULL);
        }
        CloseHandle(handle->hFile);
        handle->hFile = INVALID_HANDLE_VALUE;
    }
}

void ckio_cancel(ckio_handle* handle)
{
    if (handle->hFile != INVALID_HANDLE_VALUE) {
        if ((handle->flags & CKIO_HANDLE_FLAG_ASYNC_ACTIVE)) {
            CancelIoEx(handle->hFile, &handle->overlapped);
            handle->flags &= ~CKIO_HANDLE_FLAG_ASYNC_ACTIVE;
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
    BOOL res;
    if (handle->hFile == INVALID_HANDLE_VALUE)
        return 0;
    
    /* clear all error flags */
    handle->flags &= ~(CKIO_HANDLE_STATUS_MASK);
    handle->lastresult = 0;
    
    if ((handle->flags & CKIO_HANDLE_FLAG_ASYNC) != 0) {
        memset(&handle->overlapped, 0, sizeof(handle->overlapped));
        res = ReadFile(handle->hFile, buffer, (DWORD)size, NULL, &handle->overlapped);
        if (!res) {
            if (GetLastError() != ERROR_IO_PENDING) {
                handle->flags |= CKIO_HANDLE_FLAG_ERROR;
                return 0;
            }
        }

        handle->flags |= CKIO_HANDLE_FLAG_ASYNC_ACTIVE;
    }
    else {
        res = ReadFile(handle->hFile, buffer, (DWORD)size, &handle->lastresult, NULL);
        if (!res) {
            handle->flags |= CKIO_HANDLE_FLAG_ERROR;
        }
        else if (!handle->lastresult) {
            handle->flags |= CKIO_HANDLE_FLAG_EOF;
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
    if (handle->hFile == INVALID_HANDLE_VALUE)
        return kCKIO_Error;
    
    if ((handle->flags & CKIO_HANDLE_FLAG_ERROR) != 0)
        return kCKIO_Error;
    else if ((handle->flags & CKIO_HANDLE_FLAG_EOF) != 0)
        return kCKIO_EOF;
    else if ((handle->flags & CKIO_HANDLE_FLAG_CANCELED) != 0)
        return kCKIO_Canceled;

    if ((handle->flags & CKIO_HANDLE_FLAG_ASYNC_ACTIVE) != 0) {
        /* asynchronous IO */
        BOOL res;
        res = HasOverlappedIoCompleted(&handle->overlapped);
        if (!res) {
            return kCKIO_Pending;
        }
        else {
            handle->flags &= ~CKIO_HANDLE_FLAG_ASYNC_ACTIVE;

            res = GetOverlappedResult(handle->hFile, &handle->overlapped, &handle->lastresult, FALSE);
            if (!res) {
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
    LARGE_INTEGER sz;

    if (handle == INVALID_HANDLE_VALUE)
        return 0;

    if (!GetFileSizeEx(handle->hFile, &sz))
        return 0;

    if (info) {
        info->os_handle = (uintptr_t)handle->hFile;
    }

#if defined(_WIN64)
    return sz.u.LowPart;        /* TODO: not right! - perhaps we return a 64-bit value always?*/
#else
    return sz.QuadPart;
#endif
}