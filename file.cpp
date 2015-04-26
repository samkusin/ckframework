/**
 * @file    file.cpp
 * @author  Samir Sinha
 * @date    4/5/2015
 * @brief   A File Operations object
 * @copyright Copyright 2015 Samir Sinha.  All rights reserved.
 * @license ISC
 *          (http://www.isc.org/downloads/software-support-policy/isc-license/)
 */

#include "cinek/file.hpp"

#include <cstdio>

namespace cinek {

namespace file
{
static FileHandle StdIOFOpen(void* , const char* pathname, uint32_t access)
{
    char mode[2];
    if ((access & Ops::kReadAccess) != 0)
    {
        mode[0] = 'r';
    }
    else
    {
        return nullptr;
    }
    if ((access & Ops::kText) != 0)
    {
        mode[1] = 't';
    }
    else
    {
        mode[1] = 'b';
    }
    return reinterpret_cast<FileHandle>(fopen(pathname, mode));
}

static size_t StdIOFRead(void* , FileHandle fh, uint8_t* buffer, size_t cnt)
{
    return fread(buffer, 1, cnt, reinterpret_cast<FILE*>(fh));
}

static void StdIOFClose(void* , FileHandle fh)
{
    fclose(reinterpret_cast<FILE*>(fh));
}

static bool StdIOFSeek(void* , FileHandle fh, Ops::Seek seekType, long offs)
{
    int whence = SEEK_SET;
    if (seekType == Ops::kSeekCur)
        whence = SEEK_CUR;
    else if (seekType == Ops::kSeekEnd)
        whence = SEEK_END;

    return fseek(reinterpret_cast<FILE*>(fh), offs, whence)==0;
}

static size_t StdIOFSize(void* , FileHandle fh)
{
    FILE* fp = reinterpret_cast<FILE*>(fh);
    long pos = ftell(fp);
    if (pos < 0)
        return 0;
    fseek(fp, 0, SEEK_END);

    size_t sz = ftell(fp);
    fseek(fp, pos, SEEK_SET);
    return sz;
}

static long StdIOFTell(void* context, FileHandle fh)
{
    return ftell(reinterpret_cast<FILE*>(fh));
}

static bool StdIOEOF(void* context, FileHandle fh)
{
    return feof(reinterpret_cast<FILE*>(fh)) != 0;
}

Ops _CoreFileOps;

void setOpsStdio()
{
    _CoreFileOps.context = nullptr;
    _CoreFileOps.openCb = &StdIOFOpen;
    _CoreFileOps.readCb = &StdIOFRead;
    _CoreFileOps.closeCb = &StdIOFClose;
    _CoreFileOps.seekCb = &StdIOFSeek;
    _CoreFileOps.tellCb = &StdIOFTell;
    _CoreFileOps.sizeCb = &StdIOFSize;
    _CoreFileOps.eofCb = &StdIOEOF;
}


void setOps(const Ops& ops)
{
    _CoreFileOps = ops;
}

const Ops& getOps()
{
    return _CoreFileOps;
}

FileHandle open(const char* pathname, uint32_t access)
{
    return _CoreFileOps.openCb(_CoreFileOps.context, pathname, access);
}

size_t size(FileHandle fh)
{
    return _CoreFileOps.sizeCb(_CoreFileOps.context, fh);
}

size_t read(FileHandle fh, uint8_t* buffer, size_t cnt)
{
    return _CoreFileOps.readCb(_CoreFileOps.context, fh, buffer, cnt);
}

bool seek(FileHandle fh, Ops::Seek type, long offs)
{
    return _CoreFileOps.seekCb(_CoreFileOps.context, fh, type, offs);
}

long tell(FileHandle fh)
{
    return _CoreFileOps.tellCb(_CoreFileOps.context, fh);
}

bool eof(FileHandle fh)
{
    return _CoreFileOps.eofCb(_CoreFileOps.context, fh);
}

void close(FileHandle fh)
{
    _CoreFileOps.closeCb(_CoreFileOps.context, fh);
}

}

}   // namespace cinek
