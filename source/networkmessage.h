#ifndef __NETWORK_MESSAGE_H__
#define __NETWORK_MESSAGE_H__

#define NETWORKMESSAGE_MAXSIZE 16384

#include <winsock.h>
#include <string>

class Position;
class RSA;

class NetworkMessage
{
public:
	// constructor/destructor
	NetworkMessage();
	~NetworkMessage();

	// resets the internal buffer to an empty message
	void Reset();

	// socket functions
	bool ReadFromSocket(SOCKET socket);
	bool WriteToSocket(SOCKET socket);

	// simply read functions for incoming message
	unsigned char  GetByte(bool dontMoveReadPos = false);
	unsigned short GetU16(bool dontMoveReadPos = false);
	unsigned int GetU32(bool dontMoveReadPos = false);
	std::string GetString();

	// read functions for complex types
	Position GetPosition();
	//Creature *GetCreature();

	void setEncryptionState(bool state);
	void setEncryptionKey(const unsigned long* key);

	// skips count unknown/unused bytes in an incoming message
	void SkipBytes(int count){m_ReadPos += count;}

	// simply write functions for outgoing message
	void AddByte(unsigned char  value);
	void AddU16(unsigned short value);
	void AddU32(unsigned int value);
	void AddString(const std::string &value);
	void AddString(const char* value);

	// write functions for complex types
	void AddPosition(const Position &pos);

	bool end() const;

	bool RSA_encrypt(char* key);

protected:
	inline bool canAdd(int size)
	{
		return (size + m_ReadPos < NETWORKMESSAGE_MAXSIZE - 16);
	};

	void XTEA_encrypt();
	void XTEA_decrypt();

	int m_MsgSize;
	int m_ReadPos;

	unsigned char m_MsgBuf[NETWORKMESSAGE_MAXSIZE];

	bool m_encryptionEnabled;
	bool m_keyset;
	unsigned long m_key[4];
};

#endif // #ifndef __NETWORK_MESSAGE_H__
