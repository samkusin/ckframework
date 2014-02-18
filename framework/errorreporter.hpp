/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Cinekine Media
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE. 
 * 
 * @file    cinek/framework/errorreporter.hpp
 * @author  Samir Sinha
 * @date    8/17/2013
 * @brief   Object allocation within a pooled heap 
 * @copyright Cinekine
 */

#ifndef CK_ERRORREPORTER_HPP
#define CK_ERRORREPORTER_HPP

#include "cinek/core/cktypes.h"

#include <functional>

namespace cinekine {

    /** Error types. */
    enum ErrorType
    {
        Error_None,               /**< No error. */
        Error_Generic,            /**< A generic (uncategorized) error. */
        Error_IO,                 /**< An IO error. */
        
        Error_Count               /**< Number of possible SystemErrors. */
    };
    
    enum
    {
        Error_InitFail            /**< An initialization action failed. */
    };


    class ErrorReporter
    {
    public:
        ErrorHandler() = default;
        ~ErrorHandler() = default;

        /**
         *  @param  category     A combination of one or more error flags.
         *  @return True if specified error category 
         */
        bool checkError(ErrorType errType) const { return (errType != _errType); }
        /**
         *  @return True if an object's operation resulted in an error.
         */
        bool hasError() const { return _category!=0; }
        /**
         *  @return True if an object's operation resulted in an IO error.
         */
        bool hasIOError() const { return checkError(Error_IO); }
        /**
         *  An error handler functor.
         */
        typedef std::functional<void(ErrorType, int32_t)> ErrorHandler;
        /**
         *  Specify an error handler for the specified type.
         *  @param errType      On the specified error, execute the supplied handler.
         *  @param fn           The handler executed if an operation results in an error.
         */
        void onError(const ErrorHandler& fn);

    protected:
        bool reportError(ErrorType errType, int32_t code);

    private:
        ErrorType _errType;
        int32_t _code;
        ErrorHandler _handler;
    };
    
    inline void ErrorReporter::onError(const ErrorHandler& fn)
    {
        _handler = fn;
    }
    
    inline bool ErrorReporter::reportError(ErrorType errType, int32_t code)
    {
        _errType = errType;
        _code = code;
        _handlers(errType, code);
    }
    
}   // namespace cinekine

#endif
