#include <iostream>
#include <string>
#include <openssl/evp.h>

using namespace std;

string openssl_hash(string data, string algorithm) {
    EVP_MD_CTX *mdctx;
    const EVP_MD *md;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len;

    md = EVP_get_digestbyname(algorithm.c_str());
    if (md == NULL) {
        cerr << "Error: " << algorithm << " algorithm not found!" << endl;
        return "";
    }

    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, data.c_str(), data.length());
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_free(mdctx);

    string result = "";
    for (unsigned int i = 0; i < md_len; i++) {
        result += (char)(md_value[i]);
    }
    return result;
}

/*
int main() {
    string data = "Hello, world!";

    cout << "Original text: " << data << endl;

    cout << "SHA-256 hash: " << openssl_hash(data, "sha256") << endl;
    cout << "SHA-1 hash: " << openssl_hash(data, "sha1") << endl;
    cout << "MD5 hash: " << openssl_hash(data, "md5") << endl;

    return 0;
}
*/

