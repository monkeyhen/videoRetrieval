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
		vector<pair<string, float> > heapK;

	public:
		int reset(const int& k, vector<pair<string, float> >& distance )
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
			bool operator () (pair<string, float>x, pair<string, float> y)
			{
				return x.second > y.second;
			}
		}distCmp;

		int sortHeap(const int& k, vector<pair<string, float> >& distance )
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

			vector<pair<string, float> >::const_iterator it = distance.begin();
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

		vector<pair<string, float> > getTopk()
		{
			return heapK;
		}

		const vector<pair<string, float> > getTopk() const
		{
			return heapK;
		}
	};


	class scanner
	{
	public:
		scanner() {}
		~scanner() {} 

	private:
		topk topPair;




	};
}

#endif
