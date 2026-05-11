#include <iostream>
#include <unistd.h>
#include <cstring>
using namespace std;


#include <stdbool.h>
#include <stddef.h>

void RemoveDotSegmentsDirect(char *path)
{
    size_t r = 0;
    size_t w = 0;

    while (path[r] != '\0')
    {

		if (path[r] == '/' && path[r + 1] == '/') {
    		r++;
    		continue;
		}

    	if (path[r] == '/' && path[r + 1] == '.' && path[r + 2] == '.' && 
                (path[r + 3] == '/' || path[r + 3] == '\0'))
        {
            r += 3;
            
            if (w > 0)
            {
                w--;
                while (w > 0 && path[w] != '/')
                    w--;
            }
        }
    	if (path[r] == '.' && path[r + 1] == '.'  && 
                (path[r + 2] == '/' || path[r + 2] == '\0'))
        {
            r += 2;
			w = 0;
        }
        else if (path[r] == '/' && path[r + 1] == '.' && (path[r + 2] == '/' || path[r + 2] == '\0'))
            r += 2;
		else if (path[r] == '.' && (path[r + 1] == '/' || path[r + 1] == '\0'))
            r += 1;
        else
            path[w++] = path[r++];
    }

    path[w] = '\0';
}



int main ()
{
	char buffer[100] = "/aa/../b/a/../f";
	cout << buffer << endl;
	RemoveDotSegmentsDirect(&buffer[0]);
	cout << buffer << endl;

	// RemoveDotSegments(&buffer[0], strlen(buffer));
	// cout << buffer << endl;


}