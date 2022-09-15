/**_
 * BLUE FIRE CAPITAL, LLC CONFIDENTIAL
 * _____________________
 * 
 *  Copyright © [2007] - [2012] Blue Fire Capital, LLC
 *  All Rights Reserved.
 * 
 * NOTICE:  All information contained herein is, and remains the property
 * of Blue Fire Capital, LLC and its suppliers, if any.  The intellectual
 * and technical concepts contained herein are proprietary to Blue Fire
 * Capital, LLC and its suppliers and may be covered by U.S. and Foreign
 * Patents, patents in process, and are protected by trade secret or copyright
 * law.  Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained from 
 * Blue Fire Capital, LLC.
_**/

#include "BufRange.hpp"

namespace nebula { namespace data {

std::ostream& operator<<(std::ostream &o, const ConstBufRange &aBuf) {
    if (aBuf.valid()) {
        o << "ConstBufRange[";
        return o.write(aBuf.buf, aBuf.size()) << ']';
    } else {
        return o << "ConstBufRange[invalid, buf: " << static_cast<const void*>(aBuf.buf)
                 << ", bufEnd: " << static_cast<const void*>(aBuf.bufEnd) << ']';
    }
}

std::ostream& operator<<(std::ostream &o, const BufRange &aBuf) {
    if (aBuf.valid()) {
        o << "BufRange[";
        return o.write(aBuf.buf, aBuf.size()) << ']';
    } else {
        return o << "BufRange[invalid, buf: " << static_cast<void*>(aBuf.buf)
                 << ", bufEnd: " << static_cast<const void*>(aBuf.bufEnd) << ']';
    }
}

}}
