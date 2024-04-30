#include<iostream>
#include<stdlib.h>
#include <algorithm>
#include <iomanip>
#include <string.h>
using namespace std;

struct HEAD
{
	char type[4];//文件类型
	int length;//原文件长度
	int weight[256];//权值数值
};

int InitHead(const char* pFilename, HEAD& sHead)
{
	//初始化文件头
	strcpy(sHead.type, "HUF");//文件类型
	sHead.length = 0;//原文件长度
	for (int i = 0; i < 256; i++)
	{
		sHead.weight[i] = 0;
	}
	FILE* in = fopen(pFilename, "rb");
	int ch;
	while ((ch = fgetc(in)) != EOF)
	{
		sHead.weight[ch]++;
		sHead.length++;
	}
	fclose(in);
	in = NULL;
	return 1;
}

int WriteFile(const char* pFilename, const HEAD sHead, const char* pBuffer, const int nSize)
{
	//生成文件名
	char filename[256] = { 0 };
	strcpy(filename, pFilename);
	strcat(filename, ".huf");
	//以二进制流形式打开文件
	FILE* out = fopen(filename, "wb");
	//写文件头
	fwrite(&sHead, sizeof(char), 1, out);
	//写压缩后的编码
	fwrite(pBuffer, sizeof(char), nSize, out);
	//关闭文件，释放文件指针
	fclose(out);
	out = NULL;
	cout << "生成压缩文件：" << filename << endl;
	int len = sizeof(HEAD) + strlen(pFilename) + 1 + nSize;
	return len;
}


char Str2byte(const char*pBinStr)
{
	char b=0x00;
	for(int i=0;i<8;i++)
	{
		b=b<<1;
		if(pBinStr[i]=='1'){
			b=b|0x01;
		}
	 }
	 return b;
}

class Node{
	public:
		int x; 
		int weight;
	    int parent,rchild,lchild;
	    char *HC;
		Node(){
			parent=rchild=lchild=0;
			HC=NULL;
		}
		friend class HFT; 
};
class HFT{
	Node *node;
	int n;
	
	public:
		HFT(){
			node=NULL;
			n=256;
		}
		bool initHFT(int *Weight){
			int m=2*n-1;
			node=new Node[m+1];
			for(int i=1;i<=m;i++){
				node[i].parent=node[i].rchild=node[i].lchild=0;
			}
			for(int j=1;j<=n;j++){
			//	cout<<"请输入字符："<<endl;
				node[j].x=j-1;
			//	cout<<"请输入权值："<<endl; 
				node[j].weight=Weight[j-1];
				
			}
			cout<<endl;
			for(int k=n+1;k<=m;k++){
				int s1,s2;
				Select(s1,s2,k);
				node[s1].parent=k;
				node[s2].parent=k;
				node[k].weight=node[s1].weight+node[s2].weight;
				node[k].lchild=s1;
				node[k].rchild=s2;
			}
		} 
		bool Select(int &s1,int &s2,int n){
			for(int i=1;i<n;i++){
				if(node[i].parent==0){
					s1=i;
					break;
				}
			}
			for(int i=1;i<n;i++){
				if(node[i].parent==0&&node[i].weight<node[s1].weight)
				s1=i;
			}
			for(int j=1;j<n;j++){
				if(node[j].parent==0&&j!=s1){
					s2=j;
					break;
				}
			}
			for(int j=1;j<n;j++){
				if(node[j].parent==0&&node[j].weight<node[s2].weight&&j!=s1)
				s2=j;
			}
		
		}
		void Show(){
			cout << "index  Byte  weight  parent  LTree  RTree" << endl;
    		cout << left;     
    		int m = 2*n - 1;
    		for(int i = 1; i <= m; i++)
			{
      	  	cout << setw(5) << i << "  ";
			cout << setw(6) << "HFT["<<i<<"]"<< "  "  ;
        	cout << setw(6) << node[i].weight << "  ";     
        	cout << setw(6) << node[i].parent << "  ";
        	cout << setw(6) << node[i].lchild << "  ";
        	cout << setw(6) << node[i].rchild << "  " << endl;
    		}
 
		}
		
		void HFTtest(int root){//先序测试 
			cout<<node[root].weight<<" ";
			if(node[root].lchild!=0)
			HFTtest(node[root].lchild);
			if(node[root].rchild!=0)
			HFTtest(node[root].rchild);
		}
		
