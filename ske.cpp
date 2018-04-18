#include <iostream>
#include <vector>
#include <random>
using namespace std;

std::vector<bool> asis(std::vector<bool> input){
	return input;
}
bool LSB(std::vector<bool> input){
	return input[input.size() - 1];
}

bool bool_xor(bool a, bool b){
	return (a && !b) || (!a && b);
}

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

std::vector<bool> RG(int length){
	std::vector<bool> r;
	for (int i = 0; i < length; ++i){
		r.push_back(rand()%2)
	}
	return r;
}

class PRG
{
	std::vector<bool> (*OWP)(std::vector<bool>);
	bool (*HCP)(std::vector<bool>);
public:
	PRG(std::vector<bool> (*OWP)(std::vector<bool>), bool (*HCP)(std::vector<bool>)){
		this->OWP = OWP;
		this->HCP = HCP;
	}
	// PRG(std::vector<bool> (*OWP)(std::vector<bool>)){
	// 	this->OWP = OWP;
	// }
	std::vector<bool> oneBitStretch(std::vector<bool> input){
		std::vector<bool> output = this->OWP(input);
		output.push_back(this->HCP(input));
		return output;
	}
	std::vector<bool> doubling(std::vector<bool> input){
		std::vector<bool> output0 = input;
		std::vector<bool> output1;
		for (int i = 0; i < input.size(); ++i){
			output1.push_back(this->HCP(output0));
			output0 = this->OWP(output0);
		}
		output0.insert(output0.end(), output1.begin(), output1.end());
		return output0;
	}
};

class PRF
{
	PRG *prg;
	std::vector<bool> seed;
	int seedLength;
public:
	PRF(PRG *prg, int seedLength){
		this->prg = prg;
		this->seedLength = seedLength;
		this->seed = RNG(seedLength);
	}
	std::vector<bool> keyGen(std::vector<bool> r){
		std::vector<bool> key((this->seed).begin(), (this->seed).end());
		for (int i = 0; i < r.size(); ++i)
		{
			key = prg->doubling(key);
			if (r[i]) key = std::vector<bool>(key.begin() + (key.size()/2), key.end());
			else key = std::vector<bool>(key.begin(), key.begin() + (key.size()/2));
		}
		return key;
	}
};

class SKE
{
	PRF *prf;
	int randomLength;
public:
	SKE(PRF *prf, int randomLength){
		this->prf = prf;
		this->randomLength = randomLength;
	}
	std::vector<bool> encrypt(std::vector<bool> plainText){
		std::vector<bool> r = RNG(this->randomLength);
		std::vector<bool> Key = (this->prf)->keyGen(r);
		std::vector<bool> cipherText;
		for (int i = 0; i < plainText.size(); ++i) cipherText.push_back(bool_xor(plainText[i], Key[i%Key.size()]));
		r.insert(r.end(), cipherText.begin(), cipherText.end());
		return r;
	}
	std::vector<bool> decrypt(std::vector<bool> cipherText){
		std::vector<bool> r(cipherText.begin(), cipherText.begin() + this->randomLength);
		std::vector<bool> Key = (this->prf)->keyGen(r);
		std::vector<bool> plainText;
		for (int i = 0; i < cipherText.size() - this->randomLength; ++i)
			plainText.push_back(bool_xor(cipherText[i+this->randomLength], Key[i%Key.size()]));
		return plainText;
	}
};

int main(int argc, char const *argv[])
{
	srand(time(NULL));
	std::vector<bool> v;
	for (int i = 0; i < 10; ++i)v.push_back(true);
	PRG a = PRG(asis, LSB);
	v = a.oneBitStretch(v);
	for (int i = 0; i < v.size(); ++i)cout << v[i];
	cout << endl;
	return 0;
}