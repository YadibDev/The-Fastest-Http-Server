#include <iostream>
#include <unistd.h>
#include <cstring>
using namespace std;


void    RemoveDotSegments(char *buffer, short length)
{
	short offset = 0;
	short startDote = 0;
	short offsetDir = offset;
	short tmpOffsetDir = offset;
	bool	newFolder = false;

	while (offset < length)
	{
		startDote = offset;
		if (buffer[offset] == '.')
		{
			if (offset == 0 || buffer[offset - 1] == '/') // offset == 1 for this test ../
			{
				startDote -= (offset && buffer[offset - 1] == '/');
				if (offset < length + 1 && buffer[1 + offset] == '.')
				{
					offset += 2;
					while (startDote != offset)
						buffer[startDote++] = '\0';

					do
					{
						buffer[offsetDir++] = '\0';

					} while ((buffer[offsetDir] != '/' && buffer[offsetDir] != '\0') && offsetDir < length);
					
					while (offsetDir > 0 && buffer[offsetDir] != '/')
						offsetDir--;
				}
				else if (offset < length + 1 && buffer[1 + offset] == '/')
				{
					buffer[offset++] = '\0';
					buffer[offset] = '\0';
				}
			}
		}
		if (buffer[offset] = '/')
		{
			if (newFolder)
			{
				newFolder = false;
				offsetDir = tmpOffsetDir;
			}
			else
				newFolder = true;
			tmpOffsetDir = offset;
		}
		

		offset++;
	}

	short numberZero = 0;
	offset = 0;

	for (int i = 0; i < length; i++)
	{
		if (buffer[i] == '\0')
			std::cout << "\\0";
		else
			std::cout << buffer[i];
	}
	std::cout << endl;

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
	char buffer[100] = "as/.././a/../a/./../b/c/../e";
	cout << buffer << endl;
	RemoveDotSegments(&buffer[0], strlen(buffer));
	cout << buffer << endl;

}