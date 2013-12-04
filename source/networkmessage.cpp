#include "NetworkMessage.h"
#include "Position.h"
#include "RSA.h"

NetworkMessage::NetworkMessage() {
    m_encryptionEnabled = false;
    m_keyset = false;
    Reset();
}

NetworkMessage::~NetworkMessage() {
    
}

void NetworkMessage::Reset() {
    m_MsgSize = 0;
    m_ReadPos = 4;
}

bool NetworkMessage::ReadFromSocket(SOCKET socket) {
    // just read the size to avoid reading 2 messages at once
    m_MsgSize = recv(socket, (char*)m_MsgBuf, 2, 0);
    
    // for now we expect 2 bytes at once, it should not be splitted
    //unsigned short datasize = m_MsgBuf[0] | m_MsgBuf[1] << 8;
    unsigned short datasize = *(unsigned short*)(m_MsgBuf);
    if((m_MsgSize != 2) || (datasize > NETWORKMESSAGE_MAXSIZE-2)){
        int errnum;
        errnum = ::WSAGetLastError();
        if(errnum == WSAEWOULDBLOCK){
            m_MsgSize = 0;
            return true;
        }
        if(datasize > NETWORKMESSAGE_MAXSIZE-2 && m_MsgSize == 2)
            std::cout << "Failure: [NetworkMessage::ReadFromSocket]. Greater size (" << datasize << ")" << std::endl;
        else
            std::cout << "Failure: [NetworkMessage::ReadFromSocket]. Read failed" << std::endl;
        return false;
    }
    
    int readBytes = 0;
    int read = 0;
    while(readBytes < datasize) {
        // read the real data
        read = recv(socket, (char*)m_MsgBuf+2+readBytes, std::min(datasize-readBytes, 1000), 0);
        m_MsgSize += read;
        readBytes += read;
        
        // we got something unexpected/incomplete
        if(read <= 0) {
            std::cout << "Failure: [NetworkMessage::ReadFromSocket]. Unexpected/incomplete data" << std::endl;
            return false;
        }
    }
    
    // we got something unexpected/incomplete
    if(datasize != m_MsgSize-2) {
        std::cout << "Failure: [NetworkMessage::ReadFromSocket]. Incorrect data size" << std::endl;
        return false;
    }
    
    m_ReadPos = 2;
    
    //decrypt
    if(m_encryptionEnabled){
        if(!m_keyset){
            std::cout << "Failure: [NetworkMessage::ReadFromSocket]. Key not set" << std::endl;
            return false;
        }
        if((m_MsgSize - 2) % 8 != 0){
            std::cout << "Failure: [NetworkMessage::ReadFromSocket]. Not valid encrypted message size" << std::endl;
            return false;
        }
        XTEA_decrypt();
        int tmp = GetU16();
        if(tmp > m_MsgSize - 4){
            std::cout << "Failure: [NetworkMessage::ReadFromSocket]. Not valid unencrypted message size" << std::endl;
            return false;
        }
        m_MsgSize = tmp;
    }
    return true;
}


bool NetworkMessage::WriteToSocket(SOCKET socket) {
    if(m_MsgSize == 0)
        return true;
    
    m_MsgBuf[2] = (unsigned char)(m_MsgSize);
    m_MsgBuf[3] = (unsigned char)(m_MsgSize >> 8);
    
    bool ret = true;
    
    // Set the socket I/O mode; iMode = 0 for blocking; iMode != 0 for non-blocking
    unsigned long mode = 1;
    ioctlsocket(socket, FIONBIO, &mode);
    
    int start;
    if(m_encryptionEnabled){
        if(!m_keyset){
            std::cout << "Failure: [NetworkMessage::WriteToSocket]. Key not set" << std::endl;
            return false;
        }
        start = 0;
        XTEA_encrypt();
    }
    else{
        start = 2;
    }
    
    int sendBytes = 0;
    int retry = 0;
    do{
        int b = send(socket, (char*)m_MsgBuf+start+sendBytes, std::min(m_MsgSize+2-sendBytes, 1000), 0);
        if(b <= 0) {
            int errnum = ::WSAGetLastError();
            if(errnum == WSAEWOULDBLOCK) {
                b = 0;
                Sleep(10);
                retry++;
                if(retry == 10){
                    ret = false;
                    break;
                }
            }
            else {
                ret = false;
                break;
            }
        }
        sendBytes += b;
    } while(sendBytes < m_MsgSize+2);
    
    mode = 0;
    ioctlsocket(socket, FIONBIO, &mode);
    
    return ret;
}


/******************************************************************************/
unsigned char NetworkMessage::GetByte(bool dontMoveReadPos) {
    if(dontMoveReadPos)
        return m_MsgBuf[m_ReadPos];
    else
        return m_MsgBuf[m_ReadPos++];
}
unsigned short NetworkMessage::GetU16(bool dontMoveReadPos) {
    unsigned short v = *(unsigned short*)(m_MsgBuf + m_ReadPos);
    if(!dontMoveReadPos)
        m_ReadPos += 2;
    return v;
}
unsigned int NetworkMessage::GetU32(bool dontMoveReadPos) {
    unsigned int v = *(unsigned int*)(m_MsgBuf + m_ReadPos);
    if(!dontMoveReadPos)
        m_ReadPos += 4;
    return v;
}
std::string NetworkMessage::GetString() {
    unsigned short stringlen = GetU16();
    if(stringlen >= (NETWORKMESSAGE_MAXSIZE - 16 - m_ReadPos))
        return std::string();
    
    char* v = (char*)(m_MsgBuf + m_ReadPos);
    m_ReadPos += stringlen;
    return std::string(v, stringlen);
}

