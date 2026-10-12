// 타원곡선 구현 : int

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
void EC_add(EC E, Point* R, Point P, Point Q);
void EC_double(EC E, Point* R, Point P);
void bits_inv(vector<int> *bits, int r);

int main() {
    int x = 5;
    int mod = 7;

    // 타원곡선 y^2 = x^3 + ax + b mod p
    // A(1, 4), B(3, 1)가 속하는 곡선:
    // y^2 = x^3 + 4x + 4 mod 7
    EC ec = {4, 4, 7};

    // 연산에 사용할 점
    Point A = {1, 4};
    Point B = {3, 1};

    // 덧셈과 배점 결과가 서로 덮어쓰이지 않도록
    // 각각 별도의 변수에 저장
    Point sum = {0, 0};
    Point doubled = {0, 0};

    EC_add(ec, &sum, A, B);
    EC_double(ec, &doubled, A);

    cout << "result : " << mod_inv(x, mod) << endl;
    cout << "A = : (" << A.x << ", " << A.y << ")" << endl;
    cout << "B = : (" << B.x << ", " << B.y << ")" << endl;
    cout << "A + B = : (" << sum.x << ", " << sum.y << ")" << endl;
    cout << "2A = : (" << doubled.x << ", " << doubled.y << ")" << endl;

    return 0;
}

int mod_inv(int x, int mod) {
    int r = 0;

    // mod가 0 이하이면 모듈러 연산을 수행할 수 없음
    if (mod <= 0)
        return -1;

    // x를 0 이상 mod 미만의 값으로 정규화
    x %= mod;

    while (x < 0) {
        x += mod;
    }

    // 0은 곱셈의 역원이 존재하지 않음
    // addition에서 x2 - x1이 0인 경우에도 발생할 수 있음
    if (x == 0)
        return 0;

    // 곱셈의 역원을 완전 탐색으로 계산
    for (r = 1; r < mod; r++) {
        if ((x * r) % mod == 1) {
            return r;
        }
    }

    // 곱셈의 역원이 존재하지 않음
    return -1;
}

void EC_add(EC E, Point* R, Point P, Point Q) {
    // P와 Q는 서로 다른 점이라는 것을 가정
    // 기울기:
    // lambda = (y2 - y1) * (x2 - x1)^-1 mod p
    int r = mod_inv(Q.x - P.x, E.p);

    if (r == 0) {
        // x1 == x2인 경우 분모의 역원이 존재하지 않음
        // P와 Q가 역원 관계이면 결과는 무한원점
        // 현재 구조체는 무한원점을 표현할 수 없으므로 (0, 0)으로 표현
        R->x = 0;
        R->y = 0;
    }
    else if (r < 0) {
        // -1이 반환되는 경우 역원이 존재하지 않거나
        // 입력된 mod 값이 잘못된 경우
        cout << "Error" << endl;
        R->x = 0;
        R->y = 0;
    }
    else {
        // lambda = (y2 - y1) / (x2 - x1) mod p
        r *= (Q.y - P.y);
        r %= E.p;

        while (r < 0) {
            r += E.p;
        }

        // x3 = lambda^2 - x1 - x2 mod p
        R->x = (r * r - P.x - Q.x) % E.p;

        // 음수는 양수로 바꿔서 반환
        while (R->x < 0) {
            R->x += E.p;
        }

        // mod p를 넘어가는 경우를 방지하기 위해 mod 연산 수행
        R->x %= E.p;

        // y3 = lambda(x1 - x3) - y1 mod p
        R->y = (r * (P.x - R->x) - P.y) % E.p;

        // 음수는 양수로 바꿔서 반환
        while (R->y < 0) {
            R->y += E.p;
        }

        // mod p를 넘어가는 경우를 방지하기 위해 mod 연산 수행
        R->y %= E.p;
    }
}

void EC_double(EC E, Point* R, Point P) {
    // 배점 기울기:
    // lambda = (3x1^2 + a) * (2y1)^-1 mod p
    int r = mod_inv(2 * P.y, E.p);

    if (r == 0) {
        // y가 0일 때 접선은 수직선이 됨
        // 따라서 배점 결과는 무한원점
        // 현재 구조체는 무한원점을 표현할 수 없으므로 (0, 0)으로 표현
        R->x = 0;
        R->y = 0;
    }
    else if (r < 0) {
        // -1이 반환되는 경우 역원이 존재하지 않거나
        // 입력된 mod 값이 잘못된 경우
        cout << "Error" << endl;
        R->x = 0;
        R->y = 0;
    }
    else {
        // lambda = (3x1^2 + a) / (2y1) mod p
        r *= (3 * P.x * P.x + E.a);
        r %= E.p;

        while (r < 0) {
            r += E.p;
        }

        // x3 = lambda^2 - 2x1 mod p
        R->x = (r * r - 2 * P.x) % E.p;

        // 음수는 양수로 바꿔서 반환
        while (R->x < 0) {
            R->x += E.p;
        }

        // mod p를 넘어가는 경우를 방지하기 위해 mod 연산 수행
        R->x %= E.p;

        // y3 = lambda(x1 - x3) - y1 mod p
        R->y = (r * (P.x - R->x) - P.y) % E.p;

        // 음수는 양수로 바꿔서 반환
        while (R->y < 0) {
            R->y += E.p;
        }

        // mod p를 넘어가는 경우를 방지하기 위해 mod 연산 수행
        R->y %= E.p;
    }
}

void bits_inv(vector<int> *bits, int r){
    for(; r > 0; r /= 2){
        bits->push_back(r % 2);
    }
}

void EC_mult(EC E, Point *R, Point P, int k) {
    vector<int> bits; 
    int n;// bits의 길이를 저장할 n 선언
    n = bits.size();

    if (k == 0) {
        R->x = 0;
        R->y = 0;
        return;
    }
    else {
        bits_inv(&bits, k); // r < 0
        n = bits.size();

        *R = P; // <- 1*P를 넣은 것 = b1은 항상 1이므로 항상 일어나기 때문에 for문에 넣지 않은 것
        // R->x = P.x;
        // R->y = P.y;

        for(; n > 0; n--){ {
            EC_double(E, R, *R);
            if (bits[n - 1] == 1) {
                EC_add(E, R, *R, P);
            }
        }
    }

    Point Q = P;
    for(int i = 1; i < bits.size(); i++){
        EC_double(E, &Q, Q); // n 값에 상관없이 doubling은 항상 일어나므로 for문 안에 넣음
        if(bits[i] == 1){
            EC_add(E, &Q, Q, P);
        }
    }
    *R = Q;
    }
}