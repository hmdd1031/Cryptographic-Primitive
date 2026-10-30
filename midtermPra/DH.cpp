/*
[main 함수]
- 공개 파라미터 p, g 설정/ mpz_class p{"0x"}, mpz_class g = 2
- 개인키 생성
- 공개키 계산
- 공유하는 비밀키 계산
- 비교 연산

powm(인자 3개 base, exp, mod)
mpz_powm(인자 4개 r, base, exp, mod)

urandomm(인자 2개 gmp_randstate_t state, const mpz_class n)
mpz_urandomm(인자 3개 r, gmp_randstate_t state, const mpz_class n)

cmp(인자 2개 mpz_class A, mpz_Class B)
mpz_cmp(인자 2개 위와 동일)
*/

#include <iostream>
#include <gmpxx.h>

using namespace std;

mpz_class powm(mpz_class, mpz_class, mpz_class); 
mpz_class urandomm(gmp_randstate_t , const mpz_class); 
int cmp(mpz_class, mpz_class);

int main(){
    mpz_class xA, xB, pubA, pubB, KA, KB;

    mpz_class p {"0xFFFFFFFFFFFFFFFFADF85458A2BB4A9AAFDC5620273D3CF1D8B9C583CE2D3695A9E13641146433FBCC939DCE249B3EF97D2FE363630C75D8F681B202AEC4617AD3DF1ED5D5FD65612433F51F5F066ED0856365553DED1AF3B557135E7F57C935984F0C70E0E68B77E2A689DAF3EFE8721DF158A136ADE73530ACCA4F483A797ABC0AB182B324FB61D108A94BB2C8E3FBB96ADAB760D7F4681D4F42A3DE394DF4AE56EDE76372BB190B07A7C8EE0A6D709E02FCE1CDF7E2ECC03404CD28342F619172FE9CE98583FF8E4F1232EEF28183C3FE3B1B4C6FAD733BB5FCBC2EC22005C58EF1837D1683B2C6F34A26C1B2EFFA886B423861285C97FFFFFFFFFFFFFFFF"};
    mpz_class g = 2;
    gmp_randstate_t state;
    gmp_randinit_mt(state);

    xA = urandomm(state, p);
    xB = urandomm(state, p);

    pubA = powm(g, xA, p);
    pubB = powm(g, xB, p);

    KA = powm(pubB, xA, p);
    KB = powm(pubA, xB, p);

    if (cmp(KA,KB) == 0)
        cout << "Success" << endl;
    else 
        cout << "Fail" << endl;

    return 0;
}

// m^e mod K
mpz_class powm(mpz_class m, mpz_class e, mpz_class k){
    mpz_class r;
    mpz_powm(r.get_mpz_t(), m.get_mpz_t(), e.get_mpz_t(), k.get_mpz_t());
    return r;
}

// gmp_randstate_t 자료형 변수 선언, 
// gmp_randinit_mt 함수로 변수 초기화, 
// mpz_urandomm 함수로 p보다 작은 랜덤값 생성
mpz_class urandomm(gmp_randstate_t state, const mpz_class r){
    mpz_class res;
    gmp_randinit_mt(state);
    mpz_urandomm(res.get_mpz_t(), state, r.get_mpz_t());
    return res;
}

int cmp(mpz_class p, mpz_class q){
    int r;
    r = mpz_cmp(p.get_mpz_t(), q.get_mpz_t());
    return r;
}