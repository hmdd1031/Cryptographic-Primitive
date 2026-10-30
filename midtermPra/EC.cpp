#include <iostream>
#include <vector>

using namespace std;

struct Point {
    int x, y;
};

struct EC {
    int a, b, p;
};

int mod_inv(int x, int mod);
void EC_add(EC E, Point *R, Point A, Point B);
void EC_double(EC E, Point *R, Point A);
void bits_inv(vector<int> *bits, int r);
void EC_mult(EC E, Point *R, Point A, int r);

int main(){
    Point R;
    EC E = {2, 3, 5};

    Point A = {1, 1};
    Point B = {3, 1};
    cout << "A = : (" << A.x << ", " << A.y << ")" << endl;
    cout << "B = : (" << B.x << ", " << B.y << ")" << endl;

    EC_add(E, &R, A, B);
    cout << "A + B = : (" << R.x << ", " << R.y << ")" << endl;
    
    EC_double(E, &R, A);
    cout << "2A = : (" << R.x << ", " << R.y << ")" << endl;

    EC_mult(E, &R, A, 9);
    cout << "9A = : (" << R.x << ", " << R.y << ")" << endl;

    return 0;
}

int mod_inv(int x, int mod){
    if (x == 0)
        return 0;
    else if(mod <= 0)
        return -1;
    else
        while (x<0)
            x += mod;

        for(int r = 0; r < mod; r++){
            if((x*r) % mod == 1)
                return r;
        }
        return -1;
}

void EC_add(EC E, Point *R, Point A, Point B){
    int r;

    if(A.x == 0 && A.y == 0)
        *R = B;

    else if (B.x == 0 && B.y == 0)
        *R = A;

    else if(A.x == B.x){
        if((A.y + B.y) % E.p == 0){
             R->x = 0; 
             R->y = 0;
        }

        else if(A.y == B.y)
            EC_double(E, R, A);

        else 
            cout << "error" <<  endl;
    }
    
    else {
        r = mod_inv((B.x - A.x), E.p);

        if (r == 0){
            R->x = 0;
            R->y = 0;
        }

        else if (r < 0)
            cout << "error" << endl;

        else {
            r *= (B.y - A.y);
            r %= E.p;           
            
            R->x = (r*r) - A.x - B.x;
            while(R->x < 0)
                R->x += E.p;
            R->x %= E.p;

            R->y = r*(A.x - R->x) - A.y;
            while(R->y < 0)
                R->y += E.p;
            R->y %= E.p;
        }       
    }
}

void EC_double(EC E, Point *R, Point A){
    int r = mod_inv(2*A.y, E.p);

    if(r==0){
        R->x = 0;
        R->y = 0;
    }

    else if(r < 0)
        cout << "error" << endl;
    
    else{
        r *= ((3*A.x*A.x) + E.a);
        r %= E.p;

        R->x = r*r - 2*A.x;
        while(R->x < 0)
            R->x += E.p;
        R->x %= E.p;

        R->y = r*(A.x - R->x) - A.y;
        while(R->y < 0)
            R->y += E.p;
        R->y %= E.p;
    } 
}

void bits_inv(vector<int> *bits, int r){ // bit값들을 저장할 벡터
    for(; r > 0; r /= 2) // 몫을 구해서 다음 비트를 계산할 수 있도록 r을 갱신
        (*bits).push_back(r % 2); // 나머지를 구해서 현재 숫자의 마지막 비트가 0인지 1인지 확인
}

void EC_mult(EC E, Point *R, Point A, int r){
    vector<int> bits;
    int n;

    if(A.x == 0 && A.y == 0){
        R->x = 0;
        R->y = 0;
    }

    else if(r <= 0)
        cout << "error" << endl;

    else{
        bits_inv(&bits, r);
        n = bits.size(); // 벡터의 크기

        *R = A; // 첫번째 비트(가장 왼쪽 비트) 초기화

        // 나머지 비트 처리
        for(; n > 1; n--){ // 
            EC_double(E, R, *R); // 현재 점을 두 배로
            if(bits.at(n-2) == 1) // 다음 비트가 1인지 확인
                EC_add(E, R, *R, A); // A 더함
        }
    }
}