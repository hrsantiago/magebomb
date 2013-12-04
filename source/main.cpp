#include <windows.h>
#include <iostream>
#include <vector>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "Config.h"
#include "XML.h"
#include "Data.h"
#include "Login.h"
#include "ConstTibia.h"

unsigned short attackDelay, exhausted, tibiaVersion;
bool otserv = true;
std::string host;

boost::recursive_mutex loginMutex;
Target *target;
Leader *leader;

HANDLE hProcess;

std::vector<Character*> charList;

ConfigManager g_config;

bool readFromTibia()
{
    std::string playerName;
    unsigned long playerId, playerHp, headColor, bodyColor, legsColor, feetColor, outfitId;
    unsigned char readByte, version;
    unsigned long readAddress, x;
    bool readBattle = true;
    
    boost::recursive_mutex::scoped_lock lock(loginMutex);
    if(tibiaVersion == 800)
        version = 0;
    else if(tibiaVersion == 810)
        version = 1;
    else if(tibiaVersion == 811)
        version = 2;
    else if(tibiaVersion == 820)
        version = 3;
    else if(tibiaVersion == 821)
    	version = 4;
    else if(tibiaVersion == 822)
    	version = 5;
    
    // Read Target_ID
    ReadProcessMemory(hProcess, (LPVOID)targetIdAddress[version], &target->playerId, 4, NULL);
    
    // Read Leader_ID
    ReadProcessMemory(hProcess, (LPVOID)playerIdAddress[version], &leader->playerId, 4, NULL);
    lock.unlock();
    
    readAddress = battleBeginAddress[version];
    
    while(readBattle)
    {
        // Stop reading at the end of battle list
        if(battleEndAddress[version] <= readAddress)
        {
            readBattle = false;
            break;
        }
        
        // Read Player id
        ReadProcessMemory(hProcess, (LPVOID)readAddress, &playerId, 4, NULL);
        readAddress += 4;
        
        // Read Player_Name
        for(x = readAddress; x < readAddress + 32; x++)
        {
            ReadProcessMemory(hProcess, (LPVOID)x, &readByte, 1, NULL);
            if(readByte != 0x00)
                playerName += readByte;
            else
                break;
        }
        readAddress += 32;
        
        // Stop reading if player name is invalid
        if(playerName[0] == 0x00)
        {
            readBattle = false;
            break;
        }
        
        // Skip Walking, Direction, X, Y, Z
        readAddress += 60;
        
        // Read Outfit
        ReadProcessMemory(hProcess, (LPVOID)readAddress, &outfitId, 4, NULL);
        readAddress += 4;
        ReadProcessMemory(hProcess, (LPVOID)readAddress, &headColor, 4, NULL);
        readAddress += 4;
        ReadProcessMemory(hProcess, (LPVOID)readAddress, &bodyColor, 4, NULL);
        readAddress += 4;
        ReadProcessMemory(hProcess, (LPVOID)readAddress, &legsColor, 4, NULL);
        readAddress += 4;
        ReadProcessMemory(hProcess, (LPVOID)readAddress, &feetColor, 4, NULL);
        readAddress += 4;
        
        //Skip Addon, Light, Light_Color, Light_Patten
        readAddress += 20;
        
        // Read HP
        ReadProcessMemory(hProcess, (LPVOID)readAddress, &playerHp, 4, NULL);
        readAddress += 4;
        
        // Skip Walk_Speed, Is_Visible, Skull, Party
        readAddress += 20;
        
        // Set leader info
        if(leader->playerId == playerId)
        {
            leader->playerName = playerName;
            leader->headColor = headColor;
            leader->bodyColor = bodyColor;
            leader->legsColor = legsColor;
            leader->feetColor = feetColor;
        }
        
        // Set character Outfit, HP
        lock.lock();
        for(std::vector<Character*>::iterator it = charList.begin(), end = charList.end(); it != end; it++)
        {
            if((*it)->playerId == playerId)
            {
                (*it)->outfitId = outfitId;
                (*it)->playerHp = playerHp;
                break;
            }
        }
        lock.unlock();
        
        // Update information about target
        if(target->playerId == playerId)
        {
            lock.lock();
            
            target->playerName = playerName;
            target->headColor = headColor;
            target->bodyColor = bodyColor;
            target->legsColor = legsColor;
            target->feetColor = feetColor;
            
            lock.unlock();
        }
        playerName.clear();
    }
    return true;
}

