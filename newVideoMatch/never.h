#ifndef NEVER_H_
#define NEVER_H_

#include <iostream>
#include <fstream>
#include <sstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>
#include <string>
#include <map>
#include <algorithm>

using namespace std;
using namespace cv;

namespace search{

	typedef struct invertedList
	{
		vector<string> videoList;
		map<string, vector<int>> videoDoc;

	}INVERTEDDOC;

	typedef struct Nod 
	{
		node* next;
		int ndepth;
		bool isLeaf;
		INVERTEDDOC* filteredVideos;
		
		Nod()
		{
			ndepth = 0;
			next = NULL;
			isLeaf = false;
			filteredVideos = NULL;
		}
	}node;

	typedef short unsigned int _FEATURE;

	class skyTree{
	private:
		node* lucene;
		int depth;

	public:
		skyTree() { depth = 0;  lucene = NULL; }
		virtual ~skyTree() { lucene = NULL; }

		node* initializeTree();
		bool makeTree();
		node* search(_FEATURE queryFeature);
	};

}

#endif