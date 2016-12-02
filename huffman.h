#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <string>
#include <cstring>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <omp.h>

using namespace std;
static const int n_alphabet = 256;

static const unsigned char dwMask[] = {0x00000001, 0x00000002, 0x00000004, 0x00000008,
				       0x00000010, 0x00000020, 0x00000040, 0x00000080};

/* @brief: set bit value
 */
inline unsigned char operator_bit(unsigned char in, int pos, int flag);

/* @brief: get bit value at current position
 */
inline int get_cur_bit(const unsigned char * in, size_t curbits);

struct node {
	node * p;
	node * left;
	node * right;
	int weight;
	char a;
	node() : weight(0), p(NULL), left(NULL), right(NULL) { };
}; 

/* @brief: get parent of current node in a priority queue(heap)
 * @param: i, index of current node
 * @retval: index of parent node
 */
size_t parent(size_t i);

/* @brief: get left child of current node in a priority queue(heap)
 * @param: i, index of current node
 * @param: size, size of the heap
 * @retval: index of left child node
 */
size_t left(size_t i, size_t size);

/* @brief: get right child of current node in a priority queue(heap)
 * @param: i, index of current node
 * @param: size, size of the heap
 * @retval: index of right child node
 */
size_t right(size_t i, size_t size);

/* @brief: build the heap
 * @param: head, huffman tree node array
 * @param: i, index of root node of sub binary tree
 * @param: size, size of heap
 * @retval: head, the heap
 */
void huff_heapify(node ** head, size_t i, size_t size);

/* @brief: heap sort
 * @param: head, huffman tree node array
 * @param: size, size of heap
 * @retval: head, sorted tree node array
 */
void huff_heapsort(node ** head, size_t size);

/* @brief: get all the symbols in alphabet and count the frequency of each symbol
 * @param: wfl, input binary data to be compressed
 * @param: wflLen, size of input data
 * @retval: first node of the alphabet array
 */
node * huff_get_alphabet(const float * wfl, const size_t wflLen);

/* @brief: merge two node and create a parent whose child are the two node
 * @param: left, left child node to be merged
 * @param: right, right child node to be merged
 * @retval: a parent node
 */
node * huff_node_merge(node * left, node * right);

/* @brief: build the huffman tree
 * @param: wfl, input binary data
 * @param: wflLen, size fo input data
 * @param: codeTable, the huffman code of each symbol
 * @retval: root node of the huffman tree
 */
node * huff_tree_gen(const float * wfl, const size_t wflLen, vector<vector<uint8_t> > & codeTable);

/* @brief: travel a huffman tree, output information of the huffman tree to standard output stream
 * @param: huff, root node of the huffman tree
 * @param: prefix, prefix string of each node
 */
void huff_tree_travel(node * huff, string prefix);

/* @brief: free memory of the huffman tree node
 * @param: huff, root node of the huffman tree
 */
void huff_tree_free(node * huff);

/* @brief: huffman tree compressor, encoder
 * @param: wfl, input binary data
 * @param: wflLen, size of input data
 * @param: codeTable, huffman code of each symbols
 * @retval: stream, compressed bit stream
 * @retval: size in bytes of the compressed stream
 */
size_t compressor(const float * wfl, const int wflLen, const vector<vector<uint8_t> > & codeTable, unsigned char ** stream);

/* @brief: huffman tree decompressor, decoder
 * @param: stream, compressed bit stream
 * @param: size in bytes of the compressed stream
 * @param: huff, root node of huffman tree
 * @param: wflLen, size of input data
 * @retval: wfl, output reconstructed binary data
 */
void decompressor(const unsigned char * stream, const size_t lenbts, const node * huff, const size_t wflLen, float * wfl);

void WflToBin(const char * fileName, const float * wfl, const int length);

void UCharToBin(const char * fileName, const unsigned char * stream, const int length);

double test_valid(const float * wfl, const float * wfl_reconstr, size_t wflLen);

#endif
