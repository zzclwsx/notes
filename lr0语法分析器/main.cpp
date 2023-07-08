#include <iostream>
#include <vector>
#include <iomanip>
#include <cstring>
#include <sstream>
 
using namespace std;

 
class Grammar{
public:
    //产生式的个数
    int grammarNum ;
    //定义产生式数组
    string chanshengshi[100] = {"E->S","S->BB","B->aB","B->b"};
 
    Grammar(){
        //产生式个数
        grammarNum = 4;
    }
};
//定义LR文法的分析表
class LRAnalyseTable{
public:
    char Tchar[10]={'a','b','#'};
    //定义终结符的个数
    int TNum =3;
    char Vchar[10]={'E','S','B'};
    //定义非终结符的个数
    int VNum = 3;
    //定义状态数
    int statusNum = 10;
    //action表
    string action[10][3]={
    {"s3","s4",""},
    {"","","acc"},
    {"s6","s7",""},
    {"s3","s4",""},
    {"r3","r3",""},
    {"","","r1"},
    {"s6","s7",""},
    {"","","r3"},
    {"r2","r2",""},
    {"","","r2"}
    };
    //goto表
    int goTo[10][3] = {
    {-1,1,2},
    {-1,-1,-1},
    {-1,-1,5},
    {-1,-1,8},
    {-1,-1,-1},
    {-1,-1,-1},
    {-1,-1,9},
    {-1,-1,-1},
    {-1,-1,-1},
    {-1,-1,-1}
    };

   //获取终结符的索引
    int getTcharIndex(char c){
        for(int i=0;i<TNum;i++){
            if(Tchar[i] == c){
                return i;
            }
        }
        return -1;
    }
    //获取非终结符的索引
    int getVcharIndex(char c){
        for(int i=0;i<VNum;i++){
            if(Vchar[i] == c){
                return i;
            }
        }
        return -1;
    }
};
 //状态栈
vector<int> status;

//符号栈
vector<char> sign;

//输入的字符串
vector<char> inputStr;

//记录输入的字符串
string inputS;

//定义文法
Grammar grammar;

//定义LR分析表
LRAnalyseTable analyseTable;

//读取输入的字符串
void readStr();

//对栈容器进行输出,i=0,返回status中的字符串,i=1,返回sign中的字符串，i=2返回inputStr
string vectTrancStr(int i);

void parseLR();
 
int main()
{
    readStr();
    parseLR();
    system("pause");
    return 0;
}
//读取输入的字符串
void readStr(){
    char c;
    cout<<"LR(1) grammar analyse:"<<endl;
    cout<<"Enter a string: ";
    cin>>c;
    while( c != '#'){
        inputS += c;
        inputStr.push_back(c);
        cin>>c;
    }
   inputStr.push_back('#');
   inputS += '#';
}

//对栈容器进行输出,i=0,返回status中的字符串,i=1,返回sign中的字符串，i=2返回inputStr中的字符串
string vectTrancStr(int i){
    char buf[100];
    int count = 0;
    //输出状态栈
    if(i == 0){
        vector<int>::iterator it =status.begin();
        //将数字转化为字符串
        string str,tempStr;
        for(it;it!= status.end();it++){
            stringstream ss;
            ss << *it;
            ss >> tempStr;
            str+=tempStr;
        }
        return str;
    }
    //输出符号栈
    else if(i == 1){
        vector<char>::iterator it = sign.begin();
        for(it ; it != sign.end() ;it++){
            buf[count] = *it;
            count++;
        }
    }
    //输出待分析的字符串
    else{
        vector<char>::iterator it = inputStr.begin();
        for(it ; it != inputStr.end();it++){
            buf[count] = *it;
            count++;
        }
    }
    buf[count] = '\0';
    string str(buf);
    return str;
}
//函数入口，开始分析lr1文法
void parseLR(){
    //步骤
    int step = 1;

    //把状态0入栈
    status.push_back(0);

    //把#加入符号栈
    sign.push_back('#');

    //输出初始栈状态
    cout<<setw(10)<<"step"<<setw(10)<<"status"<<setw(10)<<"c_stack"<<setw(10)<<"input"<<setw(10)<<"action"<<endl;
    
    //初始状态为0
    int s =0;

    //上一个状态
    int oldStatus = 999;

    //获取初始符号
    char ch = inputStr.front();

    //如果action[s][ch] =="acc" ，则分析成功
    while(analyseTable.action[s][analyseTable.getTcharIndex(ch)] != "acc"){
        //获取字符串
        string str = analyseTable.action[s][analyseTable.getTcharIndex(ch)];

        //如果str为空，表示分析表中没有定义这一项，是语法错误
        if(str.size() == 0){
            cout<<"error!"<<endl;
            cout<<inputS<<" is unvalid string"<<endl;
            return ;
        }

        //获取r或s后面的数字
        stringstream ss;
        ss << str.substr(1);
        ss >> s;

        //如果是移进
        if(str.substr(0,1) == "s"){
            cout<<setw(10)<<step<<setw(10)<<vectTrancStr(0)<<setw(10)<<vectTrancStr(1)<<setw(10)<<vectTrancStr(2)<<setw(10)<<"ACTION["<<status.back()<<","<<ch<<"]=S"<<s<<","<<"state "<<s<<" ruzhan"<<endl;
            //输入符号入栈
            sign.push_back(ch);

            //输入串里弹出
            inputStr.erase(inputStr.begin());

            //将状态数字入栈
            status.push_back(s);
        }
        //如果是归约
        else if(str.substr(0,1) == "r"){
            //用第s个产生式归约
            string formu = grammar.chanshengshi[s];

            //产生式长度
            int strSize = formu.size();

            //将产生式转化为字符数组
            char buf[100];
            strcpy(buf,formu.c_str());

            //获取产生式的首字符
            char nonTerCh = buf[0];

            //获取符号栈的出栈次数
            int popCount = strSize - 3;//-3是减去产生式左部和->符号，减去之后就是产生式右部的长度，则符号栈要出栈相应长度
            //从后往前查看状态
            vector<int>::reverse_iterator rit = status.rbegin();
            int i= 0;
            for(rit;rit != status.rend();rit++){
                i++;
                //cout<<"i = "<<i<<"   popCount = "<<popCount<<endl;
                if(i == popCount+1){
                    oldStatus = * rit;
                    break;
                }
            }
            int r = s;
            //修改s，此时s就是新状态
            s = analyseTable.goTo[oldStatus][analyseTable.getVcharIndex(nonTerCh)];
            cout<<setw(10)<<step<<setw(10)<<vectTrancStr(0)<<setw(10)<<vectTrancStr(1)<<setw(10)<<vectTrancStr(2)<<setw(10)<<"r"<<r<<(string)":"+grammar.chanshengshi[r]+(string)" guiyue,GOTO{"<<oldStatus<<","<<nonTerCh<<")="<<s<<" ruzhan"<<endl;
            
            //对符号栈进行出栈和状态栈进行出栈
            for(int i=0 ;i< popCount;i++){
                sign.pop_back();
                status.pop_back();
            }

            //再对产生式的开始符号入栈
            sign.push_back(nonTerCh);

            //再把新的状态入栈
            status.push_back(s);
        }
        //步骤数加1
        step++;
 
        //获取栈顶状态
        s = status.back();
        //获取输入的字符
        ch = inputStr.front();
    }
    cout<<setw(10)<<step<<setw(10)<<vectTrancStr(0)<<setw(10)<<vectTrancStr(1)<<setw(10)<<vectTrancStr(2)<<setw(10)<<"acc:success"<<endl;
    cout<<inputS<<" is vaild string"<<endl;
    
}