#include <iostream>
#include <vector>
#include <random>
using namespace std;


/*
    Class for Pseudo-Random Generator(PRG)
*/
class PRG
{
    /*
        PRG consists of a One-Way Permutation(OWP) and a Hard-Core Predicate(HCP)
    */
    std::vector<bool> (*OWP)(std::vector<bool>);
    bool (*HCP)(std::vector<bool>);

    public:
        /* 
            Generator Function
        */    
        PRG(std::vector<bool> (*OWP)(std::vector<bool>), bool (*HCP)(std::vector<bool>));
        
        /*
            Input : vector of booleans
            Takes input an boolean function and stretches gives one bit stretch output by calling OWP on input and appending HCP to it.
        */
        std::vector<bool> oneBitStretch(std::vector<bool> input);
        
        /*
            Input : vector of booleans
            Outputs a double length vector by applying OWP repeatedly and taking HCP
        */
        std::vector<bool> doubling(std::vector<bool> input);
};

/*
    Class for Pseudo-Random Function(PRF)
*/
class PRF
{
    /*
        PRF consists of a PRG and a seed
    */
    PRG *prg;
    std::vector<bool> seed;

    public:
        // length of seed for PRF
        int seedLength;

        /* 
            Generator Function
        */
        PRF(PRG *prg, int seedLength);
        
        /*
            Input  : Random boolean vector
            Output : Generates a key using seed and random-vector
        */
        std::vector<bool> keyGen(std::vector<bool> r);
};

/*
    Class for Symmetric Key Encryption(SKE)-CPA
*/
class SKE
{
    /*
        SKE consists of a PRF and the length of random used in encryption
    */
    PRF *prf;

    public:

        // Length of random used
        int randomLength;

        /*
            Generator Function
        */
        SKE(PRF *prf, int randomLength);

        /*
            Input  : Message as vector of boolean
            Output : Encrypts the message using PRF and send the random, cipherText of message
        */
        std::vector<bool> encrypt(std::vector<bool> plainText);

        /*
            Input  : CipherText of message along with random used for encryption
            Output : The original plain text message is obtained by obtaining random from the input and decrypting the message
        */
        std::vector<bool> decrypt(std::vector<bool> cipherText);
};

/*
    Class for Message Authetication Code(MAC)
*/
class MAC
{
    /*
        MAC consists of a PRF
    */
    PRF *prf;

    public:
    
        /*
            Generator Function
        */
        MAC(PRF *prf);

        /*
            Input  : Message for which authentication code is to be generated
            Output : MAC is generated using PRF, we considered case when message is in blocks rather than a single block
        */
        std::vector<bool> genMAC(std::vector<bool> m);
};

/*
    Class for Message Authetication Code(MAC)
*/
class SKE_CCA
{
    /*
        CCA SKE consists of CPA-secure SKE and a MAC
    */
    SKE *ske;
    MAC *mac;

    public:

        /*
            Generator Function
        */
        SKE_CCA(SKE *ske, MAC *mac);

        /*
            Input  : Message to be encryped
            Output : Gives out both CPA secure encyption and MAC corresponding to message 
        */
        std::vector<std::vector<bool>> encrypt(std::vector<bool> plainText);

        /*
            Input  : CCA encrypted message
            Output : Original Plain Text. Sign is verified first using MAC and then decrypted using CPA secure SKE decryption
        */
        std::vector<bool> decrypt(std::vector<std::vector<bool>> cipherText);
};
