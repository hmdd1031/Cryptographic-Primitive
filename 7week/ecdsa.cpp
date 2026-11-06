#include <iostream>
#include <gmpxx.h>
#include <vector>
#include <algorithm>
#include <random>

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
    sk->d = urandomm(state, sk->n-1) + 1; // 1 ~ n-1 범위
    EC_mult(sk->E, &sk->Q, sk->G, sk->d);
    pk->Q = sk->Q; 
}

ECDSA_SIG ECDSA_sign(ECDSA_SK sk, mpz_class msg){
    unsigned long seed;
    random_device rd;
    gmp_randstate_t state;
    mpz_class k, inv_k, h_m;
    Point R;
    ECDSA_SIG sig;

    int nBit = mpz_sizeinbase(sk.n.get_mpz_t(), 2); //  함수 반환값이 size_t <- int와 호환 잘 됨
    int mBit = mpz_sizeinbase(msg.get_mpz_t(), 2);

    h_m = msg >> max(mBit - nBit, 0); // msg가 더 큰 경우, 메시지 전체를 그냥 해시로 사용함

    gmp_randinit_mt(state);
    seed = rd();
    gmp_randseed_ui(state, seed);

    // S1
    do{
        k = urandomm(state, sk.n - 1) + 1;
        EC_mult(sk.E, &R, sk.G, k);
    }while(mod(R.x, sk.n) == 0); // 조건이 참이면 계속 반복
    
    sig.S1 = mod(R.x, sk.n);

     // S2
    inv_k = mod_inv(k, sk.n);
    
    if(inv_k == 0){
        cout << "inv_k doesn't exist" << endl;
        ECDSA_sign(sk, msg);
    } 

    sig.S2 = h_m + (sig.S1 * sk.d);
    sig.S2 = mod(sig.S2, sk.n);
    sig.S1 = h_m + (sig.S2 * sk.d);
    sig.S1 = mod(sig.S1, sk.n);

    if(mod(sig.S2, sk.n) == 0 || mod_inv(sig.S2, sk.n) == 0){
        cout << "repeat SIGN" << endl;
        return ECDSA_sign(sk, msg);
    }
    else 
        return sig;
}
ECDSA_SIG ECDSA_sign(ECDSA_SK sk, mpz_class msg) {
    unsigned long seed;
    random_device rd;
    gmp_randstate_t state;

    mpz_class k, inv_k, h_m;
    Point R;
    ECDSA_SIG sig;

    int nBit = mpz_sizeinbase(sk.n.get_mpz_t(), 2);
    int mBit = mpz_sizeinbase(msg.get_mpz_t(), 2);

    // 메시지의 최상위 nBit 추출
    h_m = msg >> max(mBit - nBit, 0);

    gmp_randinit_mt(state);
    seed = rd();
    gmp_randseed_ui(state, seed);

    while (true) {
        // S1 계산
        k = urandomm(state, sk.n - 1) + 1;

        EC_mult(sk.E, &R, sk.G, k);

        // 무한원점 또는 S1 = 0이면 다시 난수 생성
        if ((R.x == 0 && R.y == 0) || mod(R.x, sk.n) == 0)
            continue;

        sig.S1 = mod(R.x, sk.n);

        // S2 계산
        inv_k = mod_inv(k, sk.n);

        if (inv_k == 0)
            continue;

        sig.S2 = inv_k * (h_m + sig.S1 * sk.d);
        sig.S2 = mod(sig.S2, sk.n);

        // S2 = 0이면 다시 서명
        if (sig.S2 == 0)
            continue;

        gmp_randclear(state);
        return sig;
    }
}

bool ECDSA_verify(ECDSA_PK pk, ECDSA_SIG sig, mpz_class msg){
    mpz_class c, u_1, u_2;
    mpz_class h_m; // 입력된 msg의 최상위 nBit (차수 n의 비트 수) 만 추출하여 사용

    // 입력 받은 서명이 정상 범위인지, 서명 값 범위 검사 S1, S2 ∈ [1, n-1]
    if(sig.S1 < 1 || sig.S1 >= pk.n || sig.S2 < 1 || sig.S2 >= pk.n) // S1, S2 둘 중 하나라도 정상 범위가 아니라면
        return false; // 정상 범위가 아니면 검증 자체를 하지 않음.

    int nBit = mpz_sizeinbase(pk.n.get_mpz_t(), 2);
    int mBit = mpz_sizeinbase(msg.get_mpz_t(), 2);
    h_m = msg >> max(mBit - nBit, 0);

    c = mod_inv(sig.S2, pk.n);

    u_1 = mod(h_m * c, pk.n);
    u_2 = mod(sig.S1 * c, pk.n);

    Point R, P1, P2;
    EC_mult(pk.E, &P1, pk.G, u_1);
    EC_mult(pk.E, &P2, pk.G, u_2);
    EC_add(pk.E, &R, P1, P2);

    if(R.x == 0 && R.y == 0){
        cout << "Point of infinity" << endl;
        return false;
    }
    else{
        if(cmp(sig.S1, mod(R.x, pk.n)) == 0)
            return true;
        else 
            return false;
    }
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

int main() {
    struct ECDSA_PK pk;
    struct ECDSA_SK sk;
    struct ECDSA_SIG sig;
    mpz_class msg{"0x10"};
    bool rop;

    ECDSA_keygen(&pk, &sk);
    sig = ECDSA_sign(sk, msg);
    rop = ECDSA_verify(pk, sig);

    if(rop == true)
        cout << "true" << endl;
    else if (rop == false)
        cout << "false" << endl;
    else 
        cout << "false" << endl;
    ...
}
