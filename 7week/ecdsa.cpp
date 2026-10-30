#include <iostream>
#include <gmpxx.h>
#include <vector>
#include <algorithm>

using namespace std;

struct EC{
    mpz_class a;
    mpz_class b;
    mpz_class p;
};

struct Point{
    mpz_class x;
    mpz_class y;
};

struct ECDSA_PK{
    EC E;
    Point G;
    mpz_class n;
    Point Q;
};

struct ECDSA_SK{
    EC E;
    Point G;
    mpz_class n;
    Point Q;
    mpz_class d;
};

struct ECDSA_SIG{
    mpz_class S1;
    mpz_class S2;
};


mpz_class mod_inv(mpz_class x, mpz_class mod);
mpz_class mod(mpz_class n, mpz_class p);
mpz_class urandomm(gmp_randstate_t state, const mpz_class n);
void ECDSA_keygen(ECDSA_PK *pk, ECDSA_SK *sk);
ECDSA_SIG ECDSA_sign(ECDSA_SK sk, mpz_class msg);
bool ECDSA_verify(ECDSA_PK, ECDSA_sIG, mpz_class msg);
int cmp(mpz_class x, mpz_class y);
void EC_add(EC E, Point *R, Point P, Point Q);
void EC_double(EC E, Point *R, Point P);
void bits_inv(vector<bool> *bits, mpz_class r);
void EC_mult(EC E, Point *R, Point P, mpz_class r);


int main() {

}

mpz_class mod_inv(mpz_class x, mpz_class mod){
    mpz_class r;
    if(mpz_invert(r.get_mpz_t(), x.get_mpz_t(), mod.get_mpz_t()) == 0)
        cout << "error : an inverse doesn't exist" << endl;
    return r;
}

mpz_class mod(mpz_class n, mpz_class p){
    mpz_class r;
    mpz_mod(r.get_mpz_t(), n.get_mpz_t(), p.get_mpz_t());
    return r;
}

mpz_class urandomm(gmp_randstate_t state, const mpz_class n){
    mpz_class r;
    mpz_urandomm(r.get_mpz_t(), state, n.get_mpz_t());
    return r;
}

void ECDSA_keygen(ECDSA_PK *pk, ECDSA_SK *sk){

    unsigned long seed;
    random_device rd;
    gmp_randstate_t state;

    // 타원곡선 셋업
    pk->E = {2, 2, 17};
    sk->E = pk->E;
    pk->G = {5,1};
    sk->G = pk->G;
    pk->n = 19;
    sk->n = pk->n;

    // 랜덤한 개인키 선택
    seed = rd();
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, seed);
    
    // 공개키 계산
    sk->d = urandomm(state, sk->n-1) + 1;
    EC_mult(sk->E, &sk->Q, sk->G, sk->d);
    pk->Q = sk->G;
}

ECDSA_SIG ECDSA_sign(ECDSA_SK sk, mpz_class msg){
    nBit = mpz_sizeinbase(sk.m.get_mpz_t(), 2);
    mBit = mpz_sizeinbase(msg.get_mpz_t(), 2);
    h_m = msg >> max(mBit - nBit, 0); // msg가 더 큰 경우, 메시지 전체를 그냥 해시로 사용함

}

bool ECDSA_verify(ECDSA_PK, ECDSA_sIG, mpz_class msg){

}

int cmp(mpz_class x, mpz_class y){
    int r;
    r = mpz_cmp(x.get_mpz_t(), y.get_mpz_t());
    return r;
}

void EC_add(EC E, Point *R, Point P, Point Q){
    mpz_class r;

    if (P.x == 0 && P.y == 0) // P = point of infinity
        *R = Q;

    else if (Q.x == 0 && Q.y == 0) // Q = point of infinity
        *R = P;

    else if (P.x == 0 && Q.x == 0){
        R->x = 0;
        R->y = 0;
    }

    else if (P.x == Q.x && P.y == Q.y){
        EC_double(E, R, P);
    }
    
    else {
        mpz_class xtemp = Q.x - P.x;
        xtemp = mod(xtemp, E.p);
        xtemp = mod_inv(xtemp, E.p);

        mpz_class ytemp = Q.y - P.y;
        ytemp = mod(ytemp, E.p);

        r = ytemp*xtemp;
        r = mod(r, E.p);
        
        xtemp = r*r - P.x - Q.x;
        xtemp = mod(xtemp, E.p);
        
        ytemp = r*(P.x - xtemp) - P.y;
        ytemp = mod(ytemp, E.p);

        R->x = xtemp;
        R->y = ytemp;
    }
}

void EC_double(EC E, Point *R, Point P){
    mpz_class r, xtemp, ytemp;

    if((P.x == 0 && P.y == 0) || mod(P.y, E.p) == 0){
        R->x = 0;
        R->y = 0;

        return ;
    }

    r = mod((3*P.x*P.x + E.a) * (mod_inv(2*P.y, E.p)), E.p);

    xtemp = mod(r*r - 2*P.x, E.p);

    ytemp = mod(r*(P.x - xtemp) - P.y, E.p);

    R->x = xtemp;
    R->y = ytemp;
}

void bits_inv(vector<bool> *bits, mpz_class r){
    (*bits).clear();
    for(; r > 0; r /= 2)
        (*bits).push_back(r % 2 == 1);
}

void EC_mult(EC E, Point *R, Point P, mpz_class r){
    vector<bool> bits;
    int n;

    if(r == 0){
        R->x = 0;
        R->y = 0;

        return ;
    }

    if(r < 0){
        EC_mult(E, R, P, -r);
        if(!(R->x == 0 && R->y == 0))
            R->y = mod(-R->y, E.p);
        return ;
    }

    bits_inv(&bits, r);
    n = bits.size();

    *R = P;;

    for(; n > 1; n--){
        EC_double(E, R, *R);
        if(bits.at(n-2) == 1)
            EC_add(E, R, *R, P);
    }
}