bool checkTibia()
{
    // Check if Tibia.exe is open.
    HWND tibiaWnd = FindWindow("tibiaclient",NULL);
    if(tibiaWnd)
    {
        DWORD processId;
        GetWindowThreadProcessId(tibiaWnd, &processId);
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
        if(hProcess)
            return true;
        else
            return false;
    }
    else
        return false;
}

bool Initialize()
{
    system("title Pro MageBomb");
    system("color 1F");
    
    std::cout << "[*] Pro Magebomb by Baxnie." << std::endl;
    std::cout << "[*] Report bugs to: promagebomb@hotmail.com" << std::endl;
    std::cout << "[*] Read the file \"readme.txt\" and configure \"config.lua\" and \"characters.xml\"\n" << std::endl;
    
    std::cout << "[*] Initializing Winsock 1.1... ";
    WSAData wsadata;
    if(WSAStartup(MAKEWORD(1,1),&wsadata) != 0)
    {
        std::cout << "[error initializing winsock]" << std::endl;
        return false;
    }
    std::cout << "[done]" << std::endl;
    
    std::cout << "[*] Loading \"config.lua\"... ";
    if(!g_config.loadFile("config.lua"))
    {
        std::cout << "[error loading config.lua]" << std::endl;
        WSACleanup();
        return false;
    }
    std::cout << "[done]" << std::endl;

    std::cout << "[*] Loading \"characters.xml\"... ";
    if(!loadChar())
    {
        std::cout << "[error loading characters.xml]" << std::endl;
        WSACleanup();
        return false;
    }
    std::cout << "[done]\n" << std::endl;
    
    return true;
}

int main()
{
    if(!Initialize())
    {
        system("pause");
        return -1;
    }
    
    unsigned char threadId = 0;
    
    const unsigned long port = g_config.getNumber(ConfigManager::PORT);
    const unsigned long loginDelay = g_config.getNumber(ConfigManager::LOGIN_DELAY);
    
    attackDelay = g_config.getNumber(ConfigManager::ATTACK_DELAY);
    exhausted = g_config.getNumber(ConfigManager::EXHAUSTED);
    tibiaVersion = g_config.getNumber(ConfigManager::VERSION);
    host = g_config.getString(ConfigManager::SERVERNAME);
    
    target = new Target();
    leader = new Leader();
    
    if(tibiaVersion != 800 && tibiaVersion != 810 && tibiaVersion != 811 && tibiaVersion != 820 && tibiaVersion != 821 && tibiaVersion != 822)
    {
        std::cout << "<*> Version is invalid. You can use 800 or 810 or 811 or 820 or 821 or 822." << std::endl;
        system("pause");
        WSACleanup();
        return -1;
    }
    
    if(host == "tibia05.cipsoft.com")
    {
        otserv = false;
        tibiaVersion = currentTibiaVersion;
    }
    
    if(!checkTibia())
    {
        std::cout << "<*> Your Tibia.exe must be opened before." << std::endl;
        system("pause");
        WSACleanup();
        return -1;
    }
    if(!readFromTibia())
    {
        std::cout << "<*> Unknown error." << std::endl;
        system("pause");
        WSACleanup();
        return -1;
    }
    
    struct hostent* nHost = gethostbyname(host.c_str());
    
    if(!nHost)
    {
        std::cout << "<*> Could not resolve hostname." << std::endl;
        system("pause");
        WSACleanup();
        return -1;
    }
    
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = ((struct in_addr *)(nHost->h_addr))->s_addr;
    sin.sin_port = htons(port);
    
    Login *login = NULL;
    for(std::vector<Character*>::iterator it = charList.begin(), end = charList.end(); it != end; it++)
	{
        (*it)->threadId = threadId;
        login = new Login(sin, (*it)->threadId);
        boost::thread loginThread(boost::bind(&Login::LoginProtocol, login));
        threadId++;
        
        Sleep(loginDelay);
    }
    
    // Stay updating information from client
    bool bQuit = false;
    while(!bQuit)
    {
        if(!readFromTibia())
            bQuit = true;
            
        Sleep(50);
    }
    
    if(target)
        delete target;
    if(leader)
        delete leader;
    
    WSACleanup();
    return 0;
}
