#include <iostream>
#include <vector>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "XML.h"
#include "Data.h"

extern std::vector<Character*> charList;
extern std::vector<Target*> targetList;

bool loadChar()
{
    std::string str;
    xmlDocPtr doc = xmlParseFile("characters.xml");
    if(doc)
    {
        xmlNodePtr root, p;
        char *nodeValue = NULL;
        root = xmlDocGetRootElement(doc);
        p = root->children;

        if(xmlStrcmp(root->name,(const xmlChar*) "characters") != 0)
        {
            xmlFreeDoc(doc);
            return false;
        }
        while(p)
        {
            str = (char*)p->name;
            if(str == "character")
            {
                Character *char1 = new Character;
                
                nodeValue = (char*)xmlGetProp(p, (const xmlChar *)"account");
                if(nodeValue)
                {
                    char1->account = atol(nodeValue);
                    xmlFreeNodeValue(nodeValue);
                }
                nodeValue = (char*)xmlGetProp(p, (const xmlChar *)"password");
                if(nodeValue)
                {
                    char1->password = nodeValue;
                    xmlFreeNodeValue(nodeValue);
                }
                nodeValue = (char*)xmlGetProp(p, (const xmlChar *)"name");
                if(nodeValue)
                {
                    char1->playerName = nodeValue;
                    xmlFreeNodeValue(nodeValue);
                }
                nodeValue = (char*)xmlGetProp(p, (const xmlChar *)"utamovita");
                if(nodeValue)
                {
                    std::string utamoVita = nodeValue;
                    if(utamoVita == "YES")
                        char1->utamoVita = true;
                    else if(utamoVita == "NO")
                        char1->utamoVita = false;
                    else
                    {
                        std::cout << "\nInto \"utamovita\" value can be only YES or NO." << std::endl;
                        return false;
                    }
                    xmlFreeNodeValue(nodeValue);
                }
                nodeValue = (char*)xmlGetProp(p, (const xmlChar *)"rune");
                if(nodeValue)
                {
                    char1->playerRune = nodeValue;
                    xmlFreeNodeValue(nodeValue);
                }
                nodeValue = (char*)xmlGetProp(p, (const xmlChar *)"meleeattack");
                if(nodeValue)
                {
                    std::string meleeAttack = nodeValue;
                    if(meleeAttack == "YES")
                        char1->meleeAttack = true;
                    else if(meleeAttack == "NO")
                        char1->meleeAttack = false;
                    else
                    {
                        std::cout << "\nInto \"meleeattack\" value can be only YES or NO." << std::endl;
                        return false;
                    }
                    xmlFreeNodeValue(nodeValue);
                }
                nodeValue = (char*)xmlGetProp(p, (const xmlChar *)"followtarget");
                if(nodeValue)
                {
                    std::string followTarget = nodeValue;
                    if(followTarget == "YES")
                        char1->followTarget = true;
                    else if(followTarget == "NO")
                        char1->followTarget = false;
                    else
                    {
                        std::cout << "\nInto \"followtarget\" value can be only YES or NO." << std::endl;
                        return false;
                    }
                    xmlFreeNodeValue(nodeValue);
                }
                nodeValue = (char*)xmlGetProp(p, (const xmlChar *)"wearoutfit");
                if(nodeValue)
                {
                    std::string wearOutfit = nodeValue;
                    if(wearOutfit == "YES")
                        char1->wearOutfit = true;
                    else if(wearOutfit == "NO")
                        char1->wearOutfit = false;
                    else
                    {
                        std::cout << "\nInto \"wearoutfit\" value can be only YES or NO." << std::endl;
                        return false;
                    }
                    xmlFreeNodeValue(nodeValue);
                }
                nodeValue = (char*)xmlGetProp(p, (const xmlChar *)"heal");
                if(nodeValue)
                {
                    char1->playerHeal = atol(nodeValue);
                    xmlFreeNodeValue(nodeValue);
                }
                charList.push_back(char1);
            }
            p=p->next; 
        }
        xmlFreeDoc(doc);
    }
    else
        return false;
        
    return true;
}
