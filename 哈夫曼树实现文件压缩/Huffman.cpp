#include <iostream>
#include <fstream>
#include <bitset>
#include <queue> 
#include <string>
using namespace std;
	struct store{//暂存数据用的一个结构
		unsigned char data;
		int weight;
		string code;
	};
	
	struct htnode{	//非叶子结点只记录权重，叶子结点需记录权重和数据 
		unsigned char data;
		int weight;
		string code;
		htnode *lchild,*rchild;
	};
		
	struct cmp{	//重写仿函数 
		bool operator()(htnode* a,htnode* b){
		    return a -> weight > b -> weight;	//比较两个结点的权值大小，为构造优先队列做准备
		}
	};
 
class huffman_tree{
	private:
		htnode *root;
	public:
		huffman_tree();
		~huffman_tree();
		void creat(struct store*,int);
		void get_hcode(store*);
		void hcode(htnode*,string,store* s);
		void destory(htnode*);
		void visit(htnode*);
		htnode* get_root();
};
huffman_tree::huffman_tree(){}
 
huffman_tree::~huffman_tree(){
	destory(root);
}
void huffman_tree::destory(htnode *temp){//用于析构函数
	if(temp != NULL){
		destory(temp -> lchild);
		destory(temp -> rchild);
		delete temp;
	}
}
 
void huffman_tree::creat(struct store* s,int n){ 	//创建哈夫曼树，n代表store数组的数量
	int w1,w2;	//子节点的权重数 
	priority_queue<htnode*,vector<htnode*>,cmp> q;//STL优先队列，三个参数分别为：储存元素的类型，储存的底层容器，比较优先级的函数
	htnode *temp;
	for(int i=0;i<n;i++){	//存储数据到优先级队列   同时将store转为哈夫曼结点
		temp = new htnode;
		temp -> data = s[i].data;
		temp -> weight = s[i].weight;
		temp -> lchild = NULL;
		temp -> rchild = NULL;
		q.push(temp);
	}
    //由于每个不同的字符都用了一个store来存储，在这里队列q中已经按权值从小到大的优先顺序排列好了所有待建树的结点。
	while(1){
		temp = new htnode; 	//创建和结点，合并两棵树 
		temp -> lchild = q.top();	//权值小为左树 
		w1 = q.top() -> weight;     //存下左树的权值
		q.pop();
		temp -> rchild = q.top();	//权值次小为右树 
		w2 = q.top() -> weight;     //存下右树的权值
		q.pop();
		temp -> weight = w1 + w2;	//新产生的结点权值为其子树权值和 
		q.push(temp);	//新结点入队 这个新节点就是最小和次小两个结点的父节点，入队等待下一轮建树。有一说一，这优先队列是真好用
		
        //结束条件判断，temp已有左右孩子，并且队列只剩1个元素也就是temp，就证明已经建好哈夫曼树，break
        if(temp->rchild->lchild!=NULL)
		    if(q.size()==1) {
			    root = temp;
			    break;
		    }
	} 
}
 
void huffman_tree::get_hcode(store* s){
	hcode(root,"",s);//当前遍历结点，当前结点编码，待编码数据数组
}
 
void huffman_tree::hcode(htnode* temp,string cd,store* s){
	if(temp!=NULL){
		if(temp->lchild==NULL&&temp->rchild==NULL){	//当前结点为叶子结点，得到哈夫曼编码 
			temp -> code = cd;
			for(int i=0;i<256;i++){
				if(temp->data==s[i].data&&cd!=s[i].code){
					s[i].code = cd;	
				}
			}
		}
		else{
			hcode(temp->lchild,cd+"0",s);	//左子树编码为0 
			hcode(temp->rchild,cd+"1",s);	//右子树编码为1 
		}
	}
}
 
void huffman_tree::visit(htnode* temp){
	cout<<temp -> data;
}
 
