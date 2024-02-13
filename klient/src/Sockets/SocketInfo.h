#ifndef SIECI_SOCKETINFO_H
#define SIECI_SOCKETINFO_H
//#include "winsockImport.h"
#include <algorithm>

class SocketInfo {
    static inline int socket;
    static inline std::vector<std::string> log;

public:


    static inline int& getSocket(){
        return socket;
    }

    static inline void setSocket(int mySocket){
        socket = mySocket;
    }

};

#endif //SIECI_SOCKETINFO_H