Position NetworkMessage::GetPosition() {
    Position pos;
    pos.x = GetU16();
    pos.y = GetU16();
    pos.z = GetByte();
    return pos;
}

/******************************************************************************/
void NetworkMessage::AddByte(unsigned char  value) {
    if(!canAdd(1))
        return;
    m_MsgBuf[m_ReadPos++] = value;
    m_MsgSize++;
}
void NetworkMessage::AddU16(unsigned short value) {
    if(!canAdd(2))
        return;
    *(unsigned short*)(m_MsgBuf + m_ReadPos) = value;
    m_ReadPos += 2; m_MsgSize += 2;
}
void NetworkMessage::AddU32(unsigned int value) {
    if(!canAdd(4))
        return;
    *(unsigned int*)(m_MsgBuf + m_ReadPos) = value;
    m_ReadPos += 4; m_MsgSize += 4;
}
void NetworkMessage::AddString(const char* value) {
    unsigned int stringlen = (unsigned int)strlen(value);
    if(!canAdd(stringlen+2) || stringlen > 8192)
        return;
    
    AddU16(stringlen);
    strcpy((char*)(m_MsgBuf + m_ReadPos), value);
    m_ReadPos += stringlen;
    m_MsgSize += stringlen;
}

void NetworkMessage::AddString(const std::string &value) {
    AddString(value.c_str());
}

void NetworkMessage::AddPosition(const Position& pos) {
    AddU16(pos.x);
    AddU16(pos.y);
    AddByte(pos.z);
}

/******************************************************************************/

bool NetworkMessage::end() const {
    return !(m_ReadPos < m_MsgSize+2);
}

/******************************************************************************/
void NetworkMessage::setEncryptionState(bool state) {
    m_encryptionEnabled = state;
}

void NetworkMessage::setEncryptionKey(const unsigned long* key) {
    memcpy(m_key, key, 16);
    m_keyset = true;
}

void NetworkMessage::XTEA_encrypt() {
    unsigned long k[4];
    k[0] = m_key[0]; k[1] = m_key[1]; k[2] = m_key[2]; k[3] = m_key[3];
    
    //add bytes until reach 8 multiple
    unsigned long n;
    if(((m_MsgSize + 2) % 8) != 0){
        n = 8 - ((m_MsgSize + 2) % 8);
        memset((void*)&m_MsgBuf[m_ReadPos], 0, n);
        m_MsgSize = m_MsgSize + n;
    }
    
    unsigned long read_pos = 0;
    unsigned long* buffer = (unsigned long*)&m_MsgBuf[2];
    while(read_pos < m_MsgSize/4){
        unsigned long v0 = buffer[read_pos], v1 = buffer[read_pos + 1];
        unsigned long delta = 0x61C88647;
        unsigned long sum = 0;
        
        for(unsigned long i = 0; i<32; i++) {
            v0 += ((v1 << 4 ^ v1 >> 5) + v1) ^ (sum + k[sum & 3]);
            sum -= delta;
            v1 += ((v0 << 4 ^ v0 >> 5) + v0) ^ (sum + k[sum>>11 & 3]);
        }
        buffer[read_pos] = v0; buffer[read_pos + 1] = v1;
        read_pos = read_pos + 2;
    }
    m_MsgSize = m_MsgSize + 2;
    m_MsgBuf[0] = (unsigned char)(m_MsgSize);
    m_MsgBuf[1] = (unsigned char)(m_MsgSize >> 8);
}

void NetworkMessage::XTEA_decrypt() {
    unsigned long k[4];
    k[0] = m_key[0]; k[1] = m_key[1]; k[2] = m_key[2]; k[3] = m_key[3];
    
    unsigned long* buffer = (unsigned long*)&m_MsgBuf[2];
    unsigned long read_pos = 0;
    while(read_pos < m_MsgSize/4){
        unsigned long v0 = buffer[read_pos], v1 = buffer[read_pos + 1];
        unsigned long delta = 0x61C88647;
        unsigned long sum = 0xC6EF3720;
        
        for(unsigned long i = 0; i<32; i++) {
            v1 -= ((v0 << 4 ^ v0 >> 5) + v0) ^ (sum + k[sum>>11 & 3]);
            sum += delta;
            v0 -= ((v1 << 4 ^ v1 >> 5) + v1) ^ (sum + k[sum & 3]);
        }
        buffer[read_pos] = v0; buffer[read_pos + 1] = v1;
        read_pos = read_pos + 2;
    }
}

bool NetworkMessage::RSA_encrypt(char* key) {
    RSA* rsa = RSA::getInstance();
    rsa->setKey(key);
    rsa->encrypt((char*)&m_MsgBuf[m_ReadPos-128], 128);
    return true;
}
