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

#include "cinek/types.hpp"

namespace cinek {

typedef void* FileHandle;

namespace file
{
    struct Ops
    {
        void* context;

        enum
        {
            kReadAccess     = 1,
            kText           = 32
        };
        enum Seek
        {
            kSeekSet        = 0,
            kSeekCur        = 1,
            kSeekEnd        = 2
        };

        FileHandle (*openCb)(void* context, const char* pathname, uint32_t access);
        size_t (*readCb)(void* context, FileHandle fh, uint8_t* buffer, size_t cnt);
        void (*closeCb)(void* context, FileHandle fh);
        size_t (*sizeCb)(void* context, FileHandle fh);
        bool (*seekCb)(void* context, FileHandle fh, Seek seekType, long offs);
        long (*tellCb)(void* context, FileHandle fh);
        bool (*eofCb)(void* context, FileHandle fh);
    };

    FileHandle  open(const char* pathname, uint32_t access=Ops::kReadAccess);
    size_t      size(FileHandle fh);
    size_t      read(FileHandle fh, uint8_t* buffer, size_t cnt);
    bool        seek(FileHandle fh, Ops::Seek type, long offs);
    long        tell(FileHandle fh);
    bool        eof(FileHandle fh);
    void        close(FileHandle fh);

    void setOpsStdio();
    void setOpsCustom(const Ops& ops);
    const Ops& getOps();
};

}   // namespace cinek

#endif