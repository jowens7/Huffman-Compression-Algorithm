#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <list>
#include <ctime>
#include <cmath>

using namespace std;

const int MAX_FILE_NAME = 81;
const int MAX_GLYPHS = 257;
const int MAX_HUFFMAN_NODES = 513;

struct HuffmanTableNode
{
	int glyph;
	int frequency;
	int left;
	int right;
};

struct BitcodeTableNode
{
	int glyph;
	string bitcode;
};

void sortByFrequency(HuffmanTableNode huffmanTable[])
{
	for (int i = 0; i < MAX_GLYPHS; i++)
	{
		int smallestIndex = i;

		for (int j = i + 1; j < MAX_GLYPHS; j++)
		{
			if (huffmanTable[j].frequency < huffmanTable[smallestIndex].frequency)
			{
				smallestIndex = j;
			}
		}
		swap(huffmanTable[i], huffmanTable[smallestIndex]);
	}

	for (int i = 0; i < MAX_GLYPHS; i++)
	{
		if (huffmanTable[i].frequency == INT_MAX)
		{
			huffmanTable[i].frequency = 0;
		}
	}
}

void reheap(HuffmanTableNode huffmanTable[], int cur, const int h)
{
	HuffmanTableNode temp;

	// While left node frequency is less than the current node frequency or the right
	// node frequency is less than the current node frequency
	while (((huffmanTable[2 * cur + 1].frequency < huffmanTable[cur].frequency) ||
		(huffmanTable[2 * cur + 2].frequency < huffmanTable[cur].frequency)) && 
		(cur < MAX_HUFFMAN_NODES))
	{
		if (((huffmanTable[2 * cur + 1].frequency <= huffmanTable[2 * cur + 2].frequency) && ((2 * cur + 1) < h)) ||
			(huffmanTable[2 * cur + 1].frequency < huffmanTable[cur].frequency) && ((2 * cur + 2) == h))
		{
			temp = huffmanTable[2 * cur + 1];
			huffmanTable[2 * cur + 1] = huffmanTable[cur];
			huffmanTable[cur] = temp;
			cur = 2 * cur + 1;
		}
		else if ((2 * cur + 2) < h)
		{
			temp = huffmanTable[2 * cur + 2];
			huffmanTable[2 * cur + 2] = huffmanTable[cur];
			huffmanTable[cur] = temp;
			cur = 2 * cur + 2;
		}
		else
		{
			cur = MAX_HUFFMAN_NODES;
		}
	}
}

int buildHuffmanTable(HuffmanTableNode huffmanTable[])
{
	int m;
	int f;
	int h;
	int numberOfEntries = 0;

	// Initialize the first values of f and h
	if (huffmanTable[MAX_GLYPHS - 1].frequency != 0)
	{
		f = MAX_GLYPHS;
		h = MAX_GLYPHS - 1;
		numberOfEntries = MAX_GLYPHS;
	}
	else
	{
		for (int i = 0; i < MAX_GLYPHS; i++)
		{
			if (huffmanTable[i].frequency == 0)
			{
				f = i;
				h = i - 1;
				numberOfEntries = i;
				i = MAX_GLYPHS;
			}
		}
	}

	while (h > 0)
	{
		// Set m to the lower of relative slots 2 and 3
		if ((huffmanTable[1].frequency <= huffmanTable[2].frequency) || (h == 1))
		{
			m = 1;
		}
		else if (h >= 2)
		{
			m = 2;
		}

		// Move m to the next free slot (f)
		huffmanTable[f] = huffmanTable[m];

		// If m < end of current heap (h), move h to m
		if (m < h)
		{
			huffmanTable[m] = huffmanTable[h];
		}

		// Reheap if necessary
		if ((huffmanTable[2 * m + 1].frequency < huffmanTable[m].frequency) ||
			(huffmanTable[2 * m + 2].frequency < huffmanTable[m].frequency))
		{
			reheap(huffmanTable, m, h);
		}

		// Move lowest frequency node [slot zero] to h
		huffmanTable[h] = huffmanTable[0];

		// Create frequency node at [slot zero]
		huffmanTable[0].glyph = -1;
		huffmanTable[0].frequency = huffmanTable[h].frequency + huffmanTable[f].frequency;
		huffmanTable[0].left = h;
		huffmanTable[0].right = f;

		// Reheap if necessary
		if (((huffmanTable[1].frequency < huffmanTable[0].frequency) ||
			(huffmanTable[2].frequency < huffmanTable[0].frequency)) &&
			(h > 1))
		{
			reheap(huffmanTable, 0, h);
		}

		// Move h and f
		h--;
		f++;
		numberOfEntries++;
	}

	return numberOfEntries;
}

