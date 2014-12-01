#ifndef  BASIS_H_
#define BASIS_H_	

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>

using namespace std;
typedef short unsigned int bitname;
typedef unsigned int uit; // uit: 2-42*10^8, it is used to index videos in database

namespace lsh
{
typedef struct info_node 
{
	uit key;
	string dirPath; //don't use string as a key,it may reduce the 'map' efficiency, and make it confused when two different videos have the same filename
	int videoLength;
	bitname* feature;
	info_node()
	{
		key = 0;
		dirPath = "";
		videoLength = 0;
		feature = NULL;
	}
} info_node;

typedef struct video_node 
{
	string dirPath; //don't use string as a key,it may reduce the 'map' efficiency, and make it confused when two different videos have the same filename
	int videoLength;
	bitname* feature;
	video_node()
	{
		dirPath = "";
		videoLength =0;
		feature = NULL;
	}
} video_node;

class videoMat
{
private:
	map<uit, info_node> videoData;
	int  num;

public:
	videoMat() {}

	info_node operator[] (uit key)
	{
		return videoData[key];
	}

	info_node operator[] (uit key) const
	{
		return videoData[key];
	}

	int getLength(uit key)
	{
		return videoData.size();
	}

	bitname* splitString2(const string& text,
		const string& delimiter,
		int& num)
	{
		bitname* parts = new bitname[400000];
		size_t delimiterPos = text.find(delimiter);
		size_t lastPos = 0;
		if (delimiterPos == string::npos)
		{
			return NULL;
		}

		num = 0;
		while (delimiterPos != string::npos)
		{
			//		parts.push_back(text.substr(lastPos, delimiterPos - lastPos));
			parts[num] = static_cast<bitname> (atoi(text.substr(lastPos, delimiterPos - lastPos).c_str()));
			lastPos = delimiterPos + delimiter.size();
			delimiterPos = text.find(delimiter, lastPos);
			num++;
		}

		return parts;
	}

	int load(string filename)
	{
		ifstream ins("features", ios::in);
		std::string midStr;
		uit referIndex = 0;
		while (getline(cin, midStr))
		{
			int num = 0;
			info_node tmpNode;

			if (midStr.empty())
			{
				cerr << "split_error!" << endl;
				return 1;
			}
			size_t delimit1 = midStr.find("\t");
			string tmpFilename = midStr.substr(0, delimit1);
			size_t delimit2 = midStr.find("\t", delimit1 + sizeof("\t"));
			int length = atoi(midStr.substr(delimit1 + sizeof("\t") - 1, delimit2 - delimit1 - sizeof("\t") + 1).c_str());
			size_t delimit3 = midStr.find("\t", delimit2 + sizeof("\t"));
			string tmpStr(midStr.begin() + delimit3 + sizeof("\t") - 1, midStr.end());
			bitname* content;
			content = splitString2(tmpStr, ",", num);

			tmpNode.key = referIndex;
			tmpNode.dirPath = tmpFilename;
			tmpNode.videoLength = length;
			tmpNode.feature = content;
			videoData[referIndex] = tmpNode;

			referIndex++;
			midStr.clear();
		}

		return 0;
	}

};
}

#endif
