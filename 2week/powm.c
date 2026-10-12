#include <stdio.h>

int powm(int base, int exp, int mod);
bool is_primitive(int base, int mod);

int main(){
    int b, e, m, res;

    printf("input base, exp, mod: ");
    scanf("%d, %d, %d", &b, &e, &m);

    printf(is_primitive(b, m) ? "true\n" : "false\n");

    res = powm(b, e, m);
    printf("result : %d\n", res);

    return 0;
}

int powm(int base, int exp, int mod){

    int r = 1;
    for (int i = 0; i < exp; i++){
        r *= base;
        r %= mod;
    }

    return r;
}

bool is_primitive(int base, int mod){
    if (powm(base, mod-1, mod) == 1)
        return true;
    else
        return false;
}

/*
bool is_primitive(int base, int mod){
    int exp = 2;
    int r = base * base;
    for(; exp < base; exp++){
        r *= base;
        r %= mod;
    }
    return exp == mod;
}
*/