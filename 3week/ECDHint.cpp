#include <iostream>
#include <vector>

using namespace std;

struct Point {
    int x, y;
};

struct EC{
    int a, b, p;
};

int mod_inv(int x, int mod);
void EC_add(EC E, Point *R, Point P, Point Q);
void EC_double(EC E, Point *R, Point P);
void bits_inv(vector<int> *bits, int r);
void EC_mult(EC E, Point *R, Point P, int r);

int main(){
    int x = 5; 
    int mod = 0;
    
    Point A = {1, 4};
    Point B = {3, 1};
    cout << "A = : (" << A.x << ", " << A.y << ")" << endl;
    cout << "B = : (" << B.x << ", " << B.y << ")" << endl;
    
    EC E = {2, 3, 5};
    Point sum = {0, 0};
    Point doubled = {0, 0};

    EC_add(E, &sum, A, B);
    cout << "A + B = : (" << sum.x << ", " << sum.y << ")" << endl;
    
    EC_double(E, &doubled, A);
    cout << "2A = : (" << doubled.x << ", " << doubled.y << ")" << endl;

    return 0;
}

int mod_inv(int x, int mod){
    int r; 

    if(x == 0)
        return 0;

    else if (mod <= 0)
        return -1;
    
    else {
        while(x < 0)
            x += mod;
        
        for(; r < mod; r++){
            if((r*x) % mod == 1)
                return r;
        }
        return -1;
    }
}

void EC_add(EC E, Point *R, Point P, Point Q){

    if(P.x == 0 && P.y == 0)
        *R = Q;
    
    else if(Q.x == 0 && Q.y == 0)
        cout << "error" << endl;
    else{
        int r = mod_inv((Q.x - P.x), E.p);

        if(r == 0){
            R->x = 0;
            R->y = 0;
        }

        else if(r < 0)
            cout << "error" << endl;

        else{
            r *= (Q.y - P.y);
            r %= E.p;
            
            R->x = (r*r - P.x - Q.x);
            while(R->x < 0)
                R->x += E.p;
            R->x %= E.p;

            R->y = r*(P.x - R->x) - P.y;
            while(R->y < 0)
                R->y += E.p;
            R->y %= E.p;
        }
        
    }
}

void EC_double(EC E, Point *R, Point P){
    int r = mod_inv(2*P.y, E.p);

    if(r == 0){
        R->x;
        R->y;
    }
    else if(r < 0)
        cout << "error" << endl;
    else{
        r *= (3*P.x*P.x + E.a);;
        r %= E.p;

        R->x = r*r - 2*P.x;
        while (R->x < 0)
            R->x += E.p;
        R->x %= E.p;
        
        R->y = r*(P.x - R->x) - P.y;
        while(R->y < 0)
            R->y += E.p;
        R->y %= E.p;
    }
}

void bits_inv(vector<int> *bits, int r){
    for(; r > 0; r /= 2)
        (*bits).push_back(r % 2);
}

void EC_mult(EC E, Point *R, Point P, int r){
    vector<int> bits;
    int n;

    if(P.x == 0 && P.y == 0){
        R->x = 0;
        R->y = 0;
    }
    else if(r <= 0)
        cout << "error" << endl;
    else {
        bits_inv(&bits, r);
        n = bits.size();

        *R = P;

        for(; n > 1; n--){
            EC_double(E, R, *R);
            if(bits.at(n-2) == 1)
                EC_add(E, R, *R, P);
        }
    }

}