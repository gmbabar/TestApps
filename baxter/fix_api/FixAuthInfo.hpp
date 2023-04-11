#ifndef _FIX_FIXAUTHINFO_HPP
#define _FIX_FIXAUTHINFO_HPP

#include <string>

// Load auth info
struct FixAuthInfo {
    FixAuthInfo(
            const std::string &aUsername,
            const std::string &aPassword,
            const std::string &aSenderId,
            const std::string &aTargetId,
            const std::string &aTargetSubId)
        : username(aUsername),
        password(aPassword),
        senderCompId(aSenderId),
        targetCompId(aTargetId),
        targetSubId(aTargetSubId) {
    }
    ~FixAuthInfo() {
        volatile const char zero(0);
        for(auto it = username.begin(); it != username.end(); ++it)
            *it = zero;
        for(auto it = password.begin(); it != password.end(); ++it)
            *it = zero;
    }
    std::string username;
    std::string password;
    std::string senderCompId;
    std::string targetCompId;
    std::string targetSubId;
};

// Find auto info from DB
FixAuthInfo fixAuthInfoFromDb(
        const std::string &aSessionName,
        const std::string &aDbCreds,
        const std::string &aStoredProc,
        const std::string &aSenderID,
        const std::string &aTargetID,
        const std::string &aTargetSubID);

#endif /* _FIX_FIXAUTHINFO_HPP */
