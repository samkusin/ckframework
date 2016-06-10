
#include "messenger.hpp"

namespace ckmsg {
    const uint8_t MessengerBase::kEncodedMessageHeader[4] = { 'm','e','s','g' };

    void MessengerBase::encodeHeader(uint8_t* target, const uint8_t hdr[])
    {
        target[0] = hdr[0];
        target[1] = hdr[1];
        target[2] = hdr[2];
        target[3] = hdr[3];
    }

    bool MessengerBase::checkHeader(const uint8_t* input, const uint8_t hdr[])
    {
        return input[0]==hdr[0] &&
               input[1]==hdr[1] &&
               input[2]==hdr[2] &&
               input[3]==hdr[3];
    }
}
