/**
 * @file    file.cpp
 * @author  Samir Sinha
 * @date    4/5/2015
 * @brief   A File Operations object
 * @copyright Copyright 2015 Samir Sinha.  All rights reserved.
 * @license ISC
 *          (http://www.isc.org/downloads/software-support-policy/isc-license/)
 */

#include "file.hpp"

#include <cstdio>

namespace cinek {

namespace file
{

typedef struct
{
    QueryResult lastOpResult;
    QueryExtra lastOpExtra;
}
StdFileContext;

static FileHandle StdIOFOpen(void* ctx_, const char* pathname, uint32_t access)
{
    StdFileContext* ctx = reinterpret_cast<StdFileContext*>(ctx_);
    
    char mode[2];
    if ((access & kReadAccess) != 0)
    {
        mode[0] = 'r';
    }
    else
    {
        return nullptr;
    }
    if ((access & kText) != 0)
    {
        mode[1] = 't';
    }
    else
    {
        mode[1] = 'b';
    }
    
    FILE* fp = fopen(pathname, mode);
    ctx->lastOpExtra.longResult = 0;
    return fp;
}

static size_t StdIOFRead(void* ctx_, FileHandle fh, uint8_t* buffer, size_t cnt)
{
    StdFileContext* ctx = reinterpret_cast<StdFileContext*>(ctx_);
    FILE* fp = reinterpret_cast<FILE*>(fh);
    size_t sz = fread(buffer, 1, cnt, fp);
    ctx->lastOpExtra.sizeResult = sz;
    return ctx->lastOpExtra.sizeResult;
}

static void StdIOFClose(void* ctx_, FileHandle fh)
{
    StdFileContext* ctx = reinterpret_cast<StdFileContext*>(ctx_);
    FILE* fp = reinterpret_cast<FILE*>(fh);
    fclose(fp);
    ctx->lastOpExtra.longResult = 0;
}

static bool StdIOFSeek(void* ctx_, FileHandle fh, Seek seekType, long offs)
{
    int whence = SEEK_SET;
    if (seekType == kSeekCur)
        whence = SEEK_CUR;
    else if (seekType == kSeekEnd)
        whence = SEEK_END;

    StdFileContext* ctx = reinterpret_cast<StdFileContext*>(ctx_);
    FILE* fp = reinterpret_cast<FILE*>(fh);
    ctx->lastOpExtra.longResult = 0;
    return fseek(fp, offs, whence)==0;
}

static size_t StdIOFSize(void* ctx_, FileHandle fh)
{
    FILE* fp = reinterpret_cast<FILE*>(fh);
    long pos = ftell(fp);
    if (pos < 0)
        return 0;
    fseek(fp, 0, SEEK_END);

    StdFileContext* ctx = reinterpret_cast<StdFileContext*>(ctx_);
    size_t sz = ftell(fp);
    ctx->lastOpExtra.sizeResult = sz;
    fseek(fp, pos, SEEK_SET);
    
    return sz;
}

static long StdIOFTell(void* ctx_, FileHandle fh)
{
    StdFileContext* ctx = reinterpret_cast<StdFileContext*>(ctx_);
    FILE* fp = reinterpret_cast<FILE*>(fh);

    ctx->lastOpExtra.longResult = ftell(fp);
    return ctx->lastOpExtra.longResult;
}

static bool StdIOEOF(void* ctx_, FileHandle fh)
{
    StdFileContext* ctx = reinterpret_cast<StdFileContext*>(ctx_);
    FILE* fp = reinterpret_cast<FILE*>(fh);
    ctx->lastOpExtra.longResult = 0;
    return feof(fp) != 0;
}

static QueryResult StdIOQuery(void* ctx_, FileHandle fh, QueryExtra* extra)
{
    StdFileContext* ctx = reinterpret_cast<StdFileContext*>(ctx_);
    FILE* fp = reinterpret_cast<FILE*>(fh);
    if (extra) {
         *extra = ctx->lastOpExtra;
    }
    return ferror(fp) != 0 ? kQuerySuccess : kQueryFailed;
}

static void StdIOCancel(void* , FileHandle )
{
}

Ops _CoreFileOps;
StdFileContext _StdFileContext;


void setOpsStdio()
{
    _CoreFileOps.context = &_StdFileContext;
    _CoreFileOps.openCb = &StdIOFOpen;
    _CoreFileOps.readCb = &StdIOFRead;
    _CoreFileOps.closeCb = &StdIOFClose;
    _CoreFileOps.seekCb = &StdIOFSeek;
    _CoreFileOps.tellCb = &StdIOFTell;
    _CoreFileOps.sizeCb = &StdIOFSize;
    _CoreFileOps.eofCb = &StdIOEOF;
    _CoreFileOps.queryCb = &StdIOQuery;
    _CoreFileOps.cancelCb = &StdIOCancel;
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

bool seek(FileHandle fh, Seek type, long offs)
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

QueryResult queryRequest(FileHandle fh, QueryExtra* extra)
{
    return _CoreFileOps.queryCb(_CoreFileOps.context, fh, extra);
}

void cancelRequest(FileHandle fh)
{
    return _CoreFileOps.cancelCb(_CoreFileOps.context, fh);
}

}

}   // namespace cinek
