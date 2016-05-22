#include <iostream>
#include <string>
#include <crypt.h>
using namespace std;

int main(){
	string a,salt;
	salt="aa";
	cin>>a;
	cout<<a<<" "<<salt<<" "<<string(crypt(a.c_str(),salt.c_str()))<<endl; 
}