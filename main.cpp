#include "cryptolib.cpp"
#include <iostream>
#include <vector>
#include <random>
using namespace std;

std::vector<bool> supportQuasiR2(std::vector<bool> input, bool value){
    std::vector<bool> output(input.size());
    output[0] = bool_xor(value, input[0]);
    for(int i=1; i<input.size(); i++)
        output[i] = bool_xor(input[i], output[i-1]);
    return output;
}
std::vector<bool> quasiR2(std::vector<bool> input){
    std::vector<bool> output(input.begin(), input.end());
    for (int j = 0; j < 2; j++)
        for (int i = 0; i < input.size(); i++){
            output = supportQuasiR2(output, input[i]);
        }
    return output;
}

std::vector<bool> owp(std::vector<bool> input)
{
    // write code for one way permutation and return the permutation
    return quasiR2(input);
}

std::vector<bool> asis(std::vector<bool> input)
{
    // write code for one way permutation and return the permutation
    return input;
}

bool hcp(std::vector<bool> input){
    // write code for hard core predicate for the above owp
    return input[input.size() - 1];
}

int main(int argc, char const *argv[])
{
    PRG a = PRG(owp, hcp);
    PRF prf = PRF(&a, 40);
    PRF prfMAC = PRF(&a, 20);
    MAC mac = MAC(&prfMAC);
    SKE ske = SKE(&prf, 10);
    SKE_CCA ske_cca = SKE_CCA(&ske, &mac);
    
    // PRG b = PRG(owp, hcp);
    // PRF prf1 = PRF(&b, 40);
    // PRF prfMAC1 = PRF(&b, 20);
    // MAC mac1 = MAC(&prfMAC1);
    // SKE ske1 = SKE(&prf1, 10);
    // SKE_CCA ske_cca1 = SKE_CCA(&ske1, &mac1);

    vector <bool> x(40);
    for(int i = 0; i < 40; i++){
        x[i] = ((i%3)%2);
    }
    // std::vector<std::vector<bool>> ciph = ske_cca.encrypt(x);
    // std::vector<bool> y = ske_cca.decrypt(ciph);
    std::vector<bool> ciph = ske.encrypt(x);
    std::vector<bool> y = ske.decrypt(ciph);
    if(y.size() == 40) cout << "message size match"<<endl;
    bool q = true;
    for(int i = 0; i < 40; i++){
        if(x[i] != y[i])
            q = false;
            break;    
    }
    if(q)
        cout <<"output matched" << endl;
    else
        cout <<"output mismatch" << endl;
}