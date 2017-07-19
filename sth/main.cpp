//
//  main.cpp
//  final_project
//
//  Created by Daniel/Clarence on 2015/6/13.
//  Copyright (c) 2015年 mithril. All rights reserved.
//
//bonus:
//1.when u input wrong password for the same ID 5 times continuously, u have to wait for 3 minute before trying again.
//2.change password: change ID old_pw new_pw
//3.logout: logout
//4.check balance: check

#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <algorithm>
#include <regex>
#include "md5.h"
#include <stdio.h>
#include <map>
#include <time.h>
//extern "C" {
//#include "bst.h"
//}

using namespace std;

char characters[62] = {'0','1','2','3','4','5','6','7','8','9',
    'A','B','C','D','E','F','G','H','I','J','K','L','M','N',
    'O','P','Q','R','S','T','U','V','W','X',
    'Y','Z','a','b','c','d','e','f','g','h','i','j',
    'k','l','m','n','o','p','q','r','s','t',
    'u','v','w','x','y','z'};

struct history{
    //    string id;
    unsigned rank;
    unsigned long long deal;
    bool income;
    unsigned timestamp;
    history(unsigned long long d, bool t, unsigned _time, unsigned a){
        //        id = i;
        deal = d;
        income = t;
        timestamp = _time;
        rank = a;
    }
    bool operator<(history &b){
        return this->timestamp < b.timestamp;
    }
};

struct node{
    unsigned rank;
    vector<unsigned> merged_rank;
    unsigned long long money;
    string pw;
    node(){
        money = 0;
        pw = "";
    }
};

struct name_sort{
    string name;
    int sc;
    name_sort(string a, int b){
        name = a;
        sc = b;
    }
    bool operator<(name_sort &b){
        if (this->sc < b.sc) {
            return true;
        }
        else if (this->sc > b.sc) {
            return false;
        }
        else {
            if (strcmp((this->name).c_str(), (b.name).c_str())<0){
                return true;
            }
            else {
                return false;
            }
        }
    }
};

bool name_compare(name_sort a, name_sort b){
    return (a < b);
}

bool word_compare(string a, string b){
    return strcmp(a.c_str(), b.c_str())<0?true:false;
}

bool hist_compare(history a, history b){
    return (a<b);
}

inline int score(const string &a, const string &b){
    int result = 0;
    unsigned long d = a.length()>b.length()?(a.length()-b.length()):(b.length()-a.length());
    unsigned long L = a.length()>b.length()?b.length():a.length();
    for (unsigned long i = 0; i<L; ++i) {
        result += (a[i]!=b[i])?(L-i):0;
    }
    for (unsigned long i=1; i<d+1; ++i) {
        result += i;
    }
    return result;
}

bool _stringmatch(const char *pattern, unsigned long patternLen,
                  const char *str, unsigned long stringLen)
{
    while(patternLen) {
        switch(pattern[0]) {
            case '*':
                if (patternLen == 1)
                    return true;
                while(stringLen) {
                    if (_stringmatch(pattern+1, patternLen-1,
                                     str, stringLen))
                        return true;
                    ++str;
                    --stringLen;
                }
                return false;
                break;
            case '?':
                if (stringLen == 0)
                    return false;
                ++str;
                --stringLen;
                break;
            default:
                if (pattern[0] != str[0])
                    return false;
                ++str;
                --stringLen;
                break;
        }
        ++pattern;
        patternLen--;
        if (stringLen == 0) {
            while(*pattern == '*') {
                ++pattern;
                --patternLen;
            }
            break;
        }
    }
    if (patternLen == 0 && stringLen == 0)
        return true;
    return false;
}

bool stringmatch(const char *pattern, const char *str) {
    return _stringmatch(pattern, strlen(pattern), str, strlen(str));
}

class bank_account {
private:
    node info;
public:
    vector<history> hist;
    
    bank_account(){}
    bank_account(unsigned a, string password, unsigned long long m){
        info.rank = a;
        info.pw = md5(password) ;
        info.money = m;
        //        printf("%s ,%llu\n", info.pw.c_str(), info.money);
    }
    void changepw(string pw)
    {
        info.pw = md5(pw);
    }
    unsigned find_rank(){
        return info.rank;
    }
    vector<unsigned>& find_merged_rank(){
        return info.merged_rank;
    }
    unsigned long long deposit(unsigned long long a){
        (info.money) += a;
        return info.money;
    }
    
    unsigned long long check(){
        return info.money;
    }
    
