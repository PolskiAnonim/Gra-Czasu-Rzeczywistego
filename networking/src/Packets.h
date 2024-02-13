#pragma once
#include <vector>
#include <string>
#include <array>
#include <cstring> // NIE USUWAĆ
#include <cassert>
#include <cstdint>

// Use (void) to silence unused warnings.
#define assertm(exp, msg) assert(((void)msg, exp))
/*
#ifdef _WIN32
//#include <winsockImport.h>
#include "../../klient/src/Player/Projectile.h"
*/
//using Socket = int;
//#else
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/sctp.h>
using Socket = int;
//#endif

#include <iostream>
#include <optional>

enum NetMessageType {
    MESSAGE=0,
    NETCREATEPLAYER,
    NETUPDATEPLAYER,
    NETDELETEPLAYER,
    NETQUEUEPLAYER,
    NONE
    //OTHERS IF THERE ARE
};

#pragma pack(push, 1)
struct NetRectangle {
	float x;
	float y;
    NetRectangle() = default;
    NetRectangle(float tx, float ty): x(tx), y(ty) {}

    NetRectangle& operator=(const NetRectangle& other) = default;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct NetColor {
	int r;
    int g;
    int b;
    int a;

    NetColor(int ir,int ig,int ib,int ia) : r(ir), g(ig), b(ib), a(ia) {}
    NetColor() = default;
    NetColor& operator=(const NetColor& other) = default;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct NetSpeed {
    float currentSpeedX;
	float currentSpeedY;
	float acceleration;
	float maxSpeed;

    NetSpeed() = default;

    NetSpeed(float speedX, float speedY, float accel, float maxSpeed)
            : currentSpeedX(speedX), currentSpeedY(speedY), acceleration(accel), maxSpeed(maxSpeed) {}

};
#pragma pack(pop)

#pragma pack(push, 1)
struct NetMoveAble : public NetRectangle  {
	NetColor color;
	NetSpeed speed;
	float size;

    NetMoveAble() = default;
    NetMoveAble(float s, NetColor col, NetSpeed spe, NetRectangle nr): size(s), color(col), speed(spe), NetRectangle(nr) {};
    NetMoveAble& operator=(const NetMoveAble& other) = default;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct NetProjectile : public NetRectangle {
	float halfLife = 2; // Sekundy +- 1/4 Niby
	double spawnTime;
    NetSpeed speed;

    NetProjectile(){
        this->x = -100;
        this->y = -100;
    }

	NetProjectile(NetSpeed speed, double spawnTime, float x, float y) {
		this->speed = speed;
		this->spawnTime = spawnTime;
		this->x = x;
		this->y = y;
	}

};
#pragma pack(pop)

#pragma pack(push, 1)
struct NetPlayerUpdatePacket : public NetMoveAble {
	uint8_t playerId;
    std::array<std::optional<NetProjectile>,16> projectilki;
	float dzialoRotation;


	NetPlayerUpdatePacket(NetSpeed speed, float x, float y, float dzialoRotation, std::array<std::optional<NetProjectile>, 16> projectilki) {
		this->speed = speed;
		this->x = x;
		this->y = y;
		this->dzialoRotation = dzialoRotation;
		this->projectilki = projectilki;
	}

    NetPlayerUpdatePacket() = default;
    NetPlayerUpdatePacket& operator=(const NetPlayerUpdatePacket& other) = default;

    static inline std::string getHeader(){
        return "NUP";
    }
};
#pragma pack(pop)


#pragma pack(push, 1)
struct NetMessage {
    std::array<char,3> message;
    explicit NetMessage(std::string myMessage) : message({}) {
        memset(message.data(),0,3);
        memcpy(message.data(),myMessage.data(),myMessage.size());
    };
    NetMessage() = default;
    NetMessage& operator=(const NetMessage& other) = default;
    static inline std::string getHeader(){
        return "MSG";
    }

};
#pragma pack(pop)


#pragma pack(push, 1)
class NetQueue {
public:
    NetColor color;
    std::array<char,16> name;
    static inline std::string getHeader(){
        return "QUE";
    }
    NetQueue(
            NetColor color,
            std::string myName
    ) : color(color), name({}) {
        memcpy(name.data(),myName.data(),myName.size());
    }

    NetQueue() = default;
    NetQueue& operator=(const NetQueue& other) = default;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct NetCreatePlayerPacket:NetMoveAble {
	int playerId;
    float shootCd;
	std::array<char,16> playerName;

    NetCreatePlayerPacket()= default;

    NetCreatePlayerPacket(int id, float cd, std::string pn, NetMoveAble ma) :
        playerId(id), shootCd(cd), playerName({}), NetMoveAble(ma) {
        memcpy(playerName.data(),pn.data(),pn.size());
    };
    NetCreatePlayerPacket& operator=(const NetCreatePlayerPacket& other) = default;

    static inline std::string getHeader(){
        return "NCP";
    }
};
#pragma pack(pop)


//pakiety SERVER->CLIENT:
//0x00 - NetCreatePlayerPacket
//0x01 - NetPlayerUpdatePacket
//0x02 - NetPlayerRemove

//pakiety CLIENT->SERVER:
//0x00 - NetCreatePlayerPacket (playerId=0, i tak to nie bedzie uzywane przez serwer):
// - przy odbiorze przez serwer na byc rozglaszane do innych graczy, i zeby jeszcze serwer jakies id mu przypisywal 
// - te id ma byc jakos przypisywane do polczenia tcp do uzycia w pakiecie 0x01
//0x01 - NetSelfUpdatePacket
// - rowniez rozglaszane!!!
template <typename T>
std::string getHeader(T& obj) {
    return obj.getHeader();
}

template <typename T>
std::string getHeader(std::vector<T>& vec) {
    return vec[0].getHeader();
}

template <typename T>
std::string getHeader(std::vector<std::reference_wrapper<T>>& vec){
    return vec[0].get().getHeader();
}


template<typename T, typename V=T>
__attribute__((optimize("O0")))
int sendData(Socket socket, V& data, int flag){
    const char eof = '#';

    std::string header = getHeader(data);

    if constexpr(std::is_same_v<V, std::vector<std::reference_wrapper<T>>> || std::is_same_v<V, std::vector<T>>) {
        char* msg = static_cast<char*>(malloc((sizeof(T) + sizeof(uint16_t)) * data.size() + header.size() + 1 + 1));
        char* startOfMsg = msg;
        // Skopiowanie nagłówka do wiadomości
        memcpy(msg,header.data(),header.size());
        msg+=header.size();
        // Dodanie ilości danych w vectorze
        memset(msg++,data.size(),1);
        for(auto& row:data){
            uint16_t rozmiar{static_cast<uint16_t>(sizeof(T))};
            memcpy(msg,&rozmiar, sizeof(rozmiar));
            msg+=sizeof(rozmiar);
            if constexpr(std::is_same_v<V, std::vector<std::reference_wrapper<T>>>)
                memcpy(msg,&(row.get()),sizeof(T));
            if constexpr (std::is_same_v<V, std::vector<T>>)
                memcpy(msg,&row,sizeof(T));
            msg+=sizeof(T);
        }
        // Dodanie znaku końca wiadomości
        memset(msg,'#',1);
        int snd = send(socket, startOfMsg,(sizeof(T) + sizeof(uint16_t))*data.size() + header.size()  + 1+ 1,flag);
        //int snd = sctp_sendmsg(socket, startOfMsg, (sizeof(T) + sizeof(uint16_t)) * data.size() + header.size() + 1 + 1, NULL,0,0,0,0,0,0);
        free(startOfMsg);
        return snd;
    }
    if constexpr (std::is_same_v<V,T>){
        const char eiv = '\0'; //elementsInVector zawsze 0 - wysyłamy jednostkę nie wektor

        // Tymczasowy playground do poprawnej konwersji rozmiaru wiadomości
        uint16_t rozmiar{static_cast<uint16_t>(sizeof(data))};
        // Blueprint wiadomości                              // + 1 - liczba vektorów ; + 1 znak końcowy
        char* msg = static_cast<char*>(malloc(sizeof(data) + sizeof(rozmiar) + header.size() + 2));
        // Skopiowanie nagłówka do wiadomości
        memcpy(msg,header.data(),header.size());
        // Dodanie nagłówka liczby sktruktur w wiadomości - tutaj zawsze 0
        memset(msg + header.size(),eiv,sizeof(eiv));
        // Skopiowanie rozmiaru
        memcpy(msg + header.size() + sizeof(eiv), &rozmiar, sizeof(rozmiar));
        // Skopiowanie danych
        memcpy(msg + header.size() + sizeof(rozmiar) + sizeof(eiv),(void*)&data,sizeof(data));
        // Dodanie znaku koncowego
        memset(msg + header.size() + sizeof(rozmiar) + sizeof(eiv) + sizeof(data),eof,1);
        int snd = send(socket, msg, sizeof(data) + sizeof(rozmiar) + header.size()  + sizeof(eof) + sizeof(eiv), flag);

        free(msg);
        return snd;
    }
}

template<typename T, typename V=T>
__attribute__((optimize("O0")))
int getData(Socket socket, V& data){
    if constexpr(std::is_same_v<V, std::vector<T>> or std::is_same_v<V, std::vector<std::reference_wrapper<T>>>) {
    char numberOfStructures;
    recv(socket, &numberOfStructures, sizeof(numberOfStructures), 0);
    for(int i=0;i!=static_cast<int>(numberOfStructures);i++){
        // Odczytanie orzmiaru struktury
        uint16_t size;
        memset(&size, 0, sizeof(uint16_t));
        recv(socket, &size, sizeof(size), 0);
        //int len = *reinterpret_cast<int*>(size);

        // Odbierz strukturę
        char* buffer = static_cast<char *>(malloc(size));
        memset(buffer,0,size);
        int got = recv(socket, buffer, size, 0);
        if(got != size){
            return -1;
        }
        data.push_back(*reinterpret_cast<T*>(buffer));
        free(buffer);
    }
    char endSign;
    recv(socket, &endSign, 1, 0);
    if(endSign != '#'){
        // Odczytanie wiadomości nie zakończyło się znakiem końcowym - znaczy odczytaliśmy wiadomość w połowie
        std::cout << "ASSERT" << std::endl;
        assertm(true,"Wiadomość została odczytana w połowie endSign != '#'");
        return 0;
    }
    }
    else{
        // Odbierz rozmiar struktury
            char numberOfStructures;
        recv(socket, &numberOfStructures, sizeof(numberOfStructures), 0);

        uint16_t  size;
        memset(&size, 0, sizeof(size));
        recv(socket, &size, sizeof(size), 0);


        // Odbierz strukturę
        char* buffer = new char[size];
        memset(buffer,0,size);
        if(recv(socket, buffer, size, 0) != sizeof(data)){
            return -1;
        }

        char endSign;
        recv(socket, &endSign, 1, 0);
        if(endSign != '#'){
            std::cout << "ASSERT" << std::endl;
            // Odczytanie wiadomości nie zakończyło się znakiem końcowym - znaczy odczytaliśmy wiadomość w połowie
            assertm(true,"Wiadomość została odczytana w połowie endSign != '#'");
            return 0;
        }
        data = *reinterpret_cast<T*>(buffer);
        delete[] buffer;

        return 1;
    }

}