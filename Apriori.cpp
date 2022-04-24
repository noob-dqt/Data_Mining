#include <bits/stdc++.h>
using namespace std;
const int N = 1e5+5;
//使用vector存储单个项集
int r,mx; //总共有r行数据，数据集中最大的数字是mx
int cnt[N];     //统计一项集
int surp;       //支持度
vector<vector<int>>res;    //结果集，一个vector是一个项集
bitset<N>sear,its;            //扫描数据库时用于存放一行数据状态，its存放项集
string s;
set<vector<int>>L,C;
ifstream fin("./C++/retail.dat");
void getnum(){
    int x=0;
    for(char c:s){
        if(c>='0'&&c<='9'){
            x=x*10+c-'0';    
        }
        else{
            cnt[x]++;
            mx=max(mx,x);
            x=0; 
        }
    }
    if(s.back()!=' '){
        cnt[x]++;
        mx=max(mx,x);
    }
}
void read(){
    if(!fin.is_open()){
        cout<<"there is no such file!\n";
        system("pause");
        exit(0);
    }
    while (getline(fin,s))
    {
        getnum();
        r++;
    }
}
bool isvaild(vector<int>a,vector<int>b){    //ab长度为n，则ab前n-1个数字必须相同才能合并
    int n=a.size();
    for(int i=0;i<n-1;i++){
        if(a[i]!=b[i]) return false;
    }
    return true;
}
bool check(vector<int>a){       //检查m项集(a)的所有m-1项集中是否都存在于Li-1中
    //m-项集中包含m个m-1项集
    vector<int>b;
    int n=a.size();
    for(int i=0;i<n;i++){
        b.clear();
        for(int j=0;j<n;j++){
            if(j==i) continue;
            b.push_back(a[j]);
        }
        if(L.find(b)==L.end()) return false;    //不符合条件的项集舍去
    }
    return true;
}
bool iscount(vector<int>a){         //扫描数据库，检查a是否是频繁项集
    its.reset();
    for(int num:a) its[num]=1;
    int cnt=0,x;
    fin.clear();
    fin.seekg(0);
    while (getline(fin,s))
    {
        sear.reset();
        x=0;
        for(char c:s){
            if(c>='0'&&c<='9'){
                x=x*10+c-'0';
            }
            else{
                sear[x]=1;
                x=0;
            }
        }
        if(s.back()!=' '){
            sear[x]=1;
        }
        if((sear&its)==its) cnt++;
    }
    return cnt>=surp;
}
int main()
{
    read();     //读取数据
    cout<<"please input the minimum support:"<<endl;
    double zp;
    cin>>zp;
    surp=ceil(r*zp);
    //先处理出L1，从小到大遍历整理成有序的
    for(int i=0;i<=mx;i++){
        if(cnt[i]>=surp){
            L.insert({i});
        }
    }
    vector<int>tp;          //产生临时项集
    while (L.size()>=1)      //等于1时不会再产生新的频繁项集
    {
        C.clear();          //清空Ci-1的项集
        for(auto tp:L) res.push_back(tp);  //L中结果放入结果集
        //Li生成新的Ci+1集合
        for(auto i=L.begin();i!=L.end();i++){
            auto j=i;j++;
            for(;j!=L.end();j++){
                if(!isvaild(*i,*j)) break;  //剪枝，数字有序后面的都不可能产生频繁项集
                //否则将a、b合并，只用将a、b前n-1个数取出，再将a,b末尾分别插入新项集
                tp.clear();
                for(int k=0;k<(*i).size();k++) tp.push_back((*i)[k]);
                tp.push_back((*j).back());
                //检查新生成的m项集tp的所有m-1项集是否存在于Li中，若不存在则抛弃，若符合条件则放入Ci+1中
                if(!check(tp)) continue;
                C.insert(tp);
            }
        }
        //Ci+1中筛选出Li+1
        //扫描数据库，并统计每一项集的支持度
        L.clear();      //清空L，空间重复利用，将新产生的频繁项集存入L
        for(auto tp:C){
            if(iscount(tp)) L.insert(tp);
        }
        if(L.size()==1) {res.push_back(*L.begin());break;}     //特殊处理最后一项
    }
    cout<<"total: "<<res.size()<<" items\n";
    for(auto tp:res){
        printf("{ ");
        for(int i:tp) printf("%d ",i);
        printf("}\n");

    }
    fin.close();
    return 0;
}
