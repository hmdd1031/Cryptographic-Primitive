/*
ECDH 구현
- 개인키 생성 : mpz_urandomm (xA, xB)
- 공개키 생성 : EC_mult (pubA, pubB)
- 키 교환 : EC_mult (KA, KB)
- 검증 : mpz_cmp (KA, KB)
*/
#include <iostream>
#include <gmpxx.h>
#include <vector>

using namespace std;

struct Point {
    mpz_class x, y;
};

struct EC {
    mpz_class a, b, p;
};

mpz_class urandomm(gmp_randstate_t state, const mpz_class r);
mpz_class mod_inv(mpz_class x, mpz_class p);
mpz_class mod(mpz_class n, mpz_class p);
int cmp(mpz_class A, mpz_class B);
void EC_add(EC E, Point *R, Point A, Point B);
void EC_double(EC E, Point *R, Point A);
void bits_inv(vector<int> *bits, mpz_class r);
void EC_mult(EC E, Point *R, Point A, mpz_class r);

int main(){
    mpz_class n = mpz_class{"0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141"};
    EC secp256k1 = {0, 7, mpz_class{"0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F"}};
    Point A = {mpz_class{"0x79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798"}, mpz_class{"0x483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8"}};
    mpz_class xA, xB;
    Point pubA, pubB, KA, KB;

    gmp_randstate_t state;
    gmp_randinit_mt(state);

    xA = urandomm(state, n-1) + 1;
    xB = urandomm(state, n-1) + 1;

    EC_mult(secp256k1, &pubA, A, xA);
    EC_mult(secp256k1, &pubB, A, xB);

    EC_mult(secp256k1, &KA, pubB, xA);
    EC_mult(secp256k1, &KB, pubA, xB);
     
    if(cmp(KA.x, KB.x) == 0 && cmp(KA.y, KB.y) == 0)
        cout << "Success" << endl;
    else   
        cout << "Fail" << endl;
}

mpz_class urandomm(gmp_randstate_t state, const mpz_class r){
    mpz_class n;
    mpz_urandomm(n.get_mpz_t(), state, r.get_mpz_t());
    return n;
}

mpz_class mod_inv(mpz_class x, mpz_class p){ 
    mpz_class r;
    if (mpz_invert(r.get_mpz_t(),x.get_mpz_t(), p.get_mpz_t()) == 0) // x^{-1} mod p
        cout << "error" << endl;
    return r;
}

mpz_class mod(mpz_class n, mpz_class p){
    mpz_class r;
    mpz_mod(r.get_mpz_t(), n.get_mpz_t(), p.get_mpz_t()); // n mod p
    return r;
}

int cmp(mpz_class A, mpz_class B){
    return mpz_cmp(A.get_mpz_t(), B.get_mpz_t());
}

void EC_add(EC E, Point *R, Point A, Point B){
    mpz_class r;

    if(A.x == 0 && A.y == 0)
        *R = B;

    else if (B.x == 0 && B.y == 0)
        *R = A;
    
    else if(A.x == B.x){
        if (mod(A.y + B.y, E.p) == 0){
            R->x = 0;
            R->y = 0; 
        }
        else if (A.y == B.y)
            EC_double(E, R, A);
    else 
        cout << "error" << endl;
    }

    else {
        mpz_class xtemp = B.x - A.x;
        xtemp = mod_inv(mod(xtemp, E.p), E.p);        
        
        mpz_class ytemp = B.y - A.y;
        ytemp = mod(ytemp, E.p);

        mpz_class r = mod(ytemp*xtemp, E.p);
        R->x = mod(((r*r) - A.x - B.x), E.p);
        R->y = mod((r*(A.x - R->x) - A.y), E.p);
    }
}

void EC_double(EC E, Point *R, Point A){
    if (A.y == 0){
        R->x = 0;
        R->y = 0;
        return;
    }

    mpz_class r = mod((3*A.x*A.x + E.a)*(mod_inv(2*A.y, E.p)), E.p);
    R->x = mod(r*r - 2*A.x, E.p);
    R->y = mod(r*(A.x - R->x)-A.y, E.p);
}

void bits_inv(vector<int> *bits, mpz_class p){
    for(; p > 0; p /= 2)
        (*bits).push_back(mpz_class(p % 2).get_si());
}

void EC_mult(EC E, Point *R, Point A, mpz_class p){
    if(p==0){
        R->x = 0;
        R->y = 0;
        return;
    }

    if(p < 0){
        cout << "error" << endl;
        return;
    }

    vector<int> bits;
    bits_inv(&bits, p);
    int r = bits.size();
    *R = A;
    
    for(; r > 1; r--){
        EC_double(E, R,*R);
        if(bits.at(r - 2) == 1)
            EC_add(E, R, *R, A);
    }
}