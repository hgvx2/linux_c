#ifndef BASE64_HPP
#define BASE64_HPP

//http://www.cnblogs.com/mr-wid/archive/2013/10/25/3388719.html
namespace BASE_64
{
	enum emBase64_Err
	{
		em_BASE64_OK = 0,             //OK
		em_BASE64_ERR,                //error
		em_BASE64_NO_ASCII,           //The parameter is no ascii code
		em_BASE64_PARAMETER_ERR,      //The parameter's error
		em_BASE64_BAD_BASE64          //bad base64 string
	};

	typedef int BASE64_ERR_CODE;

	//base64 编码
	/* @Base64EncodeString
	 * 对于ASCII编码的字符串进行BASE64编码
	 * @pString : 需要编码的字符串
	 * @pDest ：对 pString编码之后存储的地址， 长度必须调用GetBase64EncodeStringSize 获取
	 * @nDestSize: pDest的长度
	 * @返回值：如果没有错误发生， 返回em_BASE64_OK.
     */
	emBase64_Err Base64EncodeString(const char *pString, char *pDest, int nDestSize);

	/* @GetBase64EncodeStringSize
	 * 获取pString字符串编码后的长度
	 * @返回值：如果没有错误发生， 返回编码后的长度， 否则返回-1
	 */
	int GetBase64EncodeStringSize(const char *pString);

	//base64 解码
	/* @Base64EncodeString
	 * 对于ASCII编码的字符串进行BASE64解码
	 * @pBase64String : 需要解码的字符串
	 * @pDest ：对 pBase64String解码之后存储的地址， 长度必须调用GetBase64DecodeString 获取
	 * @nDestSize: pDest的长度
	 * @返回值：如果没有错误发生， 返回em_BASE64_OK.
     */
	emBase64_Err Base64DecodeString(const char *pBase64String, char *pDest, int nDestSize);

	/* @GetBase64DecodeString
	 * 获取pBase64String字符串解码后的长度
	 * @返回值：如果没有错误发生， 返回解码后的长度， 否则返回-1
	 */
	int GetBase64DecodeString(const char *pBase64String);
}

#endif  // BASE_HPP