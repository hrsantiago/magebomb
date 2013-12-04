#ifndef __RSA_H__
#define __RSA_H__

#include <gmp.h>
#include <string>

class RSA
{
public:
	RSA();
	~RSA();

	static RSA* getInstance();

	void setKey(char* mod);
	void encrypt(char *msg, long size);
	
protected:
	mpz_t m_e, m_mod, m, c;
};

#endif
