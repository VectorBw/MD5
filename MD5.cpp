/*
* @Author: Yacent
* @Date: 2015-10-27
*/

#include<iostream>
#include<string>
#include<memory.h>

using namespace std;

// ���� F G H I  
#define shift(x, n) (((x) << (n)) | ((x) >> (32 - (n)))) // ���Ƶ�ʱ�򣬸�λһ��Ҫ��0�������ǲ������λ
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

string convertToHex(int num) {
	const char str16[] = "0123456789abcdef";
	int hexNum;

	string temp = "";

	string hexString = "";

	for (int i = 0; i < 4; i++) {
		// һ��С��32bit��ÿ�δ���һ���ֽڣ�����ѭ��4�� 
		temp = "";
		/*
		* 1 << 8 = 2^8 = 256����256ȡģ����ȡ32bit�ĵ�8λ
		* &0xff ���� ����8λ�������λ����0���õ�8λ��ֵ
		* ������ÿ���ֽڣ���洢��ʽ����ʾ��ʽ�෴(��С�˹�ϵ)
		*/
		hexNum = ((num >> i * 8) % (1 << 8)) & 0xff;

		/*
		* ��Ϊmax(hexNum) 255����ת��Ϊhexֻ��Ҫ����ѭ������
		* tmp.insert(0, 1, str16[hexNum % 16]) ��Ϊ ��tmp�ĵ�0��λ�ò���һ��str[hexNum%16]
		*/
		for (int j = 0; j < 2; j++) {
			temp.insert(0, 1, str16[hexNum % 16]);
			hexNum = hexNum / 16;
		}

		hexString += temp;
	}
	return hexString;
}

string getMD5(string str) {
	// һ����λ�ķ������ο��� �ٶȰٿ�(MD5) 
	const unsigned s[64] = {
		7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
		5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
		4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
		6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
	};

	//  ti �� 4294967296*abs(sin(i))����������(1��i��64, 2^32=4294967296)
	const unsigned k[64] = {
		0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
		0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
		0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
		0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
		0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
		0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
		0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
		0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
		0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
		0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
		0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
		0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
		0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
		0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
		0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
		0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
	};

	// ��ʼ������ ���ӱ��� 
	unsigned int A = 0x67452301;
	unsigned int B = 0xefcdab89;
	unsigned int C = 0x98badcfe;
	unsigned int D = 0x10325476;

	int strLength = str.length();
	/*
	* ��䣺��ѭ bit lengths = 448 (mod 512)
	* ��ԭʼstr�����һ��1��������0��ֱ����������һ������
	* +8 ����Ϊ����64bit�����ڷ���str������Ϣ
	* +1 ����Ϊ������һ�������512bits
	*/
	unsigned int blockNum = ((strLength + 8) / 64) + 1; // ������Ҫ���ٸ����
	/*
	* ������ַ�������Ϊ512bits����Ϊ16��С�飬ÿС��32bits
	*/
	unsigned int *strByte = new unsigned int[blockNum * 16];
	memset(strByte, 0, sizeof(strByte) * blockNum * 16);

	for (unsigned int i = 0; i < strLength; i++) {
		/*
		* �����str������str[i]��ʾһ��char = 8 bits
		* ��strByte[i]�Ĵ�СΪ32bits
		* �ʶ��ڴ����ַ����Ĵ���str��ǰ4��char���Ž�ͬһ��С�鵱��
		* 0>>2  1>>2  2>>2  3>>2 �����Ϊ0����ǰ4λ���洢��strByte[0]����
		* << ((i%4)*8) ��ʵ�ִӵ͵��߽��д洢
		*/
		strByte[i >> 2] |= (str[i]) << ((i % 4) * 8);
	}

	/*
	* ��������str�����һ��1��������0�������һ��0x80 -> 10000000
	* strLength >> 2�Ǽ���str����ռ���ٸ�С�飬��0x80�͸������һ��char�ĺ���
	* (strLength % 4 * 8) ����0x80Ӧ�ô�����ÿ��С����ĸ�λ��
	* [][][0x80][str.end()] ��˷�ʽ
	*/
	strByte[strLength >> 2] |= 0x80 << ((strLength % 4) * 8);

	/*
	* ��64λ �洢 ��Ϣ���ǰ ��Ϣ�ĳ���
	* str һ��char = 8 bits���� str.length()*8 ���ܳ���*/
	strByte[blockNum * 16 - 2] = strLength * 8;

	/*
	* ��ÿһ����32bits��С����д���
	* FF(a ,b ,c ,d ,Mj ,s ,ti ) ����Ϊ a = b + ( (a + F(b,c,d) + Mj + ti) << s)
	* GG(a ,b ,c ,d ,Mj ,s ,ti ) ����Ϊ a = b + ( (a + G(b,c,d) + Mj + ti) << s)
	* HH(a ,b ,c ,d ,Mj ,s ,ti) ����Ϊ a = b + ( (a + H(b,c,d) + Mj + ti) << s)
	* II(a ,b ,c ,d ,Mj ,s ,ti) ����Ϊ a = b + ( (a + I(b,c,d) + Mj + ti) << s)
	* ÿ����16�εĲ��� ÿ�β�����a b c �� d  ������������һ�η����Ժ�������
	* ��������ϵ��ĸ��������ı���һ���ӷ��� Mj��һ������ ti(��k[i])
	*/

	for (unsigned int i = 0; i < blockNum; i++) {
		unsigned int a = A;
		unsigned int b = B;
		unsigned int c = C;
		unsigned int d = D;
		unsigned int f, g; // g for choose the Mj

		for (unsigned int j = 0; j < 64; j++) {
			if (j < 16) {
				f = F(b, c, d);
				g = j;
			} else if (j < 32) {
				f = G(b, c, d);
				g = (5 * j + 1) % 16;
			} else if (j < 48) {
				f = H(b, c, d);
				g = (3 * j + 5) % 16;
			} else {
				f = I(b, c, d);
				g = (7 * j) % 16;
			}
			unsigned tmpD = d;
			d = c;
			c = b;
			b = b + shift((a + f + k[j] + strByte[i * 16 + g]), s[j]);
			a = tmpD;
		}

		A = a + A;
		B = b + B;
		C = c + C;
		D = d + D;
	}

	// ��������м���
	return convertToHex(A) + convertToHex(B) + convertToHex(C) + convertToHex(D);
}

void recursiveInput() {
	string ss;
	cout << "Please input the plaintext: ";
	cin >> ss;
	cout << "Ur plaintext is: " + ss << endl;
	string s = getMD5(ss);
	cout << "There is your MD5: " + s << endl;

	string again;
	cout << "U like to have another try? (Y/N)";
	cin >> again;
	if (again == "y" || again == "Y") {
		cout << endl;
		recursiveInput();
	}
}

int main() {
    recursiveInput();
	return 0; 
} 

