#ifndef COM_HSDI_PCM_TEST_TOOLS
#define COM_HSDI_PCM_TEST_TOOLS

#include<faad.h>
#include<android\log.h>
#include<iostream>
using namespace std;
int get_ADTS_frame(unsigned char*, size_t, unsigned char*, size_t*);


#endif