#include<iostream>
#include<stdlib.h>
#include <algorithm>
#include <iomanip>
#include <string.h>
using namespace std;

struct HEAD
{
	char type[4];//�ļ�����
	int length;//ԭ�ļ�����
	int weight[256];//Ȩֵ��ֵ
};

int InitHead(const char* pFilename, HEAD& sHead)
{
	//��ʼ���ļ�ͷ
	strcpy(sHead.type, "HUF");//�ļ�����
	sHead.length = 0;//ԭ�ļ�����
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
	//�����ļ���
	char filename[256] = { 0 };
	strcpy(filename, pFilename);
	strcat(filename, ".huf");
	//�Զ���������ʽ���ļ�
	FILE* out = fopen(filename, "wb");
	//д�ļ�ͷ
	fwrite(&sHead, sizeof(char), 1, out);
	//дѹ����ı���
	fwrite(pBuffer, sizeof(char), nSize, out);
	//�ر��ļ����ͷ��ļ�ָ��
	fclose(out);
	out = NULL;
	cout << "����ѹ���ļ���" << filename << endl;
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
			//	cout<<"�������ַ���"<<endl;
				node[j].x=j-1;
			//	cout<<"������Ȩֵ��"<<endl; 
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
		
		void HFTtest(int root){//������� 
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
		
void HFTcodetest(int root){//�������    ���� 
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
	cout<<"������������ѹ��������������"<<endl;
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
	//ѹ�� 
	char *pBuffer=NULL;
	pBuffer=(char*)malloc(nSize*sizeof(char));
	memset(pBuffer,0,nSize*sizeof(char));
	Encode(pFilename,pBuffer,nSize);
	if(!pBuffer){
		cout<<"����ʧ�ܣ�"<<endl;
		return -1; 
	}
	cout<<"ѹ���ɹ���"<<endl;
	
	HEAD sHead;
	InitHead(pFilename, sHead);
	cout << "ԭ�ļ�" << pFilename << "��СΪ��" << sHead.length << "Byte" << endl;
	int len_after = WriteFile(pFilename, sHead, pBuffer, nSize);
	cout << "��СΪ��" << len_after << "Byte \nͷ�ļ�sHead��СΪ��" << sizeof(sHead) << "Byte" << endl;
	cout << "ѹ�����ʣ�" << (double)len_after * 100 / sHead.length << "%" << endl;

	free(pBuffer);


}

int Encode(const char*pFilename,char *pBuffer,const int nSize){
	FILE* in=fopen(pFilename,"rb");
	if(in==NULL)
	{
		cout<<"���ļ�ʧ�ܣ�"<<endl; 
		exit(0);
	}
	pBuffer=(char*)malloc(nSize*sizeof(char));
	if(!pBuffer){
		cout<<"����ʧ�ܣ�"<<endl;
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
			//�����ֽ� 
			pBuffer[pos++]=Str2byte(cd);
			//�ַ������ư�λ 
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
	cout<<"�����ļ�����"<<endl;
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
