#include <iostream>
#include <vector>
#include <string>

using namespace std; 

int main() {
    vector<string> msg {"Hello", "C++", "World", "form", "VS Code", "and the C++ extension!" };

    // msg 벡터에 있는 문자열들을 순차적으로 출력
    for (const string &word : msg) { // C언어 처럼 작성해도 무방. 범위 기반 for문으로 편하게 할 수도 있음
        cout << word << " ";
    }
    cout << endl;
}