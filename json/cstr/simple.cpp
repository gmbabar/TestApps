#include<iostream>
#include<string>
#include<cstring>

using namespace std;

inline bool IsJsonArray(const std::string &aMsg) {
    size_t len = aMsg.length();
    char ch; 
    int idx = -1; 
    while (++idx < len) {
        ch = aMsg[idx];
        if (::isspace(ch)) continue;
        if (ch == '[') return true;
        break;
    }   
    return false;
}

inline bool IsJsonKeyValue(const std::string &aMsg) {
    size_t len = aMsg.length();
    char ch; 
    int idx = -1; 
    while (++idx < len) {
        ch = aMsg[idx];
        if (::isspace(ch)) continue;
        if (ch == '{') return true;
        break;
    }   
    return false;
}

inline bool GetNextArrayValue(const char *&cptr, const char *&token, bool &hasMoreValues, std::string &aMsg) {
    // Check if more values at same level.
    auto HasMoreValues = [](const char *cptr) {
        if(*cptr == ',')
            ++cptr;
        bool more = false, done = false;
        while (*cptr && !done) {
            switch(*cptr) {
            case',':
                more = true;
                done = true;
                break;
            case '[':
            case ']':
                done = true;
                break;
            default:
                if(!isspace(*cptr)) {
                    more = true;
                    done = true;
                }
                break;
            }
            ++cptr;
        }
        return more;
    };

    hasMoreValues = false;
    //skip delims
    while (*cptr !=0 && (*cptr==',' || *cptr==' ' || *cptr=='[' || *cptr==']')) 
        ++cptr;
    if (*cptr == 0)
        return false;

    token = cptr++;
    while(*cptr !=0 && *cptr !=',' && *cptr !='[' && *cptr !=']')
        ++cptr;

    if (*cptr != 0) {
        hasMoreValues = HasMoreValues(cptr);
        aMsg[ cptr - aMsg.c_str() ] = 0;
        ++cptr;
        return true;
    }

    return false;
}

inline bool ProcessBookUpdate(const char *&cptr, std::string &aMsg) {
    // book:
    //  SNAPSHOT: [17082,[[7254.7,3,3.3], ..., [7354.7,2,-2.4]]]
    //  UPDATE: [17082,[7254.7,3,3.3]]
    bool hasMoreValues = false;
    const char *token = nullptr;
    const char *price = nullptr, *amount = nullptr, *volume = nullptr;
    const int FieldCount = 3;
    int idx = 0;
    while (GetNextArrayValue(cptr, token, hasMoreValues, aMsg)) {
        switch(idx % FieldCount) {
        case 2:
            volume = token;
            std::cout << "Volume(Qty): " << token << ", hasMore: " << hasMoreValues << std::endl;
            break; 
        case 1:
            amount = token;
            std::cout << "Amount(nOrders): " << token << ", hasMore: " << hasMoreValues << std::endl;
            break; 
        case 0:
            price = token;
            std::cout << "Price: " << token << ", hasMore: " << hasMoreValues << std::endl;
            break; 
        default:
            std::cout << "Unexpected case" << std::endl;
            break;
        }
        if (++idx % FieldCount == 0) {
            if (hasMoreValues) return false;  
            std::cout << "Processed Book Update: price" << price << ", amount: " << amount << ", volume: " << volume << std::endl;
        }
    }
    return (idx != 0 && idx % FieldCount == 0 ? true : false); 
}

inline bool ProcessTradeUpdate(const char *&cptr, std::string &aMsg) {
    bool hasMoreValues = false;
    const char *token = nullptr;
    const int FieldCount = 4;
    int idx = 0;
    while (GetNextArrayValue(cptr, token, hasMoreValues, aMsg)) {
        switch(idx % FieldCount) {
        case 3:
            std::cout << idx << ": Price: " << token << ", hasMore: " << hasMoreValues << std::endl;
            break; 
        case 2:
            std::cout << idx << ": Volume: " << token << ", hasMore: " << hasMoreValues << std::endl;
            break; 
        default:
            std::cout << idx << ": Token: " << token << ", hasMore: " << hasMoreValues << std::endl;
            break;
        }
        ++idx;
    }
    return (idx != 0 && idx % FieldCount == 0 ? true : false); 
}

inline bool ProcessFeedData(std::string &aMsg) {
    cout << "MSG: " << aMsg << endl;
    const char *cptr = aMsg.c_str();
    bool hasMoreValues = false;
    const char *channelId = nullptr;

    if ( !GetNextArrayValue(cptr, channelId, hasMoreValues, aMsg) ) 
       return false;
    std::cout << "Channel Id: " << channelId << std::endl;

    if (!strcmp( channelId, "12345") ) {	// Trade Message
        if (hasMoreValues) {  // Trade Update
            const char *updateType = nullptr;
            if (!GetNextArrayValue(cptr, updateType, hasMoreValues, aMsg)) 
                return false;
            cout << "UpdateType: " << updateType << endl;
            if ( strcmp(updateType, "\"te\"") )
                return false;
        }
        return ProcessTradeUpdate(cptr, aMsg);
    } else {
        return ProcessBookUpdate(cptr, aMsg);
    }
    return false;
}

int main() {
    // Trade Update
    string str = "[12345, \"te\", [89441, 1631148456164, 0.018, 7254.7]";
    ProcessFeedData(str);
    std::cout << std::endl;

    // Trade Snapshot
    str = "[12345, [[89441,1631148456164,0.018,7254.7], [9441,1631148456164,0.019,7254.8]]";
    ProcessFeedData(str);
    std::cout << std::endl;

    // BOOK:
    str = "[17082,[[7254.7,3,3.3], [7354.7,2,-2.4]]]";
    ProcessFeedData(str);
    std::cout << std::endl;
     
    str = "[17082,[7254.7,3,3.3]]";
    ProcessFeedData(str);
    std::cout << std::endl;
}

 