		void HFTcode(){
			int start=0,c=0,parent=0;
			for(int i=1;i<=n;i++){
				char *w=new char[n];
			    w[n-1]='\0';
				start=n-1;
				c=i;
				parent=node[i].parent;
				while(parent!=0){
//				if(parent==511){
//						start--;
//						w[start]='1';
//							} 
					if(node[parent].lchild==c)
					{
						start--;
						w[start]='0';
					}
				
					else 
					{
						start--;
						w[start]='1';
					}
					c=parent;
					parent=node[parent].parent;
				}
				 node[i].HC=new char[n-start];
				 strcpy(node[i].HC,&w[start]);
//				 cout<<node[i].HC<<endl;
				 delete w;
			}
			
			
		} 
		
void HFTcodetest(int root){//编码测试    先序 
	if(node[root].lchild==0&&node[root].rchild==0)
	{
		cout<< "HFT["<<root<<"]"<<"  "<<node[root].HC<<endl;
	} 
		if(node[root].lchild!=0)
		HFTcodetest(node[root].lchild);
		if(node[root].rchild!=0)
		HFTcodetest(node[root].rchild);
	}
	
int Compress(const char*pFilename){
	cout<<"——————压缩——————"<<endl;
	int weight[256]={0}; 
	FILE *in=fopen(pFilename,"rb");
	int ch;
	while((ch=getc(in))!=EOF)
	{
		weight[ch]++;
	}
	fclose(in);
	int nSize=0;
	for(int i=0;i<256;i++){
		nSize+=weight[i]*strlen(node[i+1].HC);
	}
	nSize=(nSize%8)?nSize/8+1:nSize/8;
	//压缩 
	char *pBuffer=NULL;
	pBuffer=(char*)malloc(nSize*sizeof(char));
	memset(pBuffer,0,nSize*sizeof(char));
	Encode(pFilename,pBuffer,nSize);
	if(!pBuffer){
		cout<<"开辟失败！"<<endl;
		return -1; 
	}
	cout<<"压缩成功！"<<endl;
	
	HEAD sHead;
	InitHead(pFilename, sHead);
	cout << "原文件" << pFilename << "大小为：" << sHead.length << "Byte" << endl;
	int len_after = WriteFile(pFilename, sHead, pBuffer, nSize);
	cout << "大小为：" << len_after << "Byte \n头文件sHead大小为：" << sizeof(sHead) << "Byte" << endl;
	cout << "压缩比率：" << (double)len_after * 100 / sHead.length << "%" << endl;

	free(pBuffer);


}

int Encode(const char*pFilename,char *pBuffer,const int nSize){
	FILE* in=fopen(pFilename,"rb");
	if(in==NULL)
	{
		cout<<"打开文件失败！"<<endl; 
		exit(0);
	}
	pBuffer=(char*)malloc(nSize*sizeof(char));
	if(!pBuffer){
		cout<<"开辟失败！"<<endl;
		return -1; 
	}
	
	cout<<"loading"<<endl;
	
	char cd[256]={0};
	int pos =0;
	int ch;
	while((ch=fgetc(in))!=EOF){
		strcat(cd,node[ch+1].HC);
	//	printf("%s",node[ch+1].HC);
		while(strlen(cd)>=8){
			//编码字节 
			pBuffer[pos++]=Str2byte(cd);
			//字符串左移八位 
			for(int i=0;i<(256-8);i++){
				cd[i]=cd[i+8]; 
			}
		}
	}
	if(strlen(cd)>0)
	{
		pBuffer[pos++]=Str2byte(cd);
	 } 
	 fclose(in);
	 return 1;//OK
} 

};


int main(){
	cout<<"输入文件名："<<endl;
	char filename[256];
	cin>>filename;
	int weight[256]={0}; 
	FILE *in=fopen(filename,"rb");
	int ch;
	while((ch=getc(in))!=EOF)
	{
		weight[ch]++;
	}
	fclose(in);
	cout<<"Byte"<<"  "<<"Weight"<<endl;
	for(int i=0;i<256;i++)
	{
		printf("0x%02X  %d\n",i,weight[i]);
	}
	HFT hft;
	hft.initHFT(weight);
	hft.Show();
//	hft.HFTtest(511);
	hft.HFTcode();
  //cout<<"Byte"<<"       "<<"Code"<<endl;
	hft.HFTcodetest(511);
	hft.Compress(filename);
	return 0;
}
