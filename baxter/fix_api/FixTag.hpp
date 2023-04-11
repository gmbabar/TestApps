#ifndef FIXTAG_HPP
#define FIXTAG_HPP


// Defined fix tags
enum FixTag {
    AvgPx = 6,
    BeginSeqNo = 7,
    BeginString = 8,
    BodyLength = 9,
    CheckSum = 10,
    ClOrdID = 11,
    CumQty = 14,
    EndSeqNo = 16,
    ExecID = 17,
    ExecInst = 18,
    ExecTransType = 20,
    HandlInst = 21,
    LastPx = 31,
    LastShares = 32,
    MsgSeqNum = 34,
    MsgType = 35,
    NewSeqNo = 36,
    OrderID = 37,
    OrderQty = 38,
    OrdStatus = 39,
    OrdType = 40,
    OrigClOrdID = 41,
    PossDupFlag = 43,
    Price = 44,
    RefSeqNumber = 45,
    SenderCompID = 49,
    SendingTime = 52,
    Side = 54,
    Symbol = 55,
    TargetCompID = 56,
    TargetSubID = 57,
    Text = 58,
    TimeInForce = 59,
    TransactTime = 60,
    ExecBroker = 76,
    EncryptMethod = 98,
    StopPrice = 99,
    ExDestination = 100,
    CxlRejReason = 102,
    HeartBtInt = 108,
    MinQty = 110,
    TestReqID = 112,
    LocateReqd = 114,
    OrigSendingTime = 122,
    GapFillFlag = 123,
    ResetSeqNumFlag = 141,
    NoRelatedSym = 146,             // MDRq
    ExecType = 150,
    LeavesQty = 151,
    MDReqID = 262,                  // MDRq : Market Data Request
    SubscriptionRequestType = 263,  // MDRq
    MarketDepth = 264,              // MDRq
    NoMDEntryTypes = 267,           // MDRq
    NoMDEntries = 268,      // MDIR : Market Data Incremental Refresh
    MDEntryType = 269,      // MDIR & MDRq
    MDEntryPx = 270,        // MDIR
    MDEntrySize = 271,      // MDIR
    MDEntryID = 278,        // MDIR
    MDUpdateAction = 279,   // MDIR
    SecurityReqID = 320,            // Security List Request
    SecurityResponseID = 322,       // Security List Response
    RefMsgType = 372,
    BusinessRejectRefId = 379,
    BusinessRejectReason = 380,
    CxlRejResponseTo = 434,
    Username = 553,
    Password = 554,
    SecurityListRequestType = 559,  // Security List Request
    SecurityRequestResult,          // Security List Response
    NoFills = 1362,     // Number of fills for this order
    FillExecID = 1363,  // Fill ID same as ExecID = 17
 	FillPx	= 1364,     // Fill price
	FillQty	= 1365,     // Fill quantity
	FillTradeID = 1366, // Fill trade ID. This will be shared by the fill corresponding to the other side of the trade.
	FillTime = 1367,    // Fill time
	FillLiquidityInd = 1443, // for maker, 2 for taker
    CancelAllOnDisconnect = 7001,
    Liquidity = 8001,
    PostOnly = 9004,
    ShortSaleAffirm = 9009,
    LongSaleAffirm = 9010,
    AllowRouting = 9011,
    CancelPairs = 9021,
    RegularSessionOnly = 9066,
    ShortSaleAffirmLongQuantity = 9067,
	// FeeRate	= 20100, // Fees paid on the fill, in percent
	// Fee = 20101, //Fees paid on the fill, in USD
    MaxTagNum
};

enum ExecTypeOrdStatusEnum : char {
    New = '0',
    PartialFill = '1',
    Fill = '2',
    DoneForDay = '3',
    Canceled = '4',
    Replace = '5',
    PendingCancel = '6',
    PendingReplace = 'E',
    Rejected = '8'
};

#endif /* FIXTAG_HPP */
