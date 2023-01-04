#include <iostream>

inline bool getNextJsonArrayValue(
        const char *&cptr,
        const char *cend,
        //nebula::data::ConstBufRange &token,
        std::string &token,
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
        //token = nebula::data::ConstBufRange(begin, cptr-begin);
        token = std::string(begin, cptr-begin);
        ++cptr;
        return true;
    }

    return false;
}

int main () {
    std::string msg = R"([32388,"te",[1260593688,1669829042871,0.38673629,7.3654]])";
    std::cout << msg << std::endl;
    const char *cptr = msg.c_str();
    const char * const cend = cptr + msg.size();
    bool hasMoreValues = false;
    std::string token;
    while (getNextJsonArrayValue(cptr, cend, token, hasMoreValues)) {
	std::cout << "token: " << token << " - " << hasMoreValues << std::endl;
	//hasMoreValues = false;
    }
    std::cout << "Done." << std::endl;
}


