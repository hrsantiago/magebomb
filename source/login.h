#ifndef __LOGIN_H__
#define __LOGIN_H__

#include <string>
#include "NetworkMessage.h"
#include "Position.h"
#include "ConstTibia.h"

class Login 
{
    public:
        Login(struct sockaddr_in _sin, unsigned short _threadId);
        
        void LoginProtocol(); // Used for tibia global login (skiped when otserv login)
        void GameProtocol();
        
    private:
        
        unsigned char threadId;
        struct sockaddr_in sin;
        
        NetworkMessage *netMsg;
        
        SOCKET sock;
        
        bool sendPing();
        bool follow(unsigned long targetId);
        bool attackTarget(unsigned long targetId);
        bool fightMode();
        bool say(std::string text, std::string receiver);
        bool setOutfit(unsigned char headColor, unsigned char bodyColor, unsigned char legsColor, unsigned char feetColor);
        bool useHotkey(unsigned long itemId, unsigned long targetId);
        bool cancelMove();
        bool move(Direction dir);
        
};

#endif
