#include <iostream>
#include <gmpxx.h>
#include <vector>

using namespace std;

struct Point {
    int x, y;
};

struct EC {
    int a, b, p;
};

int cmp(int A, int B);
int mod_inv(int x, int mod);
void EC_add(EC E, Point *R, Point A, Point B);
void EC_double(EC E, Point *R, Point A);
void bits_inv(vector<int> *bits, int r);
void EC_mult(EC E, Point *R, Point A, int r);

int main(){
    EC E = {2, 2, 17};
    Point G = {5, 1};
    Point R;

    cout << "G+2G :" << endl;
    EC_add(E, &R, G, R);
    cout << R.x << "," << R.y << endl;

    cout << "2G :" << endl;
    EC_mult(E, &R, G ,2);
    cout << R.x << "," << R.y << endl;

}

int mod_inv(int x, int mod){
    if(x == 0)
        return -1;
    
    else if(mod <= 0)
        return -1;

    else{
        while (x < 0)
            x += mod;
        for(int r = 1; r < mod; r++){ // r은 상수
            if((x*r) % mod == 1)
                return r;
        }
        return -1;
    }
}

void EC_add(EC E, Point *R, Point A, Point B){
    if(A.x ==0 && A.y ==0){
        *R = B;
    }
    else if(B.x ==0 && B.y ==0){
        *R = A;
    }
    else {
        int r = mod_inv((B.x - A.x), E.p);
        r %= E.p;

        r *= B.y - A.y ;
        R->x = r*r - A.x - B.x;
        while(R->x < 0)
            R->x += E.p;
        R->x %= E.p;

        R->y = r*(A.x - R->x)-A.y;
        while(R->y < 0)
            R->y += E.p;
        R->y %= E.p;
    }
}

void EC_double(EC E, Point *R, Point A){
    if(A.y % E.p == 0){
        R->x = 0;
        R->y = 0;
    }
    else {
        int r = mod_inv((2*A.y), E.p);
        r %= E.p;

        r *= 3*A.x;

        R->x = r*r - 2*A.x;
        while (R->x < 0)
            R->x += E.p;
        R->x %= E.p;

        R->y = r*(A.x- R->x)-A.y;
        while (R->y < 0)
            R->y += E.p;
        R->y %= E.p;
    }
}

void bits_inv(vector<int> *bits, int r){
    for(; r > 0; r /= 2)
        (*bits).push_back(r%2);
}

void EC_mult(EC E, Point *R, Point A, int r){
    if(r == 0){
        R->x = 0;
        R->y = 0;
    }
    vector<int> v;
    bits_inv(&v, r);
    int n = v.size();

    *R = A;
    
    for(; n > 1; n--){
        EC_double(E, R, *R);
        if (v.at(n-2) == 1)
            EC_add(E, R, *R, A);    
    }

}  













/*
// p=29, g=19, a=11, b=23으로 int DH를 구현해서 A, B, 공유키 C 출력

int powm(int g, int e, int p){
    int r=1;
    for(int i = 0; i < e; i++){ // 지수번 반복 지수만큼 곱해야 함 
        r*=g; // base 제곱
        r %= p;
    }
    return r;

}

int main(){
    int p=29, g =19, a=11, b=23;

    int A = powm(g, a, p);
    int B = powm(g, b, p);

    int C = powm(B, a, p);
    cout << A<< endl;
    cout << B << endl;

    cout << C << endl;
}
*/


/*
// is_primitive와 primitive_root를 구현해서 소수 29의 원시근을 모두 출력
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
    for(int base = 2; base<mod; base++ )
        if(is_primitive(base, mod))
            v.push_back(base);
    return v;
}

int main(){
    vector<int> v = primitive_root(29);
    for(int i=0; i < v.size(); i++ )
        cout << v.at(i) <<endl;
}

*/



/*
int powm(int base, int exp, int mod);

int main(){
    // cout << powm(3, 100, 23) << endl;
    return 0;
}

int powm(int base, int exp, int mod){
    int e=1;
    if(mod <= 0){
        cout << "error" << endl;
        return -1;
    }

    else {
        while(base < 0)
            base += mod;
        base %= mod;
        
        for(int i = 0; i < exp; i++){
            e *= base;   
            e %= mod;
        }
        return e;
    }
    return -1;
}*/