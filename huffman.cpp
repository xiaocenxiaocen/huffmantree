/* A Huffman coding program
 * @file: huffman.cpp
 * @author: xiaocenxiaocen
 * @date: 2016.11.18
 */
#include "huffman.h"

size_t parent(size_t i)
{
	return i / 2;
}

size_t left(size_t i, size_t size)
{
	return min(2 * i + 1, size);
}

size_t right(size_t i, size_t size)
{
	return min(2 * i + 2, size);
}

void huff_heapify(node ** head, size_t i, size_t size)
{
	size_t leftChild = left(i, size);
	size_t rightChild = right(i, size);
	if(leftChild < size) {
		huff_heapify(head, leftChild, size);
		if(head[leftChild]->weight < head[i]->weight) {
			node * swapNodePt = head[leftChild];
			head[leftChild] = head[i];
			head[i] = swapNodePt;
		}
	}
	if(rightChild < size) {
		huff_heapify(head, rightChild, size);
		if(head[rightChild]->weight < head[i]->weight) {
			node * swapNodePt = head[rightChild];
			head[rightChild] = head[i];
			head[i] = swapNodePt;
		}
	}
}

void huff_heapsort(node ** head, size_t size)
{
	for(auto offset = 0; offset < size; offset++) {
		huff_heapify(head + offset, 0, size - offset);
	}
}

node * huff_get_alphabet(const float * wfl, const size_t wflLen)
{
	node * alphabet = new node[n_alphabet];
	for(int i = 0; i < n_alphabet; i++) alphabet[i].a = (char)(i - 128);
	const char * bufIter = (const char *)wfl;
	for(auto i = 0; i < wflLen * sizeof(float); i++) {
		int index = 128 + bufIter[i];
		alphabet[index].weight++;
	}
	return alphabet;
}

node * huff_node_merge(node * left, node * right)
{
	node * parent = new node();
	parent->weight = left->weight + right->weight;
	parent->left = left;
	left->p = parent;
	parent->right = right;
	right->p = parent;
	return parent;
}

node * huff_tree_gen(const float * wfl, const size_t wflLen, vector<vector<uint8_t> > & codeTable)
{
	node * alphabet = huff_get_alphabet(wfl, wflLen);	
#ifdef DEBUG
	for(int i = 0; i < n_alphabet; i++) {
		cout << alphabet[i].weight << "\t" << alphabet[i].a << "\n";
	}
	cout << "\n";
#endif
	node * heap[n_alphabet];
	for(auto i = 0; i < n_alphabet; i++) {
		heap[i] = alphabet + i;
	}
	
	int curLen = n_alphabet;
	node ** head = heap;
	while(curLen >= 2) {
		huff_heapsort(head, curLen);
		node * left = head[0];
		node * right = head[1];
		node * p = huff_node_merge(left, right);
		head[1] = p;
		head++;
		curLen--;
	}

	for(int i = 0; i < n_alphabet; i++) {
		node * curNode = alphabet + i;
		while(curNode->p != NULL) {
			if(curNode == curNode->p->left) {
				codeTable[i].push_back(0);
			} else if(curNode == curNode->p->right) {
				codeTable[i].push_back(1);
			}
			curNode = curNode->p;
		}
		reverse(codeTable[i].begin(), codeTable[i].end());
	}
	return head[0];	
}

void huff_tree_travel(node * huff, string prefix)
{
	if(huff == NULL) return;
	if(huff->left == NULL && huff->right == NULL) {
		cout << prefix << "<freq: " << huff->weight << ">";
		cout << " <char: " << (huff->a) << ">" << endl;
		return;
	}
	cout << prefix << "<freq: " << huff->weight << endl;
	if(huff->left != NULL) {
		huff_tree_travel(huff->left, prefix + "|");
	}
	if(huff->right != NULL) {
		huff_tree_travel(huff->right, prefix + "|");
	}
}

void huff_tree_free(node * huff)
{
	if(huff == NULL) return;
	if(huff->left == NULL && huff->right == NULL) {
		if(huff->a == -128) free(huff);
		return;
	}
	if(huff->left != NULL) huff_tree_free(huff->left);
	if(huff->right != NULL) huff_tree_free(huff->right);
	free(huff); huff = NULL;
	return;
}

inline unsigned char operator_bit(unsigned char in, int pos, int flag)
{
	assert(pos >= 0 && pos < 8);
	if(flag) return in | dwMask[pos];
	else return in & ((~(0x00000000))^dwMask[pos]);
}

inline int get_cur_bit(const unsigned char * in, size_t curbits)
{
	size_t curbts = curbits / 8;
	int pos = curbits - curbts * 8;
//	cout << (int)(in[0]) << " " << (int)(in[1]) << " " << (int)(in[2]) << " " << (int)(in[3]) << "\n";
//	cout << (int)(dwMask[7 - pos]) << "\n";
	return ((in[curbts] & dwMask[7 - pos]) == dwMask[7 - pos]);
}

//FILE * f1;
//FILE * f2;

