#include <iostream>

using namespace std;

string encode(string msg, int r){
    for(char &c : msg)
        c += r;
    return msg;
}

string decode(string msg, int r){
    for(char &c : msg)
        c-=r;
    return msg;
}

int main(){
    string str = "hello";
    string s = encode(str, 3);
    cout << s << endl;

    string res = decode(s, 3);
    cout << res << endl;
}