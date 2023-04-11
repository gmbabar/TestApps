#include "FixAuthInfo.hpp"

#include <SQLAPI.h>
#include <nebula/db/DbHelper.hpp>
#include <nebula/gw/Safe.hpp>

// Note: does not populate sender and target ID's
FixAuthInfo fixAuthInfoFromDb(
        const std::string &aSessionName,
        const std::string &aDbCreds,
        const std::string &aStoredProc,
        const std::string &aSenderID,
        const std::string &aTargetID,
        const std::string &aTargetSubID) {
    std::cout << __func__ << ": MOCK FixAuth DB Fucntion... Please add needed values" << std::endl;
    return FixAuthInfo("","","","","");
}
