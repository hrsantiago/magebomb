#ifndef __DATA_H__
#define __DATA_H__

#include <string>

struct Character
{
    Character()
    {
        playerId = 0;
        playerHeal = 0;
        playerHp = 100;
        outfitId = 128;
        utamoVita = false;
        meleeAttack = true;
        followTarget = false;
        isOnline = false;
        wearOutfit = false;
    }
    
    std::string playerName, playerRune, password;
    unsigned long playerId, account;
    unsigned short outfitId;
    unsigned char playerHp, playerHeal, threadId;
    bool utamoVita, meleeAttack, followTarget, isOnline, wearOutfit;
};

struct Target
{
    Target()
    {
        playerId = 0;
        headColor = 0;
        bodyColor = 0;
        legsColor = 0;
        feetColor = 0;
    }
    
    std::string playerName;
    unsigned long playerId;
    unsigned char headColor, bodyColor, legsColor, feetColor;
};

struct Leader
{
    Leader()
    {
        playerId = 0;
        headColor = 0;
        bodyColor = 0;
        legsColor = 0;
        feetColor = 0;
    }
    
    std::string playerName;
    unsigned long playerId;
    unsigned char headColor, bodyColor, legsColor, feetColor;
};

#endif
