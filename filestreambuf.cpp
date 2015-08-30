/**
 * @file    filestreambuf.hpp
 * @author  Samir Sinha
 * @date    8/18/2013
 * @brief   A custom streambuf implementation for the common filesystem
 * @copyright Copyright 2014 Samir Sinha.  All rights reserved.
 * @license ISC
 *          (http://www.isc.org/downloads/software-support-policy/isc-license/)
 */

#include "filestreambuf.hpp"


namespace cinek {
    //  Initializes the stream buffer with the contents of the file specified.
    //  Uses SDL to read from/write to files
    //  This implementation supports either read or write on an opened file, but not both.
    //  Future versions may support both methods concurrently.
    //  This makes handling overflow and underflow relatively easy since in either mode
    //      we don't need to take into account handling opposite operations.
    //
    //  NOTE : No output supported at this time.
    //
    FileStreamBuf::FileStreamBuf(const char* pathname, std::ios_base::openmode mode,
                         size_t bufferSize,
                         const Allocator& allocator) :
        _allocator(allocator),
        _fileHandle(nullptr),
        _mode(mode),
        _bufferSize(bufferSize),
        _totalSize(0),
        _buffer( (char*)_allocator.alloc(sizeof(char_type) * _bufferSize) )
    {
        uint32_t fileAccess = 0;
        if (_mode & std::ios_base::in)
        {
            if (_mode & std::ios_base::out)         // does not support both read and write for now.
                return;

            fileAccess = file::Ops::kReadAccess;
        }
        /*else if (_mode & std::ios_base::out)
        {
            if (_mode & std::ios_base::in)          // does not support both read and write for now.
                return;

            if (_mode & std::ios_base::app)
            {
                if (_mode & std::ios_base::trunc)   // failure condition
                    return;

                fmode[0] = 'a';
            }
            else
            {
                fmode[0] = 'w';
            }
        }
        */
        else
        {
            return;                 // unsupported IO
        }
        if (!(_mode & std::ios_base::binary))
        {
            fileAccess |= file::Ops::kText;
        }
        _fileHandle = file::open(pathname, fileAccess);
        if (_fileHandle)
        {
            _totalSize = file::size(_fileHandle);
        }
    }

    FileStreamBuf::~FileStreamBuf()
    {
        sync();
        if (_fileHandle)
        {
            file::close(_fileHandle);
            _fileHandle = nullptr;
        }
        if (_buffer)
        {
            _allocator.free(_buffer);
            _buffer = nullptr;
        }
    }

    size_t FileStreamBuf::availableChars() const
    {
        if (!_fileHandle)
            return 0;
        size_t remaining = _totalSize;
        remaining -= file::tell(_fileHandle);
        return remaining;
    }

    //  TODO - support substituting an external buffer?  Also support unbuffered IO (destroying the buffer.)
    std::streambuf* FileStreamBuf::setbuf(char* s, std::streamsize n)
    {
        return std::streambuf::setbuf(s, n);
    }

    //  Called when the FileStreamBuf is stuffed, needing to write out data to the output device.
    int FileStreamBuf::overflow(int c)
    {
        if (!(_mode & std::ios_base::out))
            return EOF;

        return EOF;
    }

    //  Called when the FileStreamBuf is starved, needing to read in more data from the device.
    //  May also be called to get the current input character without updating our get buffer pointers
    //  (for peek perhaps?)  As such, if we still have get-data at gptr(), return the data at gptr().
    int FileStreamBuf::underflow()
    {
        if (!(_mode & std::ios_base::in))
            return EOF;
        if (!_fileHandle)
            return EOF;

        //  TODO: write out any data (via overflow) if there's data in the put buffer (when we support
        //  concurrent read/write buffers.)

        if (gptr() == egptr())
        {
            size_t putbackLen = 0;
            if (!gptr())
            {
                setg(_buffer, _buffer+_bufferSize, _buffer+_bufferSize);
            }
            else
            {
                //  save the last few characters off so they are at the tail end of our new get buffer
                putbackLen = std::min<size_t>((egptr()-gptr())/2, 32);
            }
            memmove(eback(), egptr() - putbackLen, putbackLen*sizeof(char_type));
            size_t readSize = (egptr() - eback() - putbackLen)*sizeof(char_type);
            auto readBuf = (eback() + putbackLen);
            size_t readCount = file::read(_fileHandle,
                                    (uint8_t*)readBuf,
                                    readSize
                                );
            if (readCount == 0)
                return EOF;

            setg(eback(), readBuf, readBuf + readCount/sizeof(char_type));
        }
        return (int)((unsigned char)*gptr());
    }

    //  TBD - this is an abbrieviated version of the clang filebuf impl.
    int FileStreamBuf::pbackfail(int c)
    {
        if (_fileHandle && eback() < gptr())
        {
            if (c == EOF)
            {
                gbump(-1);
                return 0;
            }
            if (c == gptr()[-1])
            {
                gbump(-1);
                return c;
            }
        }
        return EOF;
    }

    //  if there's buffered output, this acts as a flush. (not implemented)
    //
    //  for buffered input, since we're implementing a non-seekable stream, we can't backup the input
    //  file buffer.  in this case we return an error indicating a loss of input data.
    int FileStreamBuf::sync()
    {
        if (_fileHandle)
        {
            size_t revertCount = egptr()-gptr();
            
            if (!file::seek(_fileHandle, file::Ops::kSeekCur, -revertCount))
                return -1;

            setg(nullptr, nullptr, nullptr);
        }
        return 0;
    }

}   // namespace cinek
