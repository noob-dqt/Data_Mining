#include<bits/stdc++.h>
using namespace std;
struct fpnode
{
    int item;       //项
    int count;      //计数
    fpnode *fa;     //父节点
    map<int,fpnode*>sons;   //子孙节点
    fpnode *next;   //指向item下一个位置
    fpnode(int x,int y){
        sons.clear();
        item=x;
        count=y;
        fa=nullptr;
        next=nullptr;
    }
    fpnode(){
        sons.clear();
        item=-1;
        count=0;
        fa=nullptr;
        next=nullptr;
    }
};
struct node         //头表节点
{
    int item;
    int count;
    fpnode * head;
    node(){
        head=nullptr;
        count=0;
        item=-1;
    }
    node(int x,int y){
        head=nullptr;
        item=x;
        count=y;
    }
    bool operator<(const node t)const{
        if(count==t.count) return item<t.item;
        return count>t.count;
    }
};
class FP_Tree
{
public:
    bool flag;              //标记树是否是单分支树,true为单分支
    fpnode *root;           //FP树根
    vector<node>Header_Table;
    FP_Tree(){
        root=new fpnode(-1,0);      //-1标识根节点
        flag=1;
        Header_Table.clear();
    }
    void predel(fpnode* rt){        //析构函数
        if(rt==nullptr) return ;
        for(auto it:rt->sons)
            predel(it.second);
        rt->fa=nullptr;
        rt->next=nullptr;
        rt->sons.clear();
        delete rt;
    }
    ~FP_Tree(){
        predel(root);
    }
};
string path,s;        //path存放路径，s存放读取的单行数据
int row,minsup;         //row数据总行数，minsup最小支持度
map<int,fpnode*>mp;     //建树工具
unordered_map<int,int>cnt;
vector<vector<int>>res;
// map<int,int>ans;
vector<vector<int>>db;  //条件模式基
vector<int>alpha;
vector<int>Beta;
vector<int>tmp;         //建树存每条事务
int tns;
double sur;
bool cmp(int x,int y){
    if(cnt[x]==cnt[y]) return x<y;
    return cnt[x]>cnt[y];
}
FP_Tree first_construct(){
    ifstream fin(path);
    if(!fin.is_open()){
        cout<<"there is no such file!\n";
        system("pause");
        exit(-1);
    }
    cnt.clear();
    while (getline(fin,s)){
        if(s==" "||s.empty()) continue;
        int x=0;
        for(char c:s){
            if(c>='0'&&c<='9') x=x*10+c-'0';
            else{
                cnt[x]++;
                x=0;
            }
        }
        if(s.back()!=' ') cnt[x]++;
        row++;
    }
    
    // cout<<"please input the minimum support(0-1):\n";
    // cin>>sur;
    if(sur<0||sur>1) {
        cout<<"wrong support input";
        system("pause");
        exit(-1);
    }
    minsup=ceil(sur*row);     //计算支持度
    fin.clear();
    fin.seekg(0);
    FP_Tree tree;
    for(auto v:cnt)
        if(v.second>=minsup) tree.Header_Table.push_back(node(v.first,v.second));
    //第二次扫描建树
    sort(tree.Header_Table.begin(),tree.Header_Table.end());
    fpnode* p;
    while (getline(fin,s)){
        tmp.clear();
        p=tree.root;
        if(s==" "||s.empty()) continue;
        int x=0;
        for(char c:s){
            if(c>='0'&&c<='9') x=x*10+c-'0';
            else{
                if(cnt[x]>=minsup) tmp.push_back(x);
                x=0;
            }
        }
        if(s.back()!=' ') if(cnt[x]>=minsup) tmp.push_back(x);
        sort(tmp.begin(),tmp.end(),cmp);
        for(int x:tmp){
            if(p->sons.count(x)){   //已经存在
                p=p->sons[x];
                p->count++;
            }
            else{
                fpnode *newnd=new fpnode(x,1);
                if(!mp.count(x)){
                    for(auto &v:tree.Header_Table){
                        if(v.item==x){
                            v.head=newnd;
                            break;
                        }
                    }
                }
                else mp[x]->next=newnd;
                mp[x]=newnd;
                newnd->fa=p;
                p->sons[x]=newnd;
                if(tree.flag&&p->sons.size()>1) tree.flag=0;
                p=newnd;
            }
        }
    }
    return tree;
}
FP_Tree build(){    //利用条件db建树
    cnt.clear();
    mp.clear();
    for(auto &v:db)
        for(int i=0;i<v.size()-1;i++)
            cnt[v[i]]+=v.back();
    FP_Tree tree;
    for(auto &v:cnt)
        if(v.second>=minsup) tree.Header_Table.push_back(node(v.first,v.second));
    sort(tree.Header_Table.begin(),tree.Header_Table.end());
    fpnode* p;
    for(auto &v:db){
        tmp.clear();
        p=tree.root;
        for(int i=0;i<v.size()-1;i++) 
            if(cnt[v[i]]>=minsup) 
                tmp.push_back(v[i]);//最后一位是次数不能参与建树
        sort(tmp.begin(),tmp.end(),cmp);
        for(int x:tmp){
            if(p->sons.count(x)){   //已经存在
                p=p->sons[x];
                p->count+=v.back();
            }
            else{
                fpnode *newnd=new fpnode(x,v.back());
                if(!mp.count(x)){
                    for(auto &v:tree.Header_Table){
                        if(v.item==x){
                            v.head=newnd;
                            break;
                        }
                    }
                }
                else mp[x]->next=newnd;
                mp[x]=newnd;
                newnd->fa=p;
                p->sons[x]=newnd;
                if(tree.flag&&p->sons.size()>1) tree.flag=0;
                p=newnd;
            }
        }
    }
    return tree;
}
void FPgrowth(FP_Tree &tree){
    if(tree.flag){
        int len=tree.Header_Table.size();
        tns+=(1<<len)-1;
        // for(int i=1;i<(1<<len);i++)     //二进制枚举所有组合
        // {
        //     Beta=alpha;
        //     for(int j=0;j<len;j++) if((1<<j)&i) Beta.push_back(tree.Header_Table[j].item);
        //     res.push_back(Beta);
        //     ans[Beta.size()]++;
        // }
        return ;
    }
    for(auto v=tree.Header_Table.rbegin();v!=tree.Header_Table.rend();v++){
        // alpha.push_back(v->item);
        // res.push_back(alpha);
        // ans[alpha.size()]++;
        tns++;
        //生成条件模式基
        fpnode*p=v->head,*q;
        db.clear();
        while (p!=nullptr)
        {
            q=p->fa;
            tmp.clear();
            while (q->fa!=nullptr)
            {
                tmp.push_back(q->item);
                q=q->fa;
            }
            if(tmp.size()){
                tmp.push_back(p->count);    //在最后添加出现次数
                db.push_back(tmp);
            }
            p=p->next;
        }
        //db生成条件模式树
        if(db.empty()) {
            alpha.pop_back();
            continue;
        }
        FP_Tree newtree=build();
        FPgrowth(newtree);
        // alpha.pop_back();
    }
}
int main(){
    sur=0.0005;
    path="./C++/retail.dat";
    //读数据、建树
    FP_Tree ft=first_construct();
    //调用FP-Gowth算法进行挖掘
    FPgrowth(ft);
    cout<<tns;
    // cout<<"total: "<<res.size()<<" items\n";
    // for(auto v:ans){
    //     cout<<v.first<<' '<<v.second<<endl;
    // }
    //写入文件
    // ofstream outf("./C++/fpgrowth.txt");
    // outf<<"total: "<<res.size()<<" items\n";
    // for(auto v:ans)  outf<<v.first<<"-frequent item:"<<v.second<<'\n';
    // for(auto tp:res){
    //     outf<<"{ ";
    //     for(int i:tp) outf<<i<<' ';
    //     outf<<"}\n";
    // }
    // outf.close();
    return 0;
}