size_t compressor(const float * wfl, const int wflLen, const vector<vector<uint8_t> > & codeTable, unsigned char ** stream)
{
	size_t lenbits = 0;
	char * iter = (char*)(wfl);
	for(auto idx = 0; idx < wflLen * sizeof(float); idx++) {
		int i = (*iter) + 128;
		lenbits += codeTable[i].size();
		iter++;
	}
	size_t len = lenbits / 8 + (lenbits % 8 != 0);
	*stream = (unsigned char *)malloc(sizeof(unsigned char) * len);
	memset(*stream, 0, sizeof(unsigned char) * len);
	size_t curBits = 0;
	size_t curBts = 0;
//	f1 = fopen("comp.txt", "w");
	for(auto idx = 0; idx < sizeof(float) * wflLen; idx++) {
		const int i = (int)(*((char*)(wfl) + idx) + 128);
		for(int j = 0; j < codeTable[i].size(); j++) {
//			fprintf(f1, "%d", (int)(codeTable[i][j]));
			int pos = curBits - curBts * 8;
			(*stream)[curBts] = operator_bit((*stream)[curBts], 7 - pos, codeTable[i][j]);
			curBits++;
			curBts = curBits / 8;
		}
//		fprintf(f1, "\t%d", i);
//		fprintf(f1, "\n");
	}
//	fclose(f1);
	return len;
}

void decompressor(const unsigned char * stream, const size_t lenbts, const node * huff, const size_t wflLen, float * wfl)
{
	size_t curbts = 0;
	size_t curbits = 0;
	size_t offset = 0;
	char * iter = (char*)wfl;
//	f2 = fopen("decomp.txt", "w");
	while(curbts < lenbts && offset < 4 * wflLen) {
		node * curNode = const_cast<node*>(huff);
		while(curNode->left != NULL || curNode->right != NULL) {
			if(get_cur_bit(stream, curbits)) {
				curNode = curNode->right;
//				fprintf(f2, "%d", 1);
			} else {
				curNode = curNode->left;
//				fprintf(f2, "%d", 0);
			}
			curbits++;
			curbts = curbits / 8;
		}
		iter[offset] = curNode->a;
//		fprintf(f2, "\t%d", (int)(iter[offset] + 128));
//		fprintf(f2, "\n");
		offset++;
	}
//	fclose(f2);
}

void WflToBin(const char * fileName, const float * wfl, const int length)
{
	FILE * fp;
	if((fp = fopen(const_cast<char*>(fileName), "wb")) == NULL) {
		fprintf(stderr, "cannot open data file!\n");
	} 
	fwrite(wfl, sizeof(float), length, fp);
	fclose(fp);
	return;
}

void UCharToBin(const char * fileName, const unsigned char * stream, const int length)
{
	FILE * fp;
	if((fp = fopen(const_cast<char*>(fileName), "wb")) == NULL) {
		fprintf(stderr, "cannot open data file!\n");
	} 
	fwrite(stream, sizeof(unsigned char), length, fp);
	fclose(fp);
	return;
}

double test_valid(const float * wfl, const float * wfl_reconstr, size_t wflLen)
{
	double err = 0.0;
	int errIdx = 0;
	for(auto i = 0; i < wflLen; i++) {
		double curErr = abs(wfl[i] - wfl_reconstr[i]);
		if(curErr > err) {
			err = curErr;
			errIdx = i;
		}
	}
	cout << errIdx << "\n";
	return err;
}

int main(int argc, char * argv[])
{
	const int nx = 1032;
	const int ny = 1;
	const int nz = 1032;
	
	float * wfl = (float*)malloc(sizeof(float) * nx * ny * nz);
	const size_t nxyz = nx * ny * nz;
//	srand((unsigned)time(NULL));
//	for(auto idx = 0; idx < nxyz; idx++) {
//		wfl[idx] = (float)(rand()) / RAND_MAX;
//		wfl[idx] = 121;
//	}
	FILE * fp = fopen("wfl_gpu.dat", "rb");
	fread(wfl, sizeof(float), nxyz, fp);
	fclose(fp);

	vector<vector<uint8_t> > codeTable(256, vector<uint8_t>());	
	node * huff = huff_tree_gen(wfl, nxyz, codeTable);
	if(huff == NULL) {
		cerr << "generate huffman tree failed.\n";
	}
	for(int i = 0; i < codeTable.size(); i++) {
		cout << (char)(i - 128) << "\t";
		for(int c = 0; c < codeTable[i].size(); c++) {
			cout << (int)(codeTable[i][c]);
		}
		node * curNode = huff;
		int idx = 0;
		while(curNode->left != NULL && curNode->right != NULL) {
			if(codeTable[i][idx] == 0) { curNode = curNode->left; idx++; }
			else if(codeTable[i][idx] == 1) { curNode = curNode->right; idx++; }
		}
		cout << "\t : " << curNode->a;
		cout << "\n";
	}	
	char * iter = (char*)wfl;
	size_t len = 0;
	for(auto idx = 0; idx < nxyz * sizeof(float); idx++) {
		int i = (*iter) + 128;
		len += codeTable[i].size();
		iter++;
	}
	cout << "bytes after compress: " << len / 8.0 << " bytes.\n";	
	cout << "bytes before compress: " << 1.0 * nxyz * sizeof(float) << " bytes.\n";

	huff_tree_travel(huff, "");

	unsigned char * stream;
	size_t bts = compressor(wfl, nxyz, codeTable, &stream);
	UCharToBin("stream_huff.dat", stream, bts);
	cout << "bytes after compress: " << bts << " bytes.\n";
	
	float * wfl_reconstr = (float*)malloc(sizeof(float) * nxyz);
	memset(wfl_reconstr, 0, sizeof(float) * nxyz);
	decompressor(stream, bts, huff, nxyz, wfl_reconstr);

	double err = test_valid(wfl, wfl_reconstr, nxyz);
	cout << "INFO: reconstruction error: " << err << "\n";

	WflToBin("wfl_reconstr.dat", wfl_reconstr, nxyz);	
	
	huff_tree_free(huff);
	
	free(wfl); wfl = NULL;
	free(stream); stream = NULL;	
	free(wfl_reconstr); wfl_reconstr = NULL;

	return 0;		
}