    bool withdraw(unsigned long long m){
        if (m > info.money) {
            printf("fail, %llu dollars only in current account\n", info.money);
            return false;
        }
        else {
            info.money -= m;
            printf("success, %llu dollars left in current account\n", info.money);
            return true;
        }
    }
    void add_hist(unsigned long long m, bool in, unsigned t, unsigned a){
        history tmp(m,in, t,a);
        hist.push_back(tmp);
    }
    void search(string name, vector<string> &existed_ids){
        vector<history>::iterator itr;
        bool record = false;
        for (itr = hist.begin(); itr != hist.end(); ++itr) {
            if (existed_ids[(*itr).rank] == name) {
                record = true;
                if ((*itr).income) {
                    printf("From %s %llu\n", name.c_str(), (*itr).deal);
                }
                else {
                    printf("To %s %llu\n", name.c_str(), (*itr).deal);
                }
            }
        }
        if (!record) {
            printf("no record\n");
        }
    }
    bool right_pw(string p){
        string m = md5(p);
        if (info.pw.compare(m) == 0) {
            return true;
        }
        else {
            return false;
        }
    }
};

class bank_system{
private:
    time_t last_time;
    time_t current_time;
    int gap;
    int wrong_count;
    string last_wrong_id;
    string locked_id;
    unsigned timestamp;
    //    unsigned id_index;
    //    map<string, bank_account, word_compare> u;
    unordered_map<string, bank_account> usrs;
    unordered_map<string, bank_account>::iterator current_user;
public:
    
    vector<string> existed_ids;
    
    bank_system(){
        current_time = 0;
        last_time = 0;
        gap = 180;
        wrong_count = 0;
        last_wrong_id = "";
        locked_id = "";
        timestamp=0;
        //        id_index=0;
        current_user = usrs.end();
    }
    
    void login(string id, string pw){
        if(wrong_count > 2 && last_wrong_id == id)
        {
            locked_id = last_wrong_id;
            wrong_count = 0;
            const time_t t = time(NULL);
            last_time = t;
            printf("wrong password, try again 3 minutes later\n");
            return;
        }
        if(locked_id == id){
            const time_t t2 = time(NULL);
            current_time = t2;
            if(current_time-last_time < gap){
                printf("try again 3 minutes later\n");
                return;
            }
            else
            {
                last_time = 0;
                current_time = 0;
                locked_id = "";
            }
        }
        
        unordered_map<string, bank_account>::iterator result = usrs.find(id);
        if (result == usrs.end()) {
            printf("ID %s not found\n", id.c_str());
            //            current_user = usrs.end();
        }
        else {
            if ((*result).second.right_pw(pw)) {
                printf("success\n");
                //                (*result).second.deposit(0);
                current_user = usrs.find(id);
            }
            else {
                printf("wrong password\n");
                if(last_wrong_id == id)
                    wrong_count++;
                else
                    wrong_count = 0;
                last_wrong_id = id;
                //                current_user = usrs.end();
            }
        }
    }
    
    void logout()
    {
        current_user = usrs.end();
        printf("success, logged out\n");
    }
    
    void check()
    {
        if (current_user == usrs.end()) {
            printf("fail, please login\n");
        }
        else
        {
            printf("success, account balance is %llu dollars\n",(*current_user).second.check());
        }
    }
    
    void change(string id, string pw, string new_pw)
    {
        unordered_map<string, bank_account>::iterator result = usrs.find(id);
        if (result == usrs.end()) {
            printf("ID %s not found\n", id.c_str());
            //            current_user = usrs.end();
        }
        else {
            if ((*result).second.right_pw(pw)) {
                (*result).second.changepw(new_pw);
                printf("success, password changed\n");
                //                (*result).second.deposit(0);
                //                current_user = usrs.find(id);
                
            }
            else {
                printf("wrong password\n");
                //                current_user = usrs.end();
            }
        }
    }
    
    void create(string id, string pw){
        if (usrs.find(id) != usrs.end()) {
            printf("ID %s exists", id.c_str());
            recommand_id(id);
        }
        else {
            printf("success\n");
            bank_account tmp(existed_ids.size(), pw,0);
            usrs.insert(make_pair(id, tmp));
            existed_ids.push_back(id);
            //            id_index++;
            //            usrs.insert({id, tmp});
        }
    }
    
    void del(string id, string pw){
        if (usrs.find(id) == usrs.end()) {
            printf("ID %s not found\n", id.c_str());
        }
        else {
            if ((*usrs.find(id)).second.right_pw(pw)) {
                printf("success\n");
                usrs.erase(id);
            }
            else {
                printf("wrong password\n");
            }
        }
    }
    