htnode* huffman_tree::get_root(){
	return root;
}
void compress(/*string txtPath*/){
	store s[256];//文件中每个不同的字符都创建一个store的临时结构，后续会在创建哈夫曼树的时候转变为哈夫曼结点，256应该绰绰有余
	int num=0; //记录文件中读取的不同的字符个数 
	int count = 0;
    //初始化每个s的权值
	for(int i=0;i<256;i++)
		s[i].weight = 0;

	ifstream ifs,ifs1;
	ifs.open(/*txtPath*/"D:\\fruits-after\\cherry.txt",ios::in|ios::binary);//以输入或二进制方式打开文件
	if(!ifs.is_open()){
		cout<<"文件打开失败"<<endl;
	}
	else{
		unsigned char c;
		while(ifs.peek()!=EOF){	//按字节存储文件数据以及权重 
			ifs.read((char*)&c,1);
			count ++;	//共有count个字符，考虑到读取方面的困难，暂时用字母频率大致代替词条频率，
                        //用string统计出词条频率并且构建哈夫曼树压缩应该能得到更佳的压缩效果。
            
            //这个for循环效率很低，每次读取到一个字符就遍历一次store数组，如果是以前出现过的会在让这个字符权值+1
            //如果遍历完了已统计的字符（也就是遇到了权值为0的s[i]）就说明读取到了新的字符，存入并且设置权值和data等
            //字符种类num++
            //好在字符种类不多，这个for循环还不算太臃肿。
			for(int i=0;i<256;i++){
				if(s[i].weight == 0){
					s[i].data = c;
					s[i].weight ++;
					num++;
					break;
				}
				else if(s[i].data == c){
					s[i].weight ++;
					break;
				} 
			}
		}
	}
	cout<<"count:"<<count<<endl;
	//变量定义 
	string *str = new string[count];	//存储原文件对应的哈夫曼码 
	string str1 = "";	//临时存储哈夫曼码按8位分割后的字符串 
	string buf = "";	//缓冲区
	int uncompress_filesize = count;		//未压缩文件大小 
	int compress_filesize = 0;				//压缩文件大小 
	//构造哈夫曼树 
	huffman_tree tree;
	tree.creat(s,num);//建树
	tree.get_hcode(s);//编码
	//再读文件 
	ifs.clear();
	ifs.seekg(0,ios::beg);	//文件指针回调到文件开头
	unsigned  char c;
	int z = 0;
	while(ifs.peek()!=EOF){
		ifs.read((char*)&c,1);
		for(int j=0;j<num;j++){
			if(c==s[j].data){
				str[z] = s[j].code;//将原文对应的哈夫曼编码存入，每个字符的编码用一个string存
				compress_filesize += str[z].size();
				break;
			}
		}
		z ++;
	}
	//将原文件的哈夫曼码按8位分割形成字符 ,str传入str1 
	//将压缩码写入压缩文件
	int k=0,m=0,csize=(compress_filesize-1)/8+1; 		//k代表第几个字符串，m代表该字符串的第几个字符，csize代表压缩后的数据规模 
	unsigned char* new_ch = new unsigned char[csize]; 	//存储压缩文件的信息
	for(int i=0;i<csize;i++){
		new_ch[i] = 0;
		//哈夫曼码分割 
		int len = str[k].length();
		for(;m<len;m++){
			str1 += str[k][m]; 
			if(str1.length()==8) {
				if(m==(str[k].length()-1)){		//当前字符剩余的对应组成的哈夫曼码不足8位 ，需要读取下一个哈夫曼码   在这点
                                                //就能看出来压缩了空间，原本占一个字节的字符用编码表示就占不到一个字节，这个
                                                //字节剩余的空间可以放下一个字符的哈夫曼编码（的一部分），虽然文本显示是乱码，
                                                //但是存储信息后面哈夫曼解码的时候可用，而且由于解码的时候碰到叶子结点就能知
                                                //道解出一个字符，之前转换为字符打乱的编码间隔也不是障碍。
					if(k!=count-1){				//当前字符不是最后一个字符 
						k ++;
						m = -1;
					}
				}
				m ++;		//后面要跳出循环，此处需要手动++ 
				break;
			}
			if(m==(len-1)){		//当前字符剩余的对应组成的哈夫曼码不足8位 ，需要读取下一个哈夫曼码 
				if(k != (count-1)){				//当前字符不是最后一个字符 
					k ++;
					len = str[k].length();
					m = -1;					//注意要赋值-1，因为每次循环结束还要m+1，这样才能让m=0 
				}
				else{						//最后一个字符，需要凑齐8位 因为这个方法后面补0，所以要存下来压缩之前的
					int len1 = str1.length();//文件规模，解压缩的时候以这个大小为参照，以免后面补的0也被翻译出来。
					for(int i=0;i<8-len1;i++) str1 += "0";
					break;
				}
			}
		}
		//哈夫曼码转换为字符 
        //这里的哈夫曼码已经被切开了，转换为字符之后显示应该是乱码。
		for(int j=0;j<8;j++){
			new_ch[i] = 2*new_ch[i] + (str1[j]-'0');
		} 
		str1.clear();
	}
	ofstream ofs,ofs1; 
	ofs.open("D:\\yasuo.txt",ios::out|ios::binary);
	cout<<"写入压缩文件的字符"<<endl;
	for(int i=0;i<csize;i++){
		ofs.write((char*)&new_ch[i],1);//之前储存了压缩后的规模，这里判断循环结束条件方便一点。
	} 
	ofs.close();			//关闭压缩文件的输出流 



    //解压部分
	ifs1.open("D:\\yasuo.txt",ios::in|ios::binary);		//读压缩文件，准备解压 
	if(!ifs1.is_open()){
		cout<<"文件打开失败"<<endl;
	}
	else{	//打开压缩文件 
		htnode* rt,left,right;
		buf.clear();
		int count1 = 0;	//用于记录解压个数与count对应   最后count == count1就是解压完毕，后续再有字符不必理会。
		string result = "";	//记录压缩码中的哈夫曼码 
		rt = tree.get_root();	//拿到根节点用于遍历哈夫曼树 
		ofs1.open("D:\\jieya.txt",ios::out|ios::binary);	//打开解压文件	
		cout<<"解压后"<<endl;

        //难点在于将01编码转换为字符串和将字符串转换为01编码，解决了这个之后，解压缩就是哈夫曼树解码的过程，注意使用
        //相同编码的哈夫曼树。
		while(ifs1.peek()!=EOF){
			ifs1.read((char*)&c,1);
			buf = bitset<8> (c).to_string();	//得到8位01字符串 	
			for(int j=0;j<8;j++){
				if(count1==count){
					break;
				}
				result += buf[j];
				if(buf[j]=='0'){	//读哈夫曼树的左树
					rt = rt -> lchild;
				}
				else{	//读右树 
					rt = rt -> rchild;
				}
				if(result == rt -> code){		//找到了对应的哈夫曼码 
					ofs1.write((char*)&rt -> data,1);
					rt = tree.get_root();
					//buf.clear();
					result.clear(); 
					count1 ++;
				}
			}	
		}
	}
	ifs.close();
	ifs1.close();
	ofs1.close();
}
 
int main() {
    /*cout<<"please enter the target path:"<<endl;
    string path;
    cin>>path;*/
	compress(/*path*/);
	system("pause");
	return 0;
}