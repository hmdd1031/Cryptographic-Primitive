#include <iostream>
#include <gmpxx.h>
using namespace std;

struct RSA_PK{
    mpz_class e;
    mpz_class N;
};

struct RSA_SK{
    mpz_class e;
    mpz_class d;
    mpz_class N;
    mpz_class p;
    mpz_class q;
};

mpz_class lcm(mpz_class a, mpz_class b){
    mpz_class rop;
    mpz_lcm(rop.get_mpz_t(), a.get_mpz_t(), b.get_mpz_t());
    return rop;
}

mpz_class nextprime(mpz_class n){
    mpz_class rop;
    mpz_nextprime(rop.get_mpz_t(), n.get_mpz_t());
    return rop;
}

mpz_class mod_inv(mpz_class x, mpz_class mod){
    mpz_class r;
    mpz_invert(r.get_mpz_t(), x.get_mpz_t(), mod.get_mpz_t());
    return r;
}

mpz_class powm(mpz_class base, mpz_class exp, mpz_class mod){
    mpz_class r;
    mpz_powm(r.get_mpz_t(), base.get_mpz_t(), exp.get_mpz_t(), mod.get_mpz_t());
    return r;
}

int cmp(mpz_class A, mpz_class B){
    return (mpz_cmp(A.get_mpz_t(), B.get_mpz_t()));
}

mpz_class urandomb(gmp_randstate_t state, mp_bitcnt_t n){
    mpz_class r;
    gmp_randinit_mt(state);
    mpz_urandomb(r.get_mpz_t(), state, n);
    return r;
}

mpz_class sizeinbase(mpz_class op, int base){
    return (mpz_sizeinbase(op.get_mpz_t(), base));
}

mpz_class random_prime(unsigned int bit){
    mpz_class r, p;
    unsigned long seed;

    gmp_randstate_t state;
    gmp_randinit_mt(state);
    
    seed = time(NULL);
    gmp_randseed_ui(state, seed);

    r = urandomb(state, bit);
    p = nextprime(r);

    if (mpz_sizeinbase(p.get_mpz_t(), 2) > bit)
        return random_prime(bit);
    else
        return p;
}

mpz_class gcd(mpz_class a, mpz_class b){
    mpz_class r;
    mpz_gcd(r.get_mpz_t(), a.get_mpz_t(), b.get_mpz_t());
    return r;
}

void RSA_keygen(RSA_PK *pk, RSA_SK *sk, int key_size){
    mpz_class ell;

    // p와 q는 서로 다른 두 소수여야 함. cmp 함수로 비교
    sk->p = random_prime(key_size / 2);
    sk->q = random_prime(key_size / 2);
    while(cmp(sk->q, sk->p)==0)
        sk->q = random_prime(key_size); // 같으면 p나 q를 다시 뽑음
    sk->N = sk->p * sk->q;
    pk->N = sk->N;

    ell = lcm(sk->p - 1, sk->q - 1);
    pk->e = 0x10001; // 공개키, 고정

    if(gcd(pk->e, ell) != 1) // 최대공약수가 1인지, 서로소 인지
        pk->e = nextprime(pk->e); // 아나라면, 공개키 값을 nextprime으로 소수를 뽑아버림, p,q를 다시 뽑던, e를 랜덤 값을 뽑던...

    sk->e = pk->e;
    sk->d = mod_inv(pk->e, ell); // 개인키 d 곱셈 역원으로 계산
}

mpz_class RSA_encrypt(RSA_PK pk, mpz_class msg){
    return powm(msg, pk.e, pk.N);
}

mpz_class RSA_decrypt(RSA_SK sk, mpz_class ct){
    return powm(ct, sk.d, sk.N);
}   

mpz_class RSA_sign(RSA_SK sk, mpz_class msg){
    return powm(msg, sk.d, sk.N);
}

mpz_class mod(mpz_class n, mpz_class p){
    mpz_class r;
    mpz_mod(r.get_mpz_t(), n.get_mpz_t(), p.get_mpz_t());
    return r;
}


mpz_class RSA_decrypt_CRT(RSA_SK sk, mpz_class ct){
    mpz_class m1, m2;
    mpz_class M1_inv, M2_inv;
    mpz_class msg;

    m1 = powm(ct, sk.d, sk.p);
    m2 = powm(ct, sk.d, sk.q);

    M1_inv = mod_inv(sk.q, sk.p);
    M2_inv = mod_inv(sk.p, sk.q);

    msg = (m1 * sk.q * M1_inv);
    msg = mod(msg, sk.N);
    msg += (m2 * sk.p * M2_inv);
    msg = mod(msg, sk.N);

    return msg;
}

bool RSA_verify(RSA_PK pk, mpz_class sig, mpz_class msg){
    mpz_class r;
    r = powm(sig, pk.e, pk.N);
    if(cmp(r, msg) == 0)
        return true;
    else 
        return false;
}

int main(){
    
    /*
    mpz_class x;
    x = random_prime(1024);
    cout << mpz_probab_prime_p(x.get_mpz_t(), 50) << endl;
    // return 2 if x is definitely prime
    // return 1 if x is probably prime (error rate < 4^{-50})
    // return 0 if x is definitely non-prime
    */
    struct RSA_PK pk;
    struct RSA_SK sk;
    mpz_class msg = mpz_class{0x23423423};
    RSA_keygen(&pk, &sk, 2048);
    mpz_class ct = RSA_encrypt(pk, msg);
    mpz_class dec = RSA_decrypt_CRT(sk, ct);

    if(cmp(msg, dec) != 0)
        cout << "decryption failed" << endl;

    RSA_keygen(&pk, &sk, 2048);
    mpz_class sig = RSA_sign(sk, msg);
    mpz_class rop = RSA_verify(pk, sig, msg); 

    if(rop == true)
        cout << "true" << endl;
    else if (rop == false)
        cout << "false" << endl;
    else 
        cout << "error" << endl;

    

    return 0;
}