    //    friend bool id_used(string);
    bool whether_id_used(string a){
        unordered_map<string, bank_account>::iterator itr = usrs.find(a);
        if (itr == usrs.end()) {
            return false;
        }
        else {
            return true;
        }
    }
    void recommand_id(const string a){
        vector<name_sort> names;
        //        vector<name_sort> result;
        int i = 0;
        int j = 0;
        name_sort tmp(a.substr(0, a.length()-1), 1);
        //        printf("%s\n",tmp.c_str());
        if (tmp.name.size() > 0) {
            if (!whether_id_used(tmp.name)) {
                names.push_back(tmp);
                ++i;
            }
        }
        
        
        while (j<62) {
            tmp.name = a.substr(0, a.length()-1) + characters[j];
            if (characters[j] != a[a.length()-1]) {
                names.push_back(tmp);
                //                printf("%s\n",tmp.c_str());
                ++i;
            }
            else {
                int jj = 0;
                while (jj<62) {
                    tmp.name = a + characters[jj];
                    
                    if (!whether_id_used(tmp.name)) {
                        names.push_back(tmp);
                        ++i;
                        if (i == 10) {
                            break;
                        }
                    }
                    ++jj;
                }
            }
            if (i == 10) {
                break;
            }
            ++j;
        }
        
        //        j=0;
        
        
        //        if (i<10) {
        //            j=0;
        //            while (j<62) {
        //                tmp.name = a.substr(0,a.length()-2) + characters[j] + a[a.length()-1];
        //                tmp.sc = 2;
        //                if (characters[j] != a[a.length()-2]) {
        //                    names.push_back(tmp);
        //                    i++;
        //                }
        //                j++;
        //            }
        //        }
        //        printf("%d\n", i);
        //        printf("%s \n", result[i-1].c_str());
        //        sort(result.begin(), result.end());
        //        sort(names.begin(), names.end(), name_compare);
        for (int k=0; k<10; ++k) {
            if (k == 0) {
                printf(", %s", names[k].name.c_str());
                continue;
            }
            printf(",%s", names[k].name.c_str());
        }
        printf("\n");
    }
    
    void transfer(string id, unsigned long long m){
        if (usrs.find(id) != usrs.end()) {
            if ((*current_user).second.withdraw(m)) {
                ++timestamp;
                (*usrs.find(id)).second.deposit(m);
                (*current_user).second.add_hist(m, false,timestamp, (*usrs.find(id)).second.find_rank());
                (*usrs.find(id)).second.add_hist(m, true, timestamp,(*current_user).second.find_rank());
            }
        }
        else {
            vector<name_sort> names;
            //            int i = 0;
            name_sort tmp("", 1000);
            for (auto itr = usrs.begin(); itr!=usrs.end(); ++itr) {
                //                name_sort tmp((*itr).first, score((*itr).first, id));
                tmp.name = (*itr).first;
                tmp.sc = score((*itr).first, id);
                if (itr == usrs.begin()) {
                    names.push_back(tmp);
                    continue;
                }
                if ((names.back() < tmp) && (names.size()<10)) {
                    names.push_back(tmp);
                }
                else {
                    for (auto itr2 = names.begin(); itr2!=names.end(); ++itr2) {
                        if (tmp < (*itr2)) {
                            names.insert(itr2, tmp);
                            break;
                        }
                    }
                }
                if (names.size()>10) {
                    names.pop_back();
                }
            }
            //            for (int i = 0; i<100; i++) {
            //                if (i == 0) {
            //                    a.push_back(b[i]);
            //                    continue;
            //                }
            //                for (auto itr = a.begin(); itr!=a.end(); itr++) {
            //                    if (b[i]<(*itr)) {
            //                        a.insert(itr, b[i]);
            //                        break;
            //                    }
            //                }
            //                if (a.size()>10) {
            //                    a.pop_back();
            //                }
            //            }
            //            sort(names.begin(), names.end(), name_compare);
            size_t num = names.size()>10?10:names.size();
            printf("ID %s not found", id.c_str());
            for (int k=0; k<num; ++k) {
                if (k == 0) {
                    printf(", %s", names[k].name.c_str());
                    continue;
                }
                printf(",%s", names[k].name.c_str());
            }
            printf("\n");
        }
    }
    
    friend bool stringmatch(const char *pattern, const char *str);
    friend bool _stringmatch(const char *pattern, unsigned long patternLen,
                             const char *str, unsigned long stringLen);
    
