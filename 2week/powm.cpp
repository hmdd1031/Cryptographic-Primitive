#include <iostream>
#include <vector>
using namespace std;

int powm(int base, int exp, int mod);
bool is_primitive(int base, int mod);
vector<int> primitive_roots(int mod);

int main() {
    int b, e, m, res;
    char comma1, comma2;

    // 사용자 입력
    cout << "input base, exp, mod: ";
    cin >> b >> comma1 >> e >> comma2 >> m;

    // 원시근 판별
    cout << (is_primitive(b, m) ? "true" : "false") << '\n';

    // 원시근 출력
    for(int i : primitive_roots(m))
        cout << i << ' ';
    cout << endl;

    // 결과 출력
    res = powm(b, e, m);
    cout << "result : " << res << '\n';

    return 0;
}

int powm(int base, int exp, int mod) {
    int r = 1;

    for (int i = 0; i < exp; i++) {
        r *= base;
        r %= mod;
    }

    return r;
}

bool is_primitive(int base, int mod) {
    for (int exp = 1; exp < mod - 1; exp++) {
        if (powm(base, exp, mod) == 1)
            return false;
    }
    return powm(base, mod - 1, mod) == 1;  
}

vector<int> primitive_roots(int mod){
    vector<int> v;
    for(int i = 2; i < mod; i++){
        if(is_primitive(i, mod))
            v.push_back(i);
    }
    return v;
}