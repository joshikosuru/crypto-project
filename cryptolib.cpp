#include <sys/random.h>
#include "cryptolib.h"
#include <iostream>
#include <vector>
#include <random>
using namespace std;

//	BEGIN bool_xor
/**
	Input  : Two bool values\n
	Output : XOR of the two input bool variables
*/
bool bool_xor(bool a, bool b){
	return (a && !b) || (!a && b);
}
/*
	END bool_xor
*/


/**
	 Takes an array of byte-packed 'bits'
	of length 'bytes' bytes, and returns the indexed 'bit'.	
*/
bool get_bit(unsigned char *bits, unsigned bit){
	unsigned byte_index = bit / 8;
	unsigned bit_index = bit % 8;
	unsigned mask = 1 << bit_index;
	return (bits[byte_index] & mask) != 0;
}


/**
	Converts char* to boolean vector
*/
void char_to_vector_bit(vector<bool> &dest, unsigned char *src, size_t size)
{
	for (int i = 0; i < size; i++)
	{
		dest[i] = get_bit(src, i);
	}
}


//BEGIN TRNG
/**
   Truly random number generator uses /dev/random file to get system entropy 
*/
vector<bool> TRNG(size_t size)
{
	vector<bool> x(size);
	size_t sz = (size + 7) / 8;
	unsigned char buf[sz];
	size_t curr = 0;
	while (curr < sz)
	{
		curr += getrandom(buf + curr, sz - curr, GRND_RANDOM);
	}

	char_to_vector_bit(x, buf, size);
	return x;
}


//	BEGIN RNG
/**
	Input  : integer\n
	Output : returns a random boolean vector of size same as input
*/
std::vector<bool> RNG(int size){
	const int range_from = 0;
	const int range_to = 1;
	std::random_device rand_dev;
	std::mt19937 generator(rand_dev());
	std::uniform_int_distribution<int> distr(range_from, range_to);
	std::vector<bool> v;
	for (size_t i = 0; i < size; i++){
		if(distr(generator)) v.push_back(true);
		else v.push_back(false);
	}
	return v;
}
/*
	END RNG
*/



/*
	BEGIN functions of CLASS PRG

	For detailed description of functions see header file
*/
PRG::PRG(std::vector<bool> (*OWP)(std::vector<bool>), bool (*HCP)(std::vector<bool>)){
		this->OWP = OWP;
		this->HCP = HCP;
}

std::vector<bool> PRG::oneBitStretch(std::vector<bool> input){
	std::vector<bool> output = this->OWP(input);
	output.push_back(this->HCP(input));
	return output;
}

std::vector<bool> PRG::doubling(std::vector<bool> input){
	std::vector<bool> output0 = input;
	std::vector<bool> output1;
	for (int i = 0; i < input.size(); ++i){
		output1.push_back(this->HCP(output0));
		output0 = this->OWP(output0);
	}
	output0.insert(output0.end(), output1.begin(), output1.end());
	return output0;
}
/*
	END functions of CLASS PRG
*/



/*
	BEGIN functions of CLASS PRF

	For detailed description of functions see header file
*/
PRF::PRF(PRG *prg, int seedLength){
	this->prg = prg;
	this->seedLength = seedLength;
	this->seed = TRNG(seedLength);
}

std::vector<bool> PRF::keyGen(std::vector<bool> r){
	std::vector<bool> key((this->seed).begin(), (this->seed).end());
	for (int i = 0; i < r.size(); ++i)
	{
		key = prg->doubling(key);
		if (r[i]) key = std::vector<bool>(key.begin() + (key.size()/2), key.end());
		else key = std::vector<bool>(key.begin(), key.begin() + (key.size()/2));
	}
	return key;
}
/*
	END functions of CLASS PRF
*/



/*
	BEGIN functions of CLASS SKE

	For detailed description of functions see header file
*/
SKE::SKE(PRF *prf, int randomLength){
	this->prf = prf;
	this->randomLength = randomLength;
}

std::vector<bool> SKE::encrypt(std::vector<bool> plainText){
	std::vector<bool> r = RNG(this->randomLength);
	std::vector<bool> Key = (this->prf)->keyGen(r);
	std::vector<bool> cipherText;
	for (int i = 0; i < plainText.size(); ++i) cipherText.push_back(bool_xor(plainText[i], Key[i%Key.size()]));
	r.insert(r.end(), cipherText.begin(), cipherText.end());
	return r;
}

std::vector<bool> SKE::decrypt(std::vector<bool> cipherText){
	std::vector<bool> r(cipherText.begin(), cipherText.begin() + this->randomLength);
	std::vector<bool> Key = (this->prf)->keyGen(r);
	std::vector<bool> plainText;
	for (int i = 0; i < cipherText.size() - this->randomLength; ++i)
		plainText.push_back(bool_xor(cipherText[i+this->randomLength], Key[i%Key.size()]));
	return plainText;
}
/*
	END functions of CLASS SKE
*/



/*
	BEGIN functions of CLASS MAC

	For detailed description of functions see header file
*/
MAC::MAC(PRF *prf){
	this->prf = prf;
}

std::vector<bool> MAC::genMAC(std::vector<bool> m){
	if (m.size()%(prf->seedLength) != 0){
		cout << "message cannot be made into blocks!" << endl;
		exit(EXIT_FAILURE);
	}
	std::vector<bool> output(prf->seedLength, false);
	for (int i = 0; i < m.size()/(prf->seedLength); ++i){
		for (int j = 0; j < prf->seedLength; ++j)
		{
			output[j] = bool_xor(output[j], m[i*(prf->seedLength) + j]);
		}
		output = prf->keyGen(output);
	}
	return output;
}
/*
	END functions of CLASS MAC
*/



/*
	BEGIN functions of CLASS SKE_CCA

	For detailed description of functions see header file
*/
SKE_CCA::SKE_CCA(SKE *ske, MAC *mac){
	this->ske = ske;
	this->mac = mac;
}

std::vector<std::vector<bool>> SKE_CCA::encrypt(std::vector<bool> plainText)
{
	std::vector<std::vector<bool> > cipherText(2);
	cipherText[0] = ske->encrypt(plainText);
	std::vector<bool> encMess(cipherText[0].begin() + ske->randomLength, cipherText[0].end());
	cipherText[1] = mac->genMAC(encMess);
	return cipherText;
}

std::vector<bool> SKE_CCA::decrypt(std::vector<std::vector<bool>> cipherText)
{
	std::vector<bool> encMess(cipherText[0].begin() + ske->randomLength, cipherText[0].end());
	encMess = mac->genMAC(encMess);
	for (int i = 0; i < encMess.size(); ++i){
		if (encMess[i] != cipherText[1][i]){
			cout << "MAC sign is forged!" << endl;
			exit(EXIT_FAILURE);
		}
	}
	encMess = ske->decrypt(cipherText[0]);
	return encMess;
}
/*
	END functions of CLASS MAC
*/