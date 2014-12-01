#ifndef SCANNER_H_H
#define SCANNER_H_H

#include "topk_search.h"
namespace lsh{

	class scanner
	{
	public:
		scanner() { queryKey = 0; queryFeature = NULL; }
		~scanner() {}

	private:
		topk topPair;
		map<uit, int>  record;
		videoMat src;
		uit queryKey;
		bitname* queryFeature;
		int queryLength;
		string queryFilename;
		int k;

	public:

		unsigned short int hammingDist(short unsigned int x, short unsigned int y)
		{
			short unsigned int table[256] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
				1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
				1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
				2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8 };
			short unsigned int diff = x ^ y;
			short unsigned int result = 0;
			result = table[diff >> 8] + table[diff & 0xFF];
			return result;
		}

		// note that here the length of each array must be in order, the length of p must be shorter than q
		int diff(unsigned short int *p, unsigned short int* q, int l, int r, int& startIndex)
		{
			if (l > r)
				return -1;

			int sum = 10000;
			for (int i = 0; i <= r - l; i++)
			{
				int tmpSum = 0;
				for (int j = 0; j < l; j++)
				{
					//int tmp = hammingDist(p[j], q[j]);
					tmpSum += hammingDist(p[j], q[j]);
				}
				if (tmpSum < sum)
				{
					sum = tmpSum;
					startIndex = i;
				}
			}
			return sum;
		}

		void init(const videoMat& videoData, int k_)
		{
			src = videoData;
			k = k_;
		}

		void reset(uit i)
		{
			queryKey = i;
			queryFeature = src[i].feature;
			queryLength = src[i].videoLength;
			queryFilename = src[i].dirPath;
		}

		void reset(const video_node& queryvideo)
		{
			queryFeature = queryvideo.feature;
			queryLength = queryvideo.videoLength;
			queryFilename = queryvideo.dirPath;
		}

		int insertKey(uit candi)
		{
			if (record.find(candi) != record.end())
			{
				record[candi] += 1;
				return 1;
			}
			else
			{
				record[candi] = 1;
				return 0;
			}
		}

		bool isExist(uit candi)
		{
			if (record.find(candi) != record.end())
				return true;
			else
				return false;
		}


		bool push(uit key)
		{
			if (insertKey(key))
				return false;
			else
			{
				int candiLength = src[key].videoLength;
				bitname* candiFeature = src[key].feature;
				float percentage = 0.f;
				int diffValue = 0;
				int mIndex = 0;
				if (candiLength < queryLength)
				{
					diffValue = diff(candiFeature, queryFeature, candiLength, queryLength, mIndex);
					percentage = diffValue / ((candiLength << 4) + 0.001);
				}
				else
				{
					diffValue = diff(queryFeature, candiFeature, queryLength, candiLength, mIndex);
					percentage = diffValue / ((queryLength << 4) + 0.001);
				}

				pair<uit, float> nearCandi(key, percentage);
				if (nearCandi.second<topPair.getTopk().front().second || topPair.getTopk().size()<k)
				{
					topPair.push_back(nearCandi);
				}
				return true;
			}
		}

	};

	class indexMentor
	{
	private:
		uit index;
	public:
		indexMentor() { index = 0; }
		uit generate()
		{
			return index++;
		}
	};
}


#endif