void buildBitcodeTable(HuffmanTableNode huffmanTable[], BitcodeTableNode bitcodeTable[], HuffmanTableNode currentNode)
{
	static string bitcode;
	static int i = 0;

	if (currentNode.left > 0)
	{
		bitcode += "0";
		buildBitcodeTable(huffmanTable, bitcodeTable, huffmanTable[currentNode.left]);
		bitcode.pop_back();
		bitcode += "1";
		buildBitcodeTable(huffmanTable, bitcodeTable, huffmanTable[currentNode.right]);
		bitcode.pop_back();
	}
	else
	{
		bitcodeTable[currentNode.glyph].glyph = currentNode.glyph;
		bitcodeTable[currentNode.glyph].bitcode = bitcode;
		i++;
	}
}

void main()
{
	char inFileName[MAX_FILE_NAME];
	string tempOutFileName;
	char* outFileName;
	char delimiter = '.';
	int lengthOfFileName;
	HuffmanTableNode huffmanTable[MAX_HUFFMAN_NODES];
	BitcodeTableNode bitcodeTable[MAX_GLYPHS];
	unsigned char buffer;
	list<int> bytes;
	int numberOfEntries;

	for (int i = 0; i < MAX_GLYPHS; i++)
	{
		huffmanTable[i].glyph = i;
		huffmanTable[i].frequency = INT_MAX;
		huffmanTable[i].left = -1;
		huffmanTable[i].right = -1;
	}
	// EOF character
	huffmanTable[256].frequency = 1;

	ifstream fin;
	ofstream fout;

	cout << "Enter the name of the file to compress: ";
	cin.getline(inFileName, MAX_FILE_NAME);

	fin.open(inFileName, ios::in | ios::binary);

	if (fin.fail())
	{
		cout << "File " << inFileName << " could not be found, program exiting." << endl;
		exit(0);
	}

	clock_t start, end;
	start = clock();

	fin.read((char*)&buffer, sizeof buffer);
	while (fin.good())
	{
		if (huffmanTable[(int)buffer].frequency == INT_MAX)
		{
			huffmanTable[(int)buffer].frequency = 0;
		}

		huffmanTable[(int)buffer].frequency++;
		bytes.push_back((int)buffer);

		fin.read((char*)&buffer, sizeof buffer);
	}

	bytes.push_back(256);

	sortByFrequency(huffmanTable);

	numberOfEntries = buildHuffmanTable(huffmanTable);

	buildBitcodeTable(huffmanTable, bitcodeTable, huffmanTable[0]);

	lengthOfFileName = strlen(inFileName);

	// Create output file name
	tempOutFileName = inFileName;
	int pos = tempOutFileName.find(delimiter);
	if (pos != string::npos)
	{
		tempOutFileName.erase(pos);
	}
	tempOutFileName.append(".huf");
	outFileName = (char*)tempOutFileName.c_str();

	fout.open(outFileName, ios::out | ios::binary);

	// Write out header
	fout.seekp(0, ios::beg);
	fout.write((char*)&lengthOfFileName, sizeof lengthOfFileName);
	fout.write((char*)inFileName, lengthOfFileName);
	fout.write((char*)&numberOfEntries, sizeof numberOfEntries);
	
	for (int i = 0; i < numberOfEntries; i++)
	{
		fout.write((char*)&huffmanTable[i].glyph, sizeof &huffmanTable[i].glyph);
		fout.write((char*)&huffmanTable[i].left, sizeof &huffmanTable[i].left);
		fout.write((char*)&huffmanTable[i].right, sizeof &huffmanTable[i].right);
	}

	int bitcodeLength;
	int currentByte;
	unsigned char encodedByte = '\0';
	char* currentBitcode;
	int cnt = 0;

	// Encode the file
	while (!bytes.empty())
	{
		currentByte = bytes.front();
		currentBitcode = (char*)bitcodeTable[currentByte].bitcode.c_str();
		bitcodeLength = strlen(currentBitcode);

		if (cnt == 8)
		{
			fout.write((char*)&encodedByte, sizeof encodedByte);
			encodedByte = '\0';
			cnt = 0;
		}
		for (int i = 0; i < bitcodeLength; i++)
		{
			if (cnt == 8)
			{
				fout.write((char*)&encodedByte, sizeof encodedByte);
				encodedByte = '\0';
				cnt = 0;
				fout.flush();
			}

			if (currentBitcode[i] == '1')
			{
				encodedByte = encodedByte | (int)pow(2.0, cnt);
			}

			cnt++;
		}

		bytes.pop_front();
	}

	if (cnt != 8)
	{
		fout.write((char*)&encodedByte, sizeof encodedByte);
	}

	fin.close();
	fout.close();

	end = clock();
	cout << "Time: " << (double(end - start) / CLOCKS_PER_SEC) << " seconds" << endl;
}
