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

static FileHandle StdIOFOpen(void* , const char* pathname, uint32_t access)
{
    char mode[2];
    if ((access & FileOps::kReadAccess) != 0)
    {
        mode[0] = 'r';
    }
    else
    {
        return nullptr;
    }
    if ((access & FileOps::kText) != 0)
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

static bool StdIOFSeek(void* , FileHandle fh, FileOps::Seek seekType, long offs)
{
    int whence = SEEK_SET;
    if (seekType == FileOps::kSeekCur)
        whence = SEEK_CUR;
    else if (seekType == FileOps::kSeekEnd)
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

FileOps _CoreFileOps;

void setStdIOFileOps()
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


void setFileOps(const FileOps& ops)
{
    _CoreFileOps = ops;
}

const FileOps& getFileOps()
{
    return _CoreFileOps;
}


}   // namespace cinek
