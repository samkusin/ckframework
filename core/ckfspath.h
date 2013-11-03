
#ifndef CINEKINE_PATH_H
#define CINEKINE_PATH_H

#include "cktypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Joins two strings to form a platform-agnostic path.
 *  @param resultPath   [out] The result C string buffer.
 *  @param resultBufSize The result C string buffer's size.
 *  @param leftPath     The left path string of the final path. 
 *  @param rightPath    The right path string of the final path.
 */
void cinek_path_join(char* resultPath, size_t resultBufSize, const char* leftPath,
    const char* rightPath);

#ifdef __cplusplus
}
#endif

#define CK_IS_NULL_PATH(_pathcstr_) (*(_pathcstr_)=='\0')

#endif
