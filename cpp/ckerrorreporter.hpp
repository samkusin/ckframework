//
//  ckerrorreporter.hpp
//  Overview
//
//  Created by Samir Sinha on 8/17/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_ckerrorreporter_hpp
#define Overview_ckerrorreporter_hpp

#include "cinek/core/cktypes.hpp"

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