    void my_find(const string wild_id){
        vector<string> ids;
        for (auto itr = usrs.begin(); itr!=usrs.end(); ++itr) {
            if ((*itr).first != (*current_user).first) {
                if (stringmatch(wild_id.c_str(), (*itr).first.c_str())) {
                    ids.push_back((*itr).first);
                }
            }
        }
        sort(ids.begin(), ids.end(), word_compare);
        for (auto itr = ids.begin(); itr != ids.end(); ++itr) {
            if (itr == ids.begin()) {
                printf("%s", (*itr).c_str());
                continue;
            }
            printf(",%s", (*itr).c_str());
        }
        printf("\n");
    }
    
    void merge(string id1, string pw1, string id2, string pw2){
        unordered_map<string, bank_account>::iterator itr1 = usrs.find(id1);
        unordered_map<string, bank_account>::iterator itr2 = usrs.find(id2);
        if (itr1 == usrs.end()) {
            printf("ID %s not found\n", id1.c_str());
            //            current_user = usrs.end();
        }
        else if (itr2 == usrs.end()){
            printf("ID %s not found\n", id2.c_str());
        }
        else {
            if (!(*itr1).second.right_pw(pw1)) {
                printf("wrong password1\n");
                
            }
            else if (!(*itr2).second.right_pw(pw2)){
                printf("wrong password2\n");
            }
            else {
                printf("success, %s has %llu dollars\n", (*itr1).first.c_str(), (*itr1).second.deposit((*itr2).second.deposit(0)));
                //                for (auto i = usrs.begin(); i != usrs.end(); i++) {
                //                    for (auto j = (*i).second.hist.begin(); j!=(*i).second.hist.end(); j++) {
                //                        if ((*j).id == id2 && (*j).rank == (*itr2).second.find_rank()) {
                //                            (*j).id = id1;
                //                            (*j).rank = (*itr1).second.find_rank();
                //                        }
                //                    }
                //                }
                existed_ids[(*itr2).second.find_rank()] = id1;
                ((*itr1).second.find_merged_rank()).push_back((*itr2).second.find_rank());
                for (auto i = ((*itr2).second.find_merged_rank()).begin(); i!=((*itr2).second.find_merged_rank()).end(); ++i) {
                    existed_ids[*i] = id1;
                    ((*itr1).second.find_merged_rank()).push_back(*i);
                }
                (*itr1).second.hist.insert((*itr1).second.hist.end(), (*itr2).second.hist.begin(), (*itr2).second.hist.end());
                stable_sort((*itr1).second.hist.begin(), (*itr1).second.hist.end(), hist_compare);
                usrs.erase(id2);
            }
        }
    }
    
    void deposit(unsigned long long a){
        printf("success, %llu dollars in current account\n", (*current_user).second.deposit(a));
    }
    
    void withdraw(unsigned long long a){
        (*current_user).second.withdraw(a);
    }
    
    void search(string name){
        (*current_user).second.search(name, existed_ids);
    }
    
};



//typedef unordered_map<string, node> bank_acc;

