#include "ske.cpp"
#include <iostream>
#include <vector>
#include <random>
using namespace std;

std::vector<bool> asis(std::vector<bool> input)
{
    return input;
}

bool LSB(std::vector<bool> input)
{
    return input[input.size() - 1];
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    std::vector<bool> v;
    for (int i = 0; i < 10; ++i)
        v.push_back(true);
    PRG a = PRG(asis, LSB);
    PRF prf = PRF(&a, 40);
    PRF prfMAC = PRF(&a, 20);
    MAC mac = MAC(&prfMAC);
    SKE ske = SKE(&prf, 10);
    SKE_CCA ske_cca = SKE_CCA(&ske, &mac);

    vector <bool> x(40);
    for(int i = 0; i < 40; i++){
        x[i] = ((i%3)%2);
    }
    std::vector<std::vector<bool>> ciph = ske_cca.encrypt(x);
    std::vector<bool> y = ske_cca.decrypt(ciph);
    if(y.size() == 40) cout << "true"<<endl;
    for(int i = 0; i < 40; i++){
        if(x[i] == y[i])
            cout << "1" << endl;
        else
            cout << "0" << endl;
    }
}