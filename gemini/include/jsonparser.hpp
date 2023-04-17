#include <iostream>
#include "stringview.hpp"


inline bool getNextJsonArrayValue(
            const char *&cptr,
            const char *cend,
            ConstBufRange &token,
            bool &hasMoreValues) {
        // Check if more values at same level.
        auto HasMoreValues = [](const char *cptr, const char *cend) {
            if(*cptr == ',')
                ++cptr;
            bool more = false, done = false;
            while (cptr <= cend && !done) {
                switch(*cptr) {
                case ',':
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

        const char *begin = cptr++;
        while(cptr <= cend && *cptr !=',' && *cptr !='[' && *cptr !=']')
            ++cptr;

        if (cptr <= cend) {
            hasMoreValues = HasMoreValues(cptr, cend);
            token = ConstBufRange(begin, cptr-begin);
            ++cptr;
            return true;
        }

        return false;
    }

    //inline
    bool isJsonArray(const std::string &aMsg) {
        size_t len = aMsg.length();
        char ch;
        size_t idx = 0;
        while (idx < len) {
            ch = aMsg[idx++];
            if (::isspace(ch)) continue;
            if (ch == '[') return true;      // should be first char
            break;
        }
        return false;
    }

    //inline
    bool isJsonArray(const ConstBufRange &aBuf) {
        size_t len = aBuf.size();
        char ch;
        size_t idx = 0;
        while (idx < len) {
            ch = aBuf[idx++];
            if (::isspace(ch)) continue;
            if (ch == '[') return true;      // should be first char
            break;
        }
        return false;
    }

    //inline
    bool isJsonObject(const ConstBufRange &aBuf) {
        size_t len = aBuf.size();
        char ch;
        size_t idx = 0;
        while (idx < len) {
            ch = aBuf[idx++];
            if (::isspace(ch)) continue;
            if (ch == '{') return true;      // should be first char
            break;
        }
        return false;
    }

    // inline
    bool getJsonKey(
            const ConstBufRange &aBuf,
            const std::string& aKey,
            size_t &aBegin,
            size_t &aEnd) {
        auto pos = aBuf.find(aKey, (aBegin > 0 && aBegin < aBuf.size() ? aBegin : 0));
        if (pos == std::string::npos)
            return false;
        aBegin = pos;
        aEnd = pos + aKey.size();
        return true;
    }

    // inline
    bool getJsonStrVal(
            const ConstBufRange &aBuf,
            const std::string& aKey,
            size_t &aBegin,
            size_t &aEnd) {
        size_t pos = aBuf.find(aKey, (aBegin > 0 && aBegin < aBuf.size() ? aBegin : 0));
        if (pos == std::string::npos) 
            return false;
        pos += aKey.size();
        pos = aBuf.find(':', pos);
        if (pos == std::string::npos) 
            return false;
        aBegin = aBuf.find('"', pos)+1;
        if (aBegin == std::string::npos) 
            return false;
        aEnd = aBuf.find('"', aBegin);
        if (aEnd == std::string::npos) 
            return false;
        return true;
    }

    // inline
    bool getJsonNumVal(
            const ConstBufRange &aBuf,
            const std::string& aKey,
            size_t &aBegin,
            size_t &aEnd) {
        size_t pos = aBuf.find(aKey, (aBegin > 0 && aBegin < aBuf.size() ? aBegin : 0));
        if (pos == std::string::npos) 
            return false;
        pos += aKey.size();
        pos = aBuf.find(':', pos);
        if (pos == std::string::npos) 
            return false;
        aBegin = pos+1;
        aEnd = pos;
        size_t len = aBuf.size();
        while(++aEnd < len && (isdigit(aBuf[aEnd]) || aBuf[aEnd] == '.'));
        return aEnd > aBegin;
    }

    // inline
    bool getNextJsonStrVal(
            const ConstBufRange &aBuf,
            size_t &aBegin,
            size_t &aEnd) {
        aBegin = aBuf.find('"', aBegin)+1;
        if (aBegin == std::string::npos) 
            return false;
        aEnd = aBuf.find('"', aBegin);
        if (aEnd == std::string::npos) 
            return false;
        return true;
    }

    // inline
    bool getNextJsonArray(
            const ConstBufRange &aBuf,
            size_t &aBegin,
            size_t &aEnd) {
        aBegin = aBuf.find('[', aBegin);
        if (aBegin == std::string::npos) {
            std::cout << "Failed to find '['" << std::endl;
            return false;
        }
        int openCount = 0;
        const char *pbegin = &aBuf[aBegin];
        const char *cptr = pbegin - 1;
        const char *pend = &aBuf[aEnd];
        while (++cptr < pend && *cptr != 0) {
            switch(*cptr) {
            case '[':
                ++openCount;
                break;
            case ']':
                if (--openCount == 0) {
                    aEnd = aBegin + (cptr - pbegin + 1);
                    return true;
                }
                break;
            default:
                break;
            }
        }
        return false;
    }