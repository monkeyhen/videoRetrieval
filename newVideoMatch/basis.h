#ifndef  BASIS_H_
#define BASIS_H_	

#include <string>

using namespace std;
typedef short unsigned int bitname;

typedef struct 
{
	string dirPath;
	int videoLength;
	bitname* feature;

} video_node;

#endif