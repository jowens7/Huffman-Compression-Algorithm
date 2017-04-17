#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

const int MAX_FILE_NAME = 81;

struct huffmanNode
{
	int glyph;
	int lT;
	int rT;
};

void main()
{
	char compressedFileName[MAX_FILE_NAME];
	huffmanNode huffmanTree[513];
	char* tempGlyph;
	unsigned int numberOfNodes;
	unsigned int sizeOfFileName;
	//vector<vector<int>> tree;
	huffmanNode tree[513];
	//vector<char> decompressedFileName;
	char decompressedFileName[MAX_FILE_NAME];
	bool eofFlag = false;
	int bitPos = 0;
	short bitDirection;
	short cur = 0;
	unsigned char decompressedByte;
	unsigned char byte;
	ifstream fin;
	ofstream fout;
	char tempFileName[1000];
	unsigned char  tempOut[100000];
	int tempCount = 0;
	
	cout << "Enter the name of the file to decompress: ";
	cin.getline(compressedFileName, MAX_FILE_NAME);


	fin.open(compressedFileName, ios::in | ios::binary);

	if (fin.fail())
	{
		cout << "File " << compressedFileName << " could not be found, program exiting." << endl;
		exit(0);
	}

	fin.seekg(0);

	int read1 = (sizeof sizeOfFileName);

	fin.read((char*)&sizeOfFileName, read1);

	fin.read((char*)&decompressedFileName, sizeOfFileName);
	decompressedFileName[sizeOfFileName] = '\0';
	fout.open(decompressedFileName, ios::out | ios::binary);
	
	fin.read((char*)&numberOfNodes, 4);

	//need to get number of entries, use that to initialize 2D array

	for (int i = 0; i < numberOfNodes; i++)
	{
		huffmanNode Node;
		fin.read((char*)&Node, 12);
		/*tree[0][i] = Node.glyph;
		tree[1][i] = Node.lT;
		tree[2][i] = Node.rT;*/
		tree[i].glyph = Node.glyph;
		tree[i].lT = Node.lT;
		tree[i].rT = Node.rT;
	}

	//while (have not read all byte keys as binary)
	while (!fin.eof() && eofFlag == false)
	{
		fin.read((char*)&byte, 1);
		while (eofFlag == false && bitPos != 8)
		{
			if (tree[cur].glyph == -1)
			{
				if (byte & (short)pow(2.0, bitPos))
					bitDirection = 1;
				else
					bitDirection = 0;

				if (bitDirection == 0)
				{
					//if its left, then set cur to the index of next node pointed to by left
					cur = tree[cur].lT;
				}
				else
				{
					//if its right, then set cur to the index of next node pointed to by right
					cur = tree[cur].rT;
				}
				bitPos++;
			}
			else
			{
				decompressedByte = tree[cur].glyph;
				if (tree[cur].glyph == 256)
				{
					eofFlag = true;
				}
				else
				{
					fout.write((char*)&decompressedByte, 1);
				}
				cur = 0;
			}
		}
		bitPos = 0;
	}

	system("pause");
}
