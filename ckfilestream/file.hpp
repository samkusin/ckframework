/**
 * @file    file.hpp
 * @author  Samir Sinha
 * @date    4/5/2015
 * @brief   A file operations interface
 * @copyright Copyright 2015 Samir Sinha.  All rights reserved.
 * @license ISC
 *          (http://www.isc.org/downloads/software-support-policy/isc-license/)
 */

#ifndef CINEK_FILE_HPP
#define CINEK_FILE_HPP

#include <cstdint>
#include <cstddef>

namespace cinek {

typedef struct
{
    uintptr_t data;
}
AsyncFileHandle;

typedef void* FileHandle;

namespace file
{
    enum
    {
        kReadAccess     = 0x00000001,
        kText           = 0x00000020,
        kAsync          = 0x80000000
    };

    typedef enum
    {
        kSeekSet        = 0,
        kSeekCur        = 1,
        kSeekEnd        = 2
    }
    Seek;

    typedef enum
    {
        kQuerySuccess,
        kQueryPending,
        kQueryFailed
    }
    QueryResult;

    typedef union
    {
        size_t sizeResult;
        long longResult;
    }
    QueryExtra;

    typedef struct
    {
        void* context;

        FileHandle (*openCb)(void* context, const char* pathname, uint32_t access);
        size_t (*readCb)(void* context, FileHandle fh, uint8_t* buffer, size_t cnt);
        void (*closeCb)(void* context, FileHandle fh);
        size_t (*sizeCb)(void* context, FileHandle fh);
        bool (*seekCb)(void* context, FileHandle fh, Seek seekType, long offs);
        long (*tellCb)(void* context, FileHandle fh);
        bool (*eofCb)(void* context, FileHandle fh);
        QueryResult (*queryCb)(void* context, FileHandle fh, QueryExtra* extra);
        void (*cancelCb)(void* context, FileHandle fh);
    } Ops;

    FileHandle  open(const char* pathname, uint32_t access);
    size_t      size(FileHandle fh);
    size_t      read(FileHandle fh, uint8_t* buffer, size_t cnt);
    bool        seek(FileHandle fh, Seek type, long offs);
    long        tell(FileHandle fh);
    bool        eof(FileHandle fh);
    void        close(FileHandle fh);
    QueryResult queryRequest(FileHandle fh, QueryExtra* extra);
    void        cancelRequest(FileHandle fh);

    void setOpsStdio();
    void setOpsCustom(const Ops& ops);
    const Ops& getOps();
};

}   // namespace cinek

#endif