/*
* @Author: Yacent
* @Date: 2015-10-27
*/

#include<iostream>
#include<string>
#include<memory.h>

using namespace std;

// 定义 F G H I  
#define shift(x, n) (((x) << (n)) | ((x) >> (32 - (n)))) // 右移的时候，高位一定要补0，而不是补充符号位
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
		// 一个小块32bit，每次处理一个字节，所以循环4次 
		temp = "";
		/*
		* 1 << 8 = 2^8 = 256，对256取模，即取32bit的低8位
		* &0xff 即把 除低8位外的所有位都置0，得低8位的值
		* 逆序处理每个字节，因存储方式和显示方式相反(大小端关系)
		*/
		hexNum = ((num >> i * 8) % (1 << 8)) & 0xff;

		/*
		* 因为max(hexNum) 255，故转换为hex只需要两次循环即可
		* tmp.insert(0, 1, str16[hexNum % 16]) 意为 在tmp的第0个位置插入一个str[hexNum%16]
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
	// 一种移位的方法，参考自 百度百科(MD5) 
	const unsigned s[64] = {
		7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
		5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
		4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
		6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
	};

	//  ti 是 4294967296*abs(sin(i))的整数部分(1≤i≤64, 2^32=4294967296)
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

	// 初始化变量 链接变量 
	unsigned int A = 0x67452301;
	unsigned int B = 0xefcdab89;
	unsigned int C = 0x98badcfe;
	unsigned int D = 0x10325476;

	int strLength = str.length();
	/*
	* 填充：遵循 bit lengths = 448 (mod 512)
	* 在原始str后，填充一个1和无数个0，直至满足上面一个条件
	* +8 是因为最后的64bit是用于放置str长度信息
	* +1 是因为至少有一个大块存放512bits
	*/
	unsigned int blockNum = ((strLength + 8) / 64) + 1; // 计算需要多少个大块
	/*
	* 填充后的字符串长度为512bits，分为16个小块，每小块32bits
	*/
	unsigned int *strByte = new unsigned int[blockNum * 16];
	memset(strByte, 0, sizeof(strByte) * blockNum * 16);

	for (unsigned int i = 0; i < strLength; i++) {
		/*
		* 传入的str，其中str[i]表示一个char = 8 bits
		* 而strByte[i]的大小为32bits
		* 故对于传入字符串的处理str的前4个char都放进同一个小块当中
		* 0>>2  1>>2  2>>2  3>>2 结果都为0，即前4位都存储在strByte[0]当中
		* << ((i%4)*8) 是实现从低到高进行存储
		*/
		strByte[i >> 2] |= (str[i]) << ((i % 4) * 8);
	}

	/*
	* 填充规则：在str后，填充一个1和无数个0，即添加一个0x80 -> 10000000
	* strLength >> 2是计算str可以占多少个小块，则0x80就跟在最后一个char的后面
	* (strLength % 4 * 8) 计算0x80应该存在于每个小块的哪个位置
	* [][][0x80][str.end()] 大端方式
	*/
	strByte[strLength >> 2] |= 0x80 << ((strLength % 4) * 8);

	/*
	* 低64位 存储 信息填充前 信息的长度
	* str 一个char = 8 bits，故 str.length()*8 即总长度*/
	strByte[blockNum * 16 - 2] = strLength * 8;

	/*
	* 对每一个的32bits的小块进行处理
	* FF(a ,b ,c ,d ,Mj ,s ,ti ) 操作为 a = b + ( (a + F(b,c,d) + Mj + ti) << s)
	* GG(a ,b ,c ,d ,Mj ,s ,ti ) 操作为 a = b + ( (a + G(b,c,d) + Mj + ti) << s)
	* HH(a ,b ,c ,d ,Mj ,s ,ti) 操作为 a = b + ( (a + H(b,c,d) + Mj + ti) << s)
	* II(a ,b ,c ,d ,Mj ,s ,ti) 操作为 a = b + ( (a + I(b,c,d) + Mj + ti) << s)
	* 每轮有16次的操作 每次操作对a b c 和 d  中其中三个作一次非线性函数运算
	* 将结果加上第四个变量，文本的一个子分组 Mj和一个常数 ti(即k[i])
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

	// 将结果进行级联
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

