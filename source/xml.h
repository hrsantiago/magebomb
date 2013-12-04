#ifndef __XML_H__
#define __XML_H__

#ifdef XML_GCC_FREE
#define xmlFreeNodeValue(s)	free(s)
#else
#define xmlFreeNodeValue(s)	xmlFree(s)
#endif

bool loadChar();

#endif
