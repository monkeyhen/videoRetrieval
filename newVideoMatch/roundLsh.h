#ifndef  ROUNDLSH_H_
#define  ROUNDLSH_H_

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <random>
#include "time.h"

using namespace std;
typedef unsigned short int bitname;

namespace lsh
{

	class  rbhLsh
	{
	public:
		typedef struct  
		{
			size_t L;
			size_t N;
			size_t M;
			size_t D;
		} lsh_para;

		typedef struct 
		{
			string dirPath;
			int videoLength;
			bitname* feature;
		} video_node;

	private:
		lsh_para para;
		vector<vector<size_t> > rndPosition;  // hamming feature position 
		vector<vector<size_t> > rndBits;
		vector<map< size_t, vector<video_node> > > tables;

	public:
		rbhLsh() {};
		rbhLsh(const lsh_para& inputPara)
		{
			reset(inputPara);
		}

		~rbhLsh() {};

		void reset(const lsh_para& inputPara)
		{
			para = inputPara;
			tables.resize(para.L);
			rndPosition.resize(para.L);
			rndBits.resize(para.L);

			mt19937 rng(size_t(time(0)));
			uniform_int_distribution<size_t> rngPos(0, para.D-1);

			for (vector<vector<size_t> >::iterator it = rndPosition.begin(); it != rndPosition.end(); it++)
			{
				while (it->size() != para.N)
				{
					size_t target = rngPos(rng);
					if( find(it->begin(); it->end(), target) == it->end())
						it->push_back(target);
				}
			}

			uniform_int_distribution<size_t> rngB(0, para.M - 1);
			for (vector<vector<size_t> >::iterator it = rndBits.begin(); it != rndBits.end(); it++)
			{
				it->resize(para.N);
				for (vector<size_t>::iterator it2 = it->begin(); it2 != it->end(); it2++)
					*it2 = rngB(rng);
			}
		}

		void insert(bitname* key, int num,  string videoname)
		{
			video_node node;
			node.feature = key;
			node.videoLength = num;
			node.dirPath = videoname;

			for (int i = 0; i < para.L; i++)
			{
				size_t sum(0);
				for (int j = 0; j < para.N; j++)
				{
					size_t index = rndPosition[i][j] >> 4;
					size_t pos = rndPosition[i][j] & 0x0F;
					bitname mask = 1 << pos;

					if (key[index]  & mask)
					{
						sum += rndBits[i][j];
					}
				}

				size_t bucket = sum% para.M;
				tables[i][bucket].push_back(node);
			}
		}

		void query(bitname* key, int num, string videoname)
		{
			video_node query;
			query.dirPath = videoname;
			query.videoLength = num;
			query.feature = key;

			for (int i = 0; i < para.L; i++)
			{
				size_t sum(0);
				for (int j = 0; j < para.N; j++)
				{
					size_t index = rndPosition[i][j] >> 4;
					size_t pos = rndPosition[i][j] & 0x0F;
					bitname mask = 1 << pos;

					if (key[index] & mask)
					{
						sum += rndBits[i][j];
					}
				}

				size_t bucket = sum% para.M;

				if (tables[i].find(bucket) != tables[i].end())
				{
					vector<video_node> candidates = tables[i][bucket];
					search_topk_matches(candidates, key, num);
				}
			}
		}

		void search_topk_matches(const vector<video_node>& candidates, bitname* key, const size_t& num)
		{

		}

	};


	
}



#endif