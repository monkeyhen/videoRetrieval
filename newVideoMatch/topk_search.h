#ifndef TOPK_SEARCH_H_
#define  TOPK_SEARCH_H_

#include <iostream>
#include <vector>
#include <map>
#include <string>

#include <functional>
#include <algorithm>
#include "basis.h"

namespace lsh
{
	class topk
	{
	public:
		topk() {}
		~topk() {}

	private:
		int num;
		vector<pair<string, float> > distArray;
		vector<pair<uit, float> > heapK;   // float代表两个二值化向量中不同位数在较短的视频上占的比重

	public:
		int reset(const int& k, vector<pair<uit, float> >& distance )
		{
			if (distance.size() < k)
			{
				return 1;
			}

			for (int i = 0; i < k;i++)
			{
				heapK.push_back(distance[i]);
			}
			return 0;
		}

		struct distPair
		{
			bool operator () (pair<uit, float>x, pair<uit, float> y)
			{
				return x.second > y.second;
			}
		}distCmp;

		int sortHeap(const int& k, vector<pair<uit, float> >& distance )
		{
			if (distance.size() < k)
			{
				return 1;
			}

			for (int i = 0; i < k;i++)
			{
				heapK.push_back(distance[i]);
			}

			make_heap(heapK.begin(), heapK.end(), distCmp);

			vector<pair<uit, float> >::const_iterator it = distance.begin();
			while (it!= distance.end())
			{
				if (it->second < heapK.front().second)
				{
					pop_heap(heapK.begin(), heapK.end(), distCmp);
					heapK.pop_back();
					heapK.push_back(*it);
					push_heap(heapK.begin(), heapK.end(), distCmp);
				}
			}

			return 0;
		}

		vector<pair<uit, float> > getTopk()
		{
			return heapK;
		}

		const vector<pair<uit, float> > getTopk() const
		{
			return heapK;
		}

		bool push_back(const pair<uit, float>& candi)
		{
			if (candi.second<heapK.front().second)
			{
				heapK.push_back(candi);
				push_heap(heapK.begin(), heapK.end(), distCmp);
			}
			return true;
		}

		pair<uit, float> pop_back()
		{
			if (heapK.empty())
				return pair<uit, float>(0, 0);
			pair<uit, float> candi;
			candi = heapK.front();
			pop_heap(heapK.begin(), heapK.end(), distCmp);
			heapK.pop_back();
			return candi;
		}
	};

}

#endif
