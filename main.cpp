#include <iostream>
#include <unistd.h>
#include <cstring>
using namespace std;


void    RemoveDotSegments(char *buffer, short length)
{
	short offset = 0;
	short startDote = 0;
	short offsetDir = offset;

	while (offset < length)
	{
		startDote = offset;
		if (buffer[offset] == '.')
		{
			if (offset == 0 || buffer[--offset] == '/') // offset == 1 for this test ../
			{
				startDote += (offset && buffer[offset - 1] == '/');
				if (offset < length + 1 && buffer[1 + offset] == '.')
				{
					offset++;
					while (startDote != offset)
					{					
						buffer[startDote++] = '\0';
					}

					do
					{
						buffer[offsetDir++] = '\0';

					} while (buffer[offsetDir] != '/' && offsetDir < length);
					
					while (offsetDir > 0 && buffer[offsetDir] != '/')
					{
						offsetDir--;

					}
				}
				else if (offset < length + 1 && buffer[1 + offset] == '/')
				{
					buffer[offset++] = '\0';
					buffer[offset] = '\0';
				}
			}
		}

		offset++;
	}

	short numberZero = 0;
	offset = 0;

	while (offset < length)
	{

		if (buffer[offset] == '\0')
			numberZero++;
		else
			buffer[offset - numberZero] = buffer[offset];
		offset++;
	}
}

int main ()
{
	char buffer[100] = "/../.././ab/../a/././b/../a";
	RemoveDotSegments(&buffer[0], strlen(buffer));
}