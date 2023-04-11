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

#ifndef FIXMSGTYPE_HPP
#define FIXMSGTYPE_HPP

#include <nebula/common.hpp>

enum FixMsgType : char {
    Heartbeat = '0',
    TestRequest = '1',
    ResendRequest = '2',
    Reject = '3',
    SequenceReset = '4',
    Logout = '5',
    ExecutionReport = '8',
    OrderCancelReject = '9',        // also called OrderCancelReport in docs
    Logon = 'A',
    NewOrder = 'D',
    CancelRequest = 'F',
    CancelReplace = 'G',
    BusinessMessageReject = 'j',
    BulkCancel = 's',
    MarketDataRequest = 'V',
    MarketDataSnapshot = 'W',
    SecurityListRequest = 'x',      // Instrument loader
    MarketDataRefresh = 'X',        // MD Incremental Refresh
    SecurityListResponse = 'y',     // Instrument loader
    MarketDataRequestReject = 'Y'
};

#endif /* FIXMSGTYPE_HPP */
