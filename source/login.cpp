#include <windows.h>
#include <vector>
#include <string>
#include <time.h>

#include <boost/thread.hpp>
#include <boost/tokenizer.hpp>

#include "Login.h"
#include "NetworkMessage.h"
#include "Data.h"
#include "Config.h"
#include "ConstTibia.h"

extern unsigned short attackDelay, exhausted, tibiaVersion;
extern bool otserv;
extern boost::recursive_mutex loginMutex;
extern Target *target;
extern Leader *leader;
extern std::string host;

extern std::vector<Character*> charList;

Login::Login(struct sockaddr_in _sin, unsigned short _threadId)
{
    sin = _sin;
    threadId = _threadId;
}

bool Login::sendPing()
{
    netMsg->Reset();
    netMsg->AddByte(0x1E);
    
    if(!netMsg->WriteToSocket(sock))
        return false;
    else
        return true;
}

bool Login::move(Direction dir)
{
    netMsg->Reset();
    switch(dir)
    {
        case NORTH:
            netMsg->AddByte(0x65);
            break;
        case EAST:
            netMsg->AddByte(0x66);
            break;
        case SOUTH:
            netMsg->AddByte(0x67);
            break;
        case WEST:
            netMsg->AddByte(0x68);
            break;
        case NORTHEAST:
            netMsg->AddByte(0x6A);
            break;
        case SOUTHEAST:
            netMsg->AddByte(0x6B);
            break;
        case SOUTHWEST:
            netMsg->AddByte(0x6C);
            break;
        case NORTHWEST:
            netMsg->AddByte(0x6D);
            break;
    }
    
    if(!netMsg->WriteToSocket(sock))
        return false;
    else
        return true;
}

bool Login::cancelMove()
{
    netMsg->Reset();
	netMsg->AddByte(0xBE);
    
    if(!netMsg->WriteToSocket(sock))
        return false;
    else
        return true;
}

bool Login::useHotkey(unsigned long itemId, unsigned long targetId)
{
    netMsg->Reset();
	netMsg->AddByte(0x84);
	
    Position pos(0xFFFF,0x00,0x00);
    netMsg->AddPosition(pos);
    netMsg->AddU16(itemId);
    netMsg->AddByte(0x00);
    
    if(targetId == 0)
        return true;
    else
        netMsg->AddU32(targetId);
	
    if(!netMsg->WriteToSocket(sock))
        return false;
    else
        return true;
}

bool Login::setOutfit(unsigned char headColor, unsigned char bodyColor, unsigned char legsColor, unsigned char feetColor)
{            
    netMsg->Reset();
	netMsg->AddByte(0xD3);
	
	boost::recursive_mutex::scoped_lock lock(loginMutex);
	for(std::vector<Character*>::iterator it = charList.begin(), end = charList.end(); it != end; it++)
	{
        if((*it)->threadId == threadId)
        {
	        netMsg->AddU16((*it)->outfitId);
	        break;
        }
    }
	
    netMsg->AddByte(headColor);
	netMsg->AddByte(bodyColor);
	netMsg->AddByte(legsColor);
	netMsg->AddByte(feetColor);
	lock.unlock();
	
	netMsg->AddByte(0x00);
	
    if(!netMsg->WriteToSocket(sock))
        return false;
    else
        return true;
}

bool Login::say(std::string text, std::string receiver)
{
    netMsg->Reset();
	netMsg->AddByte(0x96);
	
	if(receiver.length() == 0)
	    netMsg->AddByte(0x01);
	else
	{
        netMsg->AddByte(0x04);
        netMsg->AddString(receiver);
    }
    
	netMsg->AddString(text);
	
	if(!netMsg->WriteToSocket(sock))
        return false;
    else
        return true;
}

bool Login::fightMode()
{
    netMsg->Reset();
	netMsg->AddByte(0xA0);
	netMsg->AddByte(0x01);
	netMsg->AddByte(0x01);
	netMsg->AddByte(0x00);
    if(!netMsg->WriteToSocket(sock))
        return false;
    else
        return true;
}

bool Login::attackTarget(unsigned long targetId)
{    
    netMsg->Reset();
	netMsg->AddByte(0xA1);
	
    if(targetId == 0)
        return true;
    else
        netMsg->AddU32(targetId);
    
    if(!netMsg->WriteToSocket(sock))
        return false;
    else
        return true;
}

