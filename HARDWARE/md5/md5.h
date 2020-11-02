/* Copyright (C)
 * 2017 - Xianlee.Wu  xianleewu@163.com
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#ifndef MD5_H
#define MD5_H

typedef struct
{
  unsigned int count[2];
  unsigned int state[4];
  unsigned char buffer[64];
}MD5_CTX;

#define F(x,y,z) ((x & y) | (~x & z))
#define G(x,y,z) ((x & z) | (y & ~z))
#define H(x,y,z) (x^y^z)
#define I(x,y,z) (y ^ (x | ~z))
#define ROTATE_LEFT(x,n) ((x << n) | (x >> (32-n)))
#define FF(a,b,c,d,x,s,ac) \
{ \
  a += F(b,c,d) + x + ac; \
  a = ROTATE_LEFT(a,s); \
  a += b; \
}
#define GG(a,b,c,d,x,s,ac) \
{ \
  a += G(b,c,d) + x + ac; \
  a = ROTATE_LEFT(a,s); \
  a += b; \
}
#define HH(a,b,c,d,x,s,ac) \
{ \
  a += H(b,c,d) + x + ac; \
  a = ROTATE_LEFT(a,s); \
  a += b; \
}
#define II(a,b,c,d,x,s,ac) \
{ \
  a += I(b,c,d) + x + ac; \
  a = ROTATE_LEFT(a,s); \
  a += b; \
}

void MD5Init(MD5_CTX *context);
void MD5Update(MD5_CTX *context,unsigned char *input,unsigned int inputlen);
void MD5Final(MD5_CTX *context,unsigned char digest[16]);
void MD5Transform(unsigned int state[4],unsigned char block[64]);
void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len);
void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len);

#endif
