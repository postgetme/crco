#include "includes.h"

INT8S big_small_mode_switch(void *pdata, INT8U llong)
{
	INT8U* pp = pdata;
	INT8U i;
	INT8U tmp;
	INT8U buflong = llong-1;
	
	if (llong%2!=0) {
		return FALSE;
	}

	for (i=0; i<llong/2; i++) {
		tmp = pp[i];
		pp[i] = pp[buflong-i];
		pp[buflong-i] = tmp;
	}

	return TRUE;
}

INT8U check_sum(INT8U* pdata, INT16U dlong)
{
	INT8U csum 	= 0;
	INT16U i = 0;
	INT8U* pp 	= pdata;
	
	for (i=0; i<dlong; i++)	{
		csum += *pp++;
	}

	return csum;
}

int asc_to_hex(char* ascii, char* hex)
{
    int i, len = strlen(ascii);
    const char chHex[] = "0123456789ABCDEF";
                    
    for (i = 0; i<len; i++)
    {
        hex[i*3]    = chHex[(ascii[i]) >> 4];
        hex[i*3 +1] = chHex[(ascii[i]) & 0xf];
        hex[i*3 +2] = ' ';
    }

    hex[len * 3] = '\0';

    return len * 3;
}

int hex_to_asc(char* hex, char* ascii)
{
    int len = strlen(hex), tlen, i, cnt;
	INT8U t;
    
    for (i = 0, cnt = 0, tlen = 0; i<len; i++)
    {
        char c = toupper(hex[i]);

        if ((c>='0'&& c<='9') || (c>='A'&& c<='F'))
        {
            t = (c >= 'A') ? c - 'A' + 10 : c - '0';

            if (cnt)
                ascii[tlen++] += t, cnt = 0;
            else
                ascii[tlen] = t << 4, cnt = 1;
        }
    }
    
    return tlen;
}
