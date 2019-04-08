#include "Base64.hpp"
#include <string.h>

namespace BASE_64
{
	const char *g_pEnKey = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	const int g_pDekey[] = {
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		62, // '+'
		-1, -1, -1,
		63, // '/'
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
		-1, -1, -1, -1, -1, -1, -1,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
		13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
		-1, -1, -1, -1, -1, -1,
		26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
		39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
	};

	emBase64_Err Base64EncodeString( const char *pString, char *pDest, int nDestSize )
	{
		emBase64_Err emRet = em_BASE64_ERR;
		for (;;)
		{
			if (NULL == pString)
			{
				emRet = em_BASE64_PARAMETER_ERR;
				break;
			}

			int nByteSrc = strlen(pString);
			for (int i = 0; i < nByteSrc; i ++)
			{
				if (0 > pString[i] || 127 < pString[i])
				{
					emRet = em_BASE64_NO_ASCII;
					break;
				}
			}
			if (em_BASE64_NO_ASCII == emRet)
			{
				break;
			}
			memset(pDest, 0, nDestSize);
			int nLoop = 0 == nByteSrc % 3 ? nByteSrc : nByteSrc - 3;
			int nIndex = 0;
			int i = 0;
			for (; i < nLoop; i += 3)
			{
				pDest[nIndex]     = g_pEnKey[pString[i] >> 2];
				pDest[nIndex + 1] = g_pEnKey[((pString[i] & 3) << 4) | ((pString[i + 1] & 0xF0) >> 4)];
				pDest[nIndex + 2] = g_pEnKey[((pString[i + 1] & 0x0F) << 2) | ((pString[i + 2] & 0xC0) >> 6)];
				pDest[nIndex + 3] = g_pEnKey[pString[i + 2] & 0x3F];
				nIndex += 4;
			}
			switch(nByteSrc % 3)
			{
			case 0:
				{
					pDest[nIndex] = 0;
					break;
				}
			case 1:
				{
					pDest[nIndex]     = g_pEnKey[pString[i] >> 2];
					pDest[nIndex + 1] = g_pEnKey[((pString[i] & 3) << 4) | ((0 & 0xf0) >> 4)];
					pDest[nIndex + 2] = '=';
					pDest[nIndex + 3] = '=';
					pDest[nIndex + 4] = '\0';
					break;
				}
			case 2:
				{
					pDest[nIndex]     = g_pEnKey[pString[i] >> 2];
					pDest[nIndex + 1] = g_pEnKey[((pString[i] & 3) << 4) | ((pString[i + 1] & 0xf0) >> 4)];
					pDest[nIndex + 2] = g_pEnKey[((pString[i + 1] & 0xf) << 2) | ((0 & 0xC0) >> 6)];
					pDest[nIndex + 3] = '=';
					pDest[nIndex + 4] = '\0';
					break;
				}
			}

			emRet = em_BASE64_OK;
			break;
		}

		return emRet;
	}

	int GetBase64EncodeStringSize( const char *pString )
	{
		int nRet = -1;
		for (;;)
		{
			if (NULL == pString)
			{
				break;
			}

			int nLen = strlen(pString);
			nRet = (nLen * 4) / 3 + 4;

			break;
		}
		return nRet;
	}

	emBase64_Err Base64DecodeString( const char *pBase64String, char *pDest, int nDestSize )
	{
		emBase64_Err emRet = em_BASE64_ERR;
		for (;;)
		{
			if (NULL == pBase64String || 0 == pBase64String[0])
			{
				emRet = em_BASE64_PARAMETER_ERR;
				break;
			}
			int nBase64StringLen = strlen(pBase64String);
			if (0 != nBase64StringLen % 4)
			{
				emRet = em_BASE64_BAD_BASE64;
				break;
			}
			memset(pDest, 0, nDestSize);
			int nLoop = '=' == pBase64String[nBase64StringLen - 1] ? nBase64StringLen - 4 : nBase64StringLen;
			int b[4] = {0};
			int i = 0, n = 0;

			for (; i < nLoop; i += 4)
			{
				b[0] = g_pDekey[pBase64String[i]];
				b[1] = g_pDekey[pBase64String[i + 1]];
				b[2] = g_pDekey[pBase64String[i + 2]];
				b[3] = g_pDekey[pBase64String[i + 3]];

				if (-1 == b[0] || -1 == b[1] || -1 == b[2] || -1 == b[3])
				{
					emRet = em_BASE64_BAD_BASE64;
					break;
				}

				pDest[n]     = (b[0] << 2) | ((b[1] &0x30) >> 4);
				pDest[n + 1] = ((b[1] & 0xf) << 4) | ((b[2] & 0x3C) >> 2);
				pDest[n + 2] = ((b[2] & 0x3) << 6) | b[3];

				n += 3;
			}

			if (em_BASE64_BAD_BASE64 == emRet)
			{
				break;
			}

			if ('=' == pBase64String[nBase64StringLen - 1] && '=' == pBase64String[nBase64StringLen - 2])
			{
				b[0] = g_pDekey[pBase64String[i]];
				b[1] = g_pDekey[pBase64String[i + 1]];
				if (-1 == b[0] || -1 == b[1])
				{
					emRet = em_BASE64_BAD_BASE64;
					break;
				}
				pDest[n]     = (b[0] << 2) | ((b[1] &0x30) >> 4);
				pDest[n + 1] = '\0';
			}
			if (em_BASE64_BAD_BASE64 == emRet)
			{
				break;
			}

			if ('=' == pBase64String[nBase64StringLen - 1] && '=' != pBase64String[nBase64StringLen - 2])
			{
				b[0] = g_pDekey[pBase64String[i]];
				b[1] = g_pDekey[pBase64String[i + 1]];
				b[2] = g_pDekey[pBase64String[i + 2]];
				if (-1 == b[0] || -1 == b[1] || -1 == b[2])
				{
					emRet = em_BASE64_BAD_BASE64;
					break;
				}
				pDest[n]     = (b[0] << 2) | ((b[1] & 0x30) >> 4);
				pDest[n + 1] = ((b[1] &0xf) << 4) | ((b[2] &0x3c) >> 2);
				pDest[n + 2] = '\0';
			}
			if (em_BASE64_BAD_BASE64 == emRet)
			{
				break;
			}

			if ('=' != pBase64String[nBase64StringLen - 1] && '=' != pBase64String[nBase64StringLen - 2])
			{
				pDest[n] = '\0';
			}

			emRet = em_BASE64_OK;
			break;
		}

		return emRet;
	}

	int GetBase64DecodeString( const char *pBase64String )
	{
		int nRet = -1;
		for (;;)
		{
			if (NULL == pBase64String)
			{
				break;
			}
			int nLen = strlen(pBase64String);
			nRet = (nLen * 3) / 4 + 4;
			break;
		}
		return nRet;
	}

}