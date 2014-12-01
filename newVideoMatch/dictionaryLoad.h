//////////////////////////////////////////////////////////////////////////
// 从硬盘中读取特征数据
// 没用使用矩阵来保存数据，而是map结构进行存储，主要是由于每个视频的帧数不一样，造成其特征总长不一样
//////////////////////////////////////////////////////////////////////////

#ifndef  DICTIONARY_LOAD_H_
#define DICTIONARY_LOAD_H_ 

#include "basis.h"
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>

using namespace std;
typedef short unsigned int bitname;

class dictionary
{
private:
	map<int, pair<bitname*, int > > features;
	int videoNumber;

public:
	dictionary() { videoNumber = 0; }
	~dictionary() {};

	bool empty()
	{
		return videoNumber ? false : true;
	}

	int getSize()
	{
		return features.size();
	}

	bitname* cvtFeature(const string& videoname)
	{
		try
		{
			if (features[videoname].first = NULL)
				throw 1;

			return features[videoname].first;
		}
		catch (int e)
		{
			return NULL;
		}
	}

	int cvtNode(const string& videoname, video_node& video_info)
	{
		try
		{
			if (features[videoname].first == NULL)
			{
				throw 1;
			}

			video_info.dirPath = videoname;
			video_info.feature = features[videoname].first;
			video_info.videoLength = features[videoname].second;
			return 0;
		}
		catch (int e)
		{
			return -1;
		}
	}

	bitname* splitString( const string& text, 
					const string& delimiter,
					int& num )
	{
		bitname* parts = new bitname[400000];
		size_t delimiterPos = text.find(delimiter);
		size_t lastPos = 0;
		if (delimiterPos == string::npos)
		{
			return NULL;
		}

		num = 0;
		while(delimiterPos != string::npos)
		{
			parts[num] = static_cast<bitname> (atoi(text.substr(lastPos, delimiterPos - lastPos).c_str() ));
			lastPos = delimiterPos + delimiter.size();
			delimiterPos = text.find(delimiter, lastPos);
			num++;
		}

		return parts;
	}

	pair<bitname*, int> operator [] (const string videoname)
	{
		return features[videoname];
	}

	const pair<bitname*, int> operator [] (const string videoname) const
	{
		return features[videoname];
	}

	int loadDictionary()
	{
		fstream ins("./log/x.log", ios::in);
		std::string midStr;                                                                                                                                                                           
		int referIndex = 0;
		while(getline(ins, midStr))
		{
			bitname* content ;
			int num = 0;
			if(midStr.empty())                                                                       
			{	
				cerr<<"split_error!"<<endl;
				return 1;                                                                            
			}
			size_t delimit1 = midStr.find("\t");
			size_t delimit2 = midStr.find("\t", delimit1+sizeof("\t"));
			string tmpFilename = midStr.substr(0, delimit1);
			int tmpNum = atoi(midStr.substr(delimit1 + sizeof("\t") - 1, delimit2 - delimit1 - sizeof("\t") + 1).c_str());
			string tmpStr(midStr.begin() + delimit2 + sizeof("\t")-1, midStr.end());

			content = splitString(tmpStr, ",", num);

			pair<bitname*, int> tmpPair(content, num);
			//cout<<tmpPair.first[10]<<"\t" << num<<endl;
			features[tmpFilename] = tmpPair;
			//features.insert(pair<string, pair<bitname*,int> > (tmpFilename, pair<bitname*, int>(content, num)));

			midStr.clear();
		}

		videoNumber = features.size();
		return 0;
	}

};

//////////////////////////////////////////////////////////////////////////
/// comparer的主要作用在于对不同的hashtable里的候选视频进行匹配时，记录是否已经匹配过，避免做重复的匹配运算
//////////////////////////////////////////////////////////////////////////
class comparer
{
public:
	comparer() {};
	comparer(const dictionary& _dict)
	{
		dict = _dict;
	}

	~comparer() {}

	bool mark(const string& videoname)
	{
		if (flags[videoname])
			return false;
		else
			flags[videoname] = true;
		return true;
	}

	bitname* grabFeature(const string& videoname,  int& num)
	{
		num = dict[videoname].second;
		return feature;
	}

	int grabFeatureVec(const string& videoname, int& num, vector<bitname>& feature)
	{
		feature.reserve(40000);
		num = dict[videoname].second;
		feature.assign(dict[videoname].first, dict[videoname].first + num);
		return 0;
	}

private:
	map<string, bool> flags;
	dictionary dict;
};

#endif