
#include "ckfspath.h"


char* cinek_path_join(char *resultPath, size_t resultBufSize, const char *leftPath,
    const char* rightPath)
{
    char* const initialResultPath = resultPath;
    char* resultPathEnd = resultPath + resultBufSize - 1;

    /* left */
    while (resultPath != resultPathEnd)
    {
        if (*leftPath == '\0')
            break;
        *resultPath = *leftPath;
        ++resultPath;
        ++leftPath;
    }
    if (resultPath == resultPathEnd)
    {
        /* On error, return a null string. */
        *initialResultPath = '\0';
        return initialResultPath;
    }
    /* Enforce mid-path separator */
    if (*(resultPath-1) != '/')
    {
        *resultPath = '/';
        ++resultPath;
    }
    /* Prevent copying the right path separator (if it exists) as a result. */
    if (*rightPath == '/')
        ++rightPath;
    while (resultPath != resultPathEnd)
    {
        if (*rightPath == '\0')
            break;
        *resultPath = *rightPath;
        ++resultPath;
        ++rightPath;
    }
    if (resultPath == resultPathEnd)
    {
        /* On error, return a null string. */
        *initialResultPath = '\0';
        return initialResultPath;
    }

    *resultPath = '\0';
    return initialResultPath;
}