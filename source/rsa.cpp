#include "RSA.h"

static RSA* m_Instance = NULL;

RSA::RSA()
{
	mpz_init2(m_mod, 1024); 	
    mpz_init2(m_e, 32);
}

RSA::~RSA()
{
	mpz_clear(m_mod); 	
	mpz_clear(m_e);
}

RSA* RSA::getInstance()
{
	if(!m_Instance)
		m_Instance = new RSA();
	return m_Instance;
}

void RSA::setKey(char* mod)
{		
    mpz_set_ui(m_e, 65537); 
    mpz_set_str(m_mod, mod, 10);
}

void RSA::encrypt(char *msg, long size)
{	
   mpz_init2(m, 1024); 	
   mpz_init2(c, 1024); 	

   mpz_import(m, 128, 1, 1, 0, 0, msg); 	
		
   mpz_powm(c, m, m_e, m_mod); 	
			
   size_t count = (mpz_sizeinbase(c, 2) + 7)/8; 	
   memset(msg, 0, 128 - count); 
   size_t countp;
   mpz_export(&msg[128 - count], &countp, 1, 1, 0, 0, c); 	
		
   mpz_clear(m); 	
   mpz_clear(c);
}
