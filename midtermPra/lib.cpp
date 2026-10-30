// 라이브러리 함수 정리
#include <iostream>
#include <gmpxx.h>
#include <vector>
using namespace std;

bool is_primitive(int base, int mod);
vector<int> primitive_root(int mod);
mpz_class nextprime(mpz_class z);
mpz_class powm(mpz_class m, mpz_class e, mpz_class k);
mpz_class invert(mpz_class e, mpz_class p);
int sizeinbase(mpz_class z, int base);
mpz_class urandomm(gmp_randstate_t state, const mpz_class r);
mpz_class mod(mpz_class m, mpz_class p);

// 원시근 계산
bool is_primitive(int base, int mod){
    int exp = 2;
    int r = base*base;
    for(; r != base; exp++){
        r *= base;
        r %= mod;
    }
    return exp == mod;
}

vector<int> primitive_root(int mod){
    vector<int> v;
    for(int base = 2; base < mod; base++){
        if(is_primitive(base, mod))
            v.push_back(base);
    }
    return v;
}

// z 보다 큰 최초의 소수를 return
// 입력값보다 큰 소수를 반환함
mpz_class nextprime(mpz_class z){
    mpz_class r;
    mpz_nextprime(r.get_mpz_t(), z.get_mpz_t());
    return r;
}

// m^e mod K를 구해서 r에 저장
mpz_class powm(mpz_class m, mpz_class e, mpz_class k){
    mpz_class r;
    mpz_powm(r.get_mpz_t(),
                m.get_mpz_t(),
                e.get_mpz_t(), 
                k.get_mpz_t());
    return r;
}

// e * d mod phi = 1이 되는 d를 구함
mpz_class invert(mpz_class e, mpz_class p){
    mpz_class d;
    if(mpz_invert(d.get_mpz_t(), e.get_mpz_t(), p.get_mpz_t()) == 0){
        return -1;
    }
    return d;
}

// z를 base 진수로 나타냈을 때의 자릿수 리턴
int sizeinbase(mpz_class z, int base){
    return mpz_sizeinbase(z.get_mpz_t(), base);
}

// 호출측에서 state를 gmp_randinit_mt(state)로 초기화
// 0 이상 r 미만의 난수를 반환
mpz_class urandomm(gmp_randstate_t state, const mpz_class r){
    mpz_class n;
    mpz_urandomm(n.get_mpz_t(), state, r.get_mpz_t());
    return n;
}

// 나머지 계산
mpz_class mod(mpz_class m, mpz_class p){
    mpz_class r;
    mpz_mod(r.get_mpz_t(), m.get_mpz_t(), p.get_mpz_t());
    return r;
}