bool Login::follow(unsigned long targetId)
{    
    netMsg->Reset();
	netMsg->AddByte(0xA2);
	
    if(targetId == 0)
        return true;
    else
        netMsg->AddU32(targetId);
    
    if(!netMsg->WriteToSocket(sock))
        return false;
    else
        return true;
}

void Login::LoginProtocol()
{
    std::string password, playerName;
    unsigned long account;
    
    for(std::vector<Character*>::iterator it = charList.begin(), end = charList.end(); it != end; it++)
    {
        if((*it)->threadId == threadId)
        {
            account = (*it)->account;
            password = (*it)->password;
            playerName = (*it)->playerName;
            break;
        }
    }
    
    bool success = false;
    while(!success)
    {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if(sock == INVALID_SOCKET)
        {
            std::cout << "<*> Invalid Socket. (LoginProtocol)" << std::endl;
            continue;
        }
        if(connect(sock, (struct sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR)
        {
            std::cout << "<*> Could not connect. (LoginProtocol)" << std::endl;
            system("pause");
            closesocket(sock);
            continue;
        }
        
        netMsg = new NetworkMessage();
        
        netMsg->AddByte(0x01);
        netMsg->AddU16(0x02);
        netMsg->AddU16(tibiaVersion);
        netMsg->AddU32(DAT_SIGNATURE);
        netMsg->AddU32(SPR_SIGNATURE);
        netMsg->AddU32(PIC_SIGNATURE);
        
        unsigned long key[4];
	    key[0] = rand() % 0xFFFFFFFF;
	    key[1] = rand() % 0xFFFFFFFF;
	    key[2] = rand() % 0xFFFFFFFF;
	    key[3] = rand() % 0xFFFFFFFF;

        netMsg->AddByte(0x00);

        netMsg->AddU32(key[0]);
        netMsg->AddU32(key[1]);
        netMsg->AddU32(key[2]);
        netMsg->AddU32(key[3]);
        
        netMsg->AddU32(account);
        netMsg->AddString(password);

        for(int x = 23 + password.length(); x < 128; x++)
            netMsg->AddByte(0x00);
        
        if(otserv)
            netMsg->RSA_encrypt(RSAKEY_OTS);
        else
            netMsg->RSA_encrypt(RSAKEY_CIP);
        
        if(!netMsg->WriteToSocket(sock))
        {
            std::cout << "<*> Error while writing to socket. (LoginProtocol)" << std::endl;
            closesocket(sock);
            if(netMsg)
                delete netMsg;
            continue;
        }
        
        netMsg->setEncryptionKey(key);
	    netMsg->setEncryptionState(true);
	
	    if(netMsg->ReadFromSocket(sock))
	    {
            while(!(netMsg->end()))
            {
                unsigned char opt = netMsg->GetByte();
                switch(opt)
                {
                    case 0x0A: //Error message
                    {
                        printf("<*> Error: %s (LoginProtocol)\n",netMsg->GetString().c_str());
                        break;
                    }
                    case 0x14: //MOTD message
                    {
                        netMsg->GetString();
                        break;
                    }

                    case 0x64: //Character list
                    {
                        std::string name,world;
                        unsigned long worldIp;
                        unsigned short worldPort;
                        int numChars = netMsg->GetByte();
                        for(int i = 0; i < numChars; i++)
                        {
                            name = netMsg->GetString();
		                    world = netMsg->GetString();
		                    worldIp = netMsg->GetU32();
		                    worldPort = netMsg->GetU16();
		                    if(name == playerName)
		                    {
                                sin.sin_addr.s_addr = worldIp;
                                sin.sin_port = htons(worldPort);
                            }
                        }
                        netMsg->GetU16(); // Premium
                        
                        break;
                    }
                }
            }
        }
        closesocket(sock);
        if(netMsg)
            delete netMsg;
            
        success = true;
    }
    
    GameProtocol();
}

void Login::GameProtocol()
{
    // Player variables
    std::string playerName, playerRune, password;
    unsigned long playerId, account;
    unsigned short outfitId;
    unsigned char playerHp, playerHeal;
    bool utamoVita, meleeAttack, followTarget, wearOutfit;
    
    for(std::vector<Character*>::iterator it = charList.begin(), end = charList.end(); it != end; it++)
    {
        if((*it)->threadId == threadId)
        {
            playerName = (*it)->playerName;
            playerRune = (*it)->playerRune;
            password = (*it)->password;
            playerId = (*it)->playerId;
            account = (*it)->account;
            outfitId = (*it)->outfitId;
            playerHp = (*it)->playerHp;
            playerHeal = (*it)->playerHeal;
            utamoVita = (*it)->utamoVita;
            meleeAttack = (*it)->meleeAttack;
            followTarget = (*it)->followTarget;
            wearOutfit = (*it)->wearOutfit;
            break;
        }
    }
    
    bool success = false;
    while(!success)
    {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if(sock == INVALID_SOCKET)
        {
            std::cout << "<*> Invalid Socket. (GameProtocol)" << std::endl;
            continue;
        }
        if(connect(sock, (struct sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR)
        {
            std::cout << "<*> Could not connect. (GameProtocol)" << std::endl;
            closesocket(sock);
            system("pause");
            continue;
        }
        
        netMsg = new NetworkMessage();
        
        netMsg->AddByte(0x0A);
        netMsg->AddU16(0x02);
        
        netMsg->AddU16(tibiaVersion);

	    unsigned long key[4];
	    key[0] = rand() % 0xFFFFFFFF;
	    key[1] = rand() % 0xFFFFFFFF;
	    key[2] = rand() % 0xFFFFFFFF;
	    key[3] = rand() % 0xFFFFFFFF;

        netMsg->AddByte(0x00);

        netMsg->AddU32(key[0]);
        netMsg->AddU32(key[1]);
        netMsg->AddU32(key[2]);
        netMsg->AddU32(key[3]);

        netMsg->AddByte(0x00);
        netMsg->AddU32(account); 
        netMsg->AddString(playerName);
        netMsg->AddString(password);

        for(int x = 26 + password.length() + playerName.length(); x < 128; x++)
            netMsg->AddByte(0x00);
        
        if(otserv)
            netMsg->RSA_encrypt(RSAKEY_OTS);
        else
            netMsg->RSA_encrypt(RSAKEY_CIP);
        
        for(std::vector<Character*>::iterator it = charList.begin(), end = charList.end(); it != end; it++)
        {
            if((*it)->threadId == threadId)
            {
                (*it)->isOnline = true;
                break;
            }
        }
        
        bool waitForAll = true, hasInvalid = false;
        while(waitForAll)
        {
            for(std::vector<Character*>::iterator it = charList.begin(), end = charList.end(); it != end; it++)
            {
                if((*it)->isOnline == false)
                {
                    hasInvalid = true;
                    break;
                }
            }
            if(!hasInvalid)
                waitForAll = false;
            else
                hasInvalid = false;
            Sleep(1);
        }
        
        if(!netMsg->WriteToSocket(sock))
        {
            std::cout << "<*> Error while writing to socket. (GameProtocol)" << std::endl;
            closesocket(sock);
            if(netMsg)
                delete netMsg;
            continue;
        }
        
        netMsg->setEncryptionKey(key);
	    netMsg->setEncryptionState(true);
	    
	    bool online = true, followLeader = false, gotPlayerId = false;
	    bool followSet = false, outfitSet = true, isFollowingLeader = false;
	    
	    std::string lastMessage, command, param;
	    bool executeMessage = false;
	    
	    unsigned long x, oldTargetId = 0;
	    
	    unsigned long initialTime = 0, finalTime = 0, exhaustion = 0, sendPingTime = 0;
	    
	    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
        boost::char_separator<char> sep(" ");
        
        if(followTarget && meleeAttack)
	        fightMode();
	    
	    Sleep(attackDelay);
        
        if(utamoVita)
            say("utamo vita","");
            
        if(threadId == 0)
            say("Pro Magebomb, by Baxnie.",""); // Advert
	    
	    while(online)
	    {
            initialTime = GetTickCount();
            
            // Update information from client
            for(std::vector<Character*>::iterator it = charList.begin(), end = charList.end(); it != end; it++)
            {
                if((*it)->threadId == threadId)
                {
                    playerName = (*it)->playerName;
                    playerRune = (*it)->playerRune;
                    password = (*it)->password;
                    playerId = (*it)->playerId;
                    account = (*it)->account;
                    outfitId = (*it)->outfitId;
                    playerHp = (*it)->playerHp;
                    playerHeal = (*it)->playerHeal;
                    utamoVita = (*it)->utamoVita;
                    meleeAttack = (*it)->meleeAttack;
                    followTarget = (*it)->followTarget;
                    wearOutfit = (*it)->wearOutfit;
                    break;
                }
            }
            // Stop attacking target
            if(target->playerId == 0 && oldTargetId != 0)
            {
                isFollowingLeader = false;
                if(!cancelMove())
                    online = false;
                
                if(followTarget && meleeAttack)
                    if(!fightMode())
                        online = false;
            }    
            // Respond ping. If not responded pings > 24, otserv disconects you
            if(sendPingTime >= 5000)
            {
                sendPing();
                sendPingTime = 0;
            }
            // Answer text messages by leader
            if(executeMessage)
            {
                tokenizer tokens(lastMessage, sep);
                x = 0;
                for(tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
                {
                    if(x == 0)
                        command = *tok_iter;
                    else if(x == 1)
                        param = *tok_iter;
                    x++;
                }
                // Say text
                if(command == "%say")
                {
                    param.clear();    
                    for(x = 0; x < lastMessage.length(); x++)
                        param += lastMessage[x + 5];
                        
                    if(!say(param.c_str(),""))
                        online = false;        
                }
                // Follow leader
                else if(command == "%follow")
                {
                    if(param == "on")
                        followLeader = true;
                    else if(param == "off")
                        followLeader = false;
                }
                // Move direction
                else if(command == "%move")
                {
                    if(param == "north")
                        move(NORTH);
                    else if(param == "south")
                        move(SOUTH);
                    else if(param == "east")
                        move(EAST);
                    else if(param == "west")
                        move(WEST);
                    else if(param == "northwest")
                        move(NORTHWEST);
                    else if(param == "northeast")
                        move(NORTHEAST);
                    else if(param == "southwest")
                        move(SOUTHWEST);
                    else if(param == "southeast")
                        move(SOUTHEAST);
                }
                // Wear leaders outfit
                else if(command == "%wearoutfit")
                {
                    if(!setOutfit(leader->headColor, leader->bodyColor, leader->legsColor, leader->feetColor))
                        online = false;
                }
                // Use a random outfit
                else if(command == "%randomoutfit")
                {
                    srand(time(NULL));
                    if(!setOutfit(rand() % 132, rand() % 132, rand() % 132, rand() % 132))
                        online = false;
                }
                // Drink a fluid
                else if(command == "%drink")
                {
                    if(param == "hp")
                    {
                        if(!useHotkey(hp,playerId))
                            online = false;
                    }
                    else if(param == "mp")
                    {
                        if(!useHotkey(mp,playerId))
                            online = false;
                    }
                    else if(param == "ghp")
                    {
                        if(!useHotkey(ghp,playerId))
                            online = false;
                    }
                    else if(param == "gmp")
                    {
                        if(!useHotkey(gmp,playerId))
                            online = false;
                    }
                    else if(param == "shp")
                    {
                        if(!useHotkey(shp,playerId))
                            online = false;
                    }
                    else if(param == "smp")
                    {
                        if(!useHotkey(smp,playerId))
                            online = false;
                    }
                }
                param.clear();
                command.clear();
                executeMessage = false;
            }
            // New target is set
            if(target->playerId != oldTargetId)
            {
                oldTargetId = target->playerId;
                followSet = false;
                outfitSet = false;
            }
            // Start following leader
            if(followLeader && !isFollowingLeader)
            {
                if(!follow(leader->playerId))
                    online = false;
                followSet = false;
                isFollowingLeader = true;
            }
            // Stop following leader
            else if(!followLeader && isFollowingLeader)
            {
                isFollowingLeader = false;
                if(!cancelMove())
                    online = false;
                if(followTarget && meleeAttack)
                    if(!fightMode())
                        online = false;
            }
            // Attack target
            if(followTarget && meleeAttack && !followSet && !followLeader)
            {
                if(!attackTarget(target->playerId))
                    online = false;
                followSet = true;
                isFollowingLeader = false;
            }
            // Follow target
            else if(followTarget && !meleeAttack && !followSet && !followLeader)
            {
                if(!follow(target->playerId))
                    online = false;
                followSet = true;
                isFollowingLeader = false;
            }
            // Attack target
            else if(!followTarget && meleeAttack && !followSet && !followLeader)
            {
                if(!attackTarget(target->playerId))
                    online = false;
                followSet = true;
                isFollowingLeader = false;
            }
            // Wear outfit if its not set
            if(wearOutfit && !outfitSet)
            {
                if(!setOutfit(target->headColor, target->bodyColor, target->legsColor, target->feetColor))
                    online = false;
                outfitSet = true;
            }
            // Heal
            if(playerHp <= playerHeal && playerHeal > 0)
            {
                if(exhaustion <= exhausted)
                    Sleep(exhausted - exhaustion); 
                if(!useHotkey(uh,playerId))
                    online = false;
                    
                exhaustion = 0;
            }
            // Use rune
            if(playerRune != "NO" && target->playerId != 0 && target->playerId != playerId)
            {
                if(exhaustion <= exhausted)
                    Sleep(exhausted - exhaustion);
                    
                if(playerRune == "SD")
                    if(!useHotkey(sd,target->playerId))
                        online = false;
                else if(playerRune == "HMM")
                    if(!useHotkey(hmm,target->playerId))
                        online = false;
                else if(playerRune == "LMM")
                    if(!useHotkey(lmm,target->playerId))
                        online = false;
                else if(playerRune == "EXP")
                    if(!useHotkey(exP,target->playerId))
                        online = false;
                else if(playerRune == "GFB")
                    if(!useHotkey(gfb,target->playerId))
                        online = false;
                else
                    online = false;
                exhaustion = 0;
            }
            // Read from server
            if(netMsg->ReadFromSocket(sock))
            {
                while(!netMsg->end())
                {
                    unsigned char opt = netMsg->GetByte();
                    switch(opt)
                    {
                        case 0x0A: // player id
                        {
                            for(std::vector<Character*>::iterator it = charList.begin(), end = charList.end(); it != end; it++)
                            {
                                if((*it)->threadId == threadId)
                                {
                                    (*it)->playerId = netMsg->GetU32();
                                    playerId = (*it)->playerId;
                                    break;
                                }
                            }
                            gotPlayerId = true;
                            netMsg->Reset();
                            break;
                        }
                        
                        case 0x14: // error
                        {
                            printf("<*> Error: %s\n",netMsg->GetString().c_str());
                            netMsg->Reset();
                        }
                        
                        case 0xAA: // Say something
                        {
                            bool privateMsg = false;
                            netMsg->GetU32(); // Unknown
                            std::string talker = netMsg->GetString(); // Talker
                            
                            unsigned short talkerLevel = netMsg->GetU16(); // Level
                            switch(netMsg->GetByte()) // Type
                            {
                                case SPEAK_SAY:
		                        case SPEAK_WHISPER:
		                        case SPEAK_YELL:
		                        case SPEAK_MONSTER_SAY:
		                        case SPEAK_MONSTER_YELL:
			                        netMsg->GetPosition();
			                        break;
		                        case SPEAK_CHANNEL_Y:
		                        case SPEAK_CHANNEL_R1:
		                        case SPEAK_CHANNEL_R2:
		                        case SPEAK_CHANNEL_O:
			                        netMsg->GetU16();
			                        break;
                                case SPEAK_PRIVATE:
                                    privateMsg = true;
                                    break;
		                        default:
			                        break;
                            }
                            if(talker == leader->playerName)
                                executeMessage = true;
                        
                            lastMessage = netMsg->GetString();
                            
                            
                            // If was sent a private message to one of mages, send it back to leader
                            if(privateMsg == true && talker != leader->playerName)
                            {
                                std::string newMsg;
                                sprintf((char*)newMsg.c_str(),"%s[%d]: %s",talker.c_str(),talkerLevel,lastMessage.c_str());
                                say(newMsg,leader->playerName);
                            }
                            break;
                        }
                        
                        default:
                        {
                            if(gotPlayerId)
                                netMsg->Reset();
                            break;
                        }
                    }
                }
            }
            else
                online = false;
                
            Sleep(1);
            finalTime = GetTickCount();
            
            exhaustion += finalTime - initialTime;
            sendPingTime += finalTime - initialTime;
        }
        
        for(std::vector<Character*>::iterator it = charList.begin(), end = charList.end(); it != end; it++)
        {
            if((*it)->threadId == threadId)
            {
                (*it)->isOnline = false;
                break;
            }
        }
        
        closesocket(sock);
        if(netMsg)
            delete netMsg;
        success = true;
    }
}
