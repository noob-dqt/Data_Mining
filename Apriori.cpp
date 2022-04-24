#include <bits/stdc++.h>
using namespace std;
const int N = 1e5+5;
//使用vector存储单个项集
int r;      //总共有r行数据
vector<unordered_set<int>>db(N);
vector<int>b;
int cnt[N];
int surp;
int mx;     //记录最大的数
vector<vector<int>>res;    //结果集，一个vector是一个项集
map<int,int>ans;
string s,path;
set<vector<int>>L,C;
double tsp=0.02;
// int tns;
void getnum(){
    int x=0;
    for(char c:s){
        if(c>='0'&&c<='9') x=x*10+c-'0';
        else{
            cnt[x]++;
            db[r].insert(x);
            mx=max(mx,x);
            x=0;
        }
    }
    if(s.back()!=' '){
        cnt[x]++;
        db[r].insert(x);
        mx=max(mx,x);
    }
}
void read(){
    ifstream fin(path);
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
    fin.close();
}
bool check(vector<int>&a){       //检查m项集(a)的所有m-1项集中是否都存在于Li-1中
    //m-项集中包含m个m-1项集
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
bool iscount(vector<int>&a){         //扫描数据库，检查a是否是频繁项集
    int cnt=0,f;
    for(int i=0;i<r;i++){
        f=1;
        for(int j:a){
            if(!db[i].count(j)){
                f=0;break;
            }
        }
        cnt+=f;
    }
    return cnt>=surp;
}
int main()
{
    path="./C++/retail.dat";
    read();     //读取数据
    // cout<<"please input the minimum support(0~1):"<<endl;
    // cin>>tsp;
    surp=ceil(tsp*r);
    //先处理出L1，从小到大遍历整理成有序的
    for(int i=0;i<=mx;i++){
        if(cnt[i]>=surp){
            L.insert({i});
        }
    }
    vector<int>tp;          //产生临时项集
    bool f;
    while (L.size()>=1)      //等于1时不会再产生新的频繁项集
    {
        
        C.clear();          //清空Ci-1的项集
        for(auto v:L) 
        {
            // tns++;
            res.push_back(v);  //L中结果放入结果集
            ans[v.size()]++;
        }
        //Li生成新的Ci+1集合
        for(auto i=L.begin();i!=L.end();i++){
            auto j=i;j++;
            for(;j!=L.end();j++){
                f=0;
                for(int k=0;k<(*i).size()-1;k++){
                    if((*i)[k]!=(*j)[k]){
                        f=1;
                        break;
                    }
                }
                if(f) break;  //剪枝，数字有序后面的都不可能产生频繁项集
                //否则将a、b合并，只用将a、b前n-1个数取出，再将a,b末尾分别插入新项集
                tp=*i;
                tp.push_back((*j).back());
                //检查新生成的m项集tp的所有m-1项集是否存在于Li中，若不存在则抛弃，若符合条件则放入Ci+1中
                if(!check(tp)) continue;
                C.insert(tp);
            }
        }
        //Ci+1中筛选出Li+1
        //扫描数据库，并统计每一项集的置信度
        L.clear();      //清空L，空间重复利用，将新产生的频繁项集存入L
        for(auto tp:C){
            if(iscount(tp)) L.insert(tp);
        }
        if(L.size()==1) 
        {
            // tns++;
            ans[L.begin()->size()]++;
            res.push_back(*L.begin());     //特殊处理最后一项
            break;
        }
            
    }
    // cout<<"total: "<<tns<<" items\n";
    cout<<"total: "<<res.size()<<" items\n";
    for(auto v:ans){
        printf("%d-frequent item: %d\n",v.first,v.second);
    }
    ofstream outf("./C++/Apriori.txt");
    outf<<"total: "<<res.size()<<" items\n";
    for(auto v:ans)
        outf<<v.first<<"frequent item: "<<v.second<<'\n';
    for(auto tp:res){
        outf<<"{ ";
        for(int i:tp) outf<<i<<' ';
        outf<<"}\n";
    }
    outf.close();
    return 0;
}