int main(int argc, const char * argv[]) {
    // insert code here...
        freopen("/Users/clarence/Desktop/DSA_final/release_data/test_in_small.txt", "r", stdin);
    char com[10];
    char p[4][128];
    bank_system sys1;
    //    string command;
    //    string params[5];
    unsigned long long params2[5];
    while (scanf("%s", com) != EOF){
        //    while(1){
        
        //        scanf("%", &x)
        //        std::cin>>command;
        //        scanf("%s", com);
        //        if(std::cin.eof()){
        //            break;
        //        }
        if (!strcmp(com, "login")) {
            //        if (!memcmp(com, "l",1)) {
            //        if (com[3] == 'i') {
            //            cin>>params[0];
            //            cin>>params[1];
            //            sys1.login(params[0], params[1]);
            scanf("%s", p[0]);
            scanf("%s", p[1]);
            sys1.login(p[0], p[1]);
            continue;
        }
        else if (!strcmp(com, "create")){
            //        else if (!memcmp(com, "c",1)){
            //        else if (com[3] == 'a') {
            //            cin>>params[0];
            //            cin>>params[1];
            //            sys1.create(params[0], params[1]);
            scanf("%s", p[0]);
            scanf("%s", p[1]);
            sys1.create(p[0], p[1]);
            continue;
        }
        else if (!strcmp(com, "delete")){
            //        else if (!memcmp(com, "del",3)){
            //        else if (com[3] == 'e') {
            //            cin>>params[0];
            //            cin>>params[1];
            //            sys1.del(params[0], params[1]);
            scanf("%s", p[0]);
            scanf("%s", p[1]);
            sys1.del(p[0], p[1]);
            continue;
        }
        else if (!strcmp(com, "merge")){
            //        else if (!memcmp(com, "m",1)){
            //        else if (com[3] == 'g') {
            //            cin>>params[0];
            //            cin>>params[1];
            //            cin>>params[2];
            //            cin>>params[3];
            scanf("%s", p[0]);
            scanf("%s", p[1]);
            scanf("%s", p[2]);
            scanf("%s", p[3]);
            sys1.merge(p[0], p[1], p[2], p[3]);
            continue;
        }
        else if (!strcmp(com, "deposit")){
            //        else if (!memcmp(com, "dep",3)){
            //        else if (com[3] == 'o') {
            //            cin>>params2[0];
            scanf("%llu", &params2[0]);
            sys1.deposit(params2[0]);
            continue;
        }
        else if (!strcmp(com, "withdraw")){
            //        else if (!memcmp(com, "w",1)){
            //        else if (com[3] == 'h') {
            //            cin>>params2[0];
            scanf("%llu", &params2[0]);
            sys1.withdraw(params2[0]);
            continue;
        }
        else if (!strcmp(com, "transfer")){
            //        else if (!memcmp(com, "t",1)){
            //        else if (com[3] == 'n') {
            //            cin>>params[0];
            //            cin>>params2[0];
            scanf("%s", p[0]);
            scanf("%llu", &params2[0]);
            sys1.transfer(p[0], params2[0]);
            continue;
        }
        else if (!strcmp(com, "find")){
            //        else if (!memcmp(com, "f",1)){
            //        else if (com[3] == 'd') {
            //            cin>>params[0];
            scanf("%s", p[0]);
            sys1.my_find(p[0]);
            continue;
        }
        else if (!strcmp(com, "search")){
            //        else if (!memcmp(com, "s",1)){
            //        else if (com[3] == 'r') {
            //            cin>>params[0];
            scanf("%s", p[0]);
            sys1.search(p[0]);
            continue;
        }
        else if(!strcmp(com, "change"))
        {
            scanf("%s",p[0]);
            scanf("%s",p[1]);
            scanf("%s",p[2]);
            sys1.change(p[0],p[1],p[2]);
            continue;
        }
        else if(!strcmp(com, "logout"))
        {
            sys1.logout();
            continue;
        }
        else if(!strcmp(com, "check"))
        {
            sys1.check();
            continue;
        }
    }
    //    sys1.create("lyx", "123456");
    //    sys1.create("ly", "12");
    //    sys1.create("ly2", "12");
    //    sys1.create("ly3", "12");
    //    //    unordered_map<string, bank_account>::iterator tmp;
    //    sys1.login("lyx", "12345");
    //    sys1.login("ly", "12345");
    //    sys1.login("lyx", "123456");
    //
    //    sys1.deposit(100000);
    //    sys1.withdraw(1000000);
    //    sys1.withdraw(100);
    //    sys1.transfer("ly2", 10);
    //    sys1.transfer("ly", 1000);
    //    sys1.transfer("lx", 1000);
    //    sys1.search("l");
    //    sys1.search("ly");
    //
    //    sys1.login("ly", "12");
    //    sys1.transfer("ly2", 100);
    //    sys1.deposit(10);
    //    sys1.withdraw(100);
    //    sys1.search("lyx");
    //
    //    sys1.my_find("l*");
    //    sys1.merge("ly", "123456", "lyx", "12");
    //    sys1.merge("ly", "12", "lyx", "12");
    //    sys1.merge("ly", "12", "lyx", "123456");
    //    sys1.search("ly");
    //    sys1.search("ly2");
    //    sys1.login("ly", "12");
    //    vector<name_sort> test;
    //    name_sort kk("lyx", 1);
    //    test.push_back(kk);
    //    cout<<test[0].name<<" "<<test[0].sc<<endl;
    //    kk.name = "ly";
    //    kk.sc = 2;
    //    test.push_back(kk);
    //    cout<<test[0].name<<" "<<test[0].sc<<endl;
    //    cout<<test[1].name<<" "<<test[1].sc<<endl;
    //    cout<<score("abcd", "abcde")<<endl;
    //    string a = "abcd";
    
    //    cout<<a[a.length()-1]<<endl;
    //    cout<<strcmp("abc", "abcz")<<endl;
    //    cout<<word_compare("abcd", "abc;")<<endl;
}

