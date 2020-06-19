#include<stdio.h>
#include<string>
#include<vector>
#include<stack>
#include<queue>
#include"Timer.h"
using namespace std;

#define MAX_VERTEX_NUM  200
typedef char TransKind ;
typedef char InfoType ;



typedef struct
{
	bool pass = false;
	char country[64] = {0};
	char city[64] = {0};
	double latitude;//纬度
	double longitude;//经度
}City,VertexType;   //城市，即点的结构体

//边结点
typedef struct ArcNode
{
	int adjvex;                     //邻接的顶点
	struct ArcNode* nextarc;       //下一个邻接边指针
	TransKind kind[6] = { 0 };
	double time;
	double money;
	InfoType info[1024] = { 0 };
}ArcNode;

//邻接表中的各个开始顶点
typedef struct VNode {  
	VertexType data;
	ArcNode* firstArc=nullptr;

}VNode,AdjList[MAX_VERTEX_NUM];

//图的结构
typedef struct {
	AdjList vertices;   //邻接表
	int vexnum=0;         //点数
	int arcnum=0;        //边数
}Graph;

bool CreateCities(const char* pathname, vector<City>& city)
{
	//该函数用于制作城市表，从city.csv中读取城市信息  
	FILE *fp = nullptr;
	fopen_s(&fp,pathname, "rt");
	if (!fp) 
	{   
		printf("Cannot open the city file: %s!",pathname);   
		return false;  
		exit(0);
	}  
	City tmpcity;//作为城市临时存储变量  
	int i = 0, j = 0;  
	char* record = NULL;  
	char* next_token =nullptr;   //存储分割后剩余的
	char buf[1024];//每一行的信息缓冲区  
	char* line = NULL;  
	/*int count = 0;*/
	while ((line = fgets(buf, 1024, fp)) != NULL) 
	{   
		
		record = strtok_s(buf, ",",&next_token);   //以","作为分隔符读取信息   
		while (record != NULL) 
		{    
			switch (j) 
			{    
			case 0://第一个逗号前面为country     
				strcpy_s(tmpcity.country, record);     
				j++; 
				break; 
			case 1://第二个逗号前面为city     
				strcpy_s(tmpcity.city, record);     
				j++; 
			    break;    
			case 2://第三个逗号前面为纬度     
				tmpcity.latitude = atof(record); 
				j++; 
				break;    
			case 3://第四个逗号前为经度     
				tmpcity.longitude = atof(record);
				j = 0; 
				break;    
			}    
			record = strtok_s(NULL, "," ,&next_token); //第二次调用，从前一个分割的地方读取, 如果没找到分割符返回null
		}   
		city.push_back(tmpcity);   //将第i个城市存入city中  
	      //再循环进行下一行的城市读取
		/*count++;
		if (count == 99)break;*/
	} 

	fclose(fp);
	printf("读取city成功\n");
	return true;
} 

int SearchCity(const char* des, vector<City>&city) 
{  //该函数返回某城市des在城市表city中的下标序号  
	for (int pos = 0; pos < int(city.size()); pos++) 
	{   
		if (!strcmp(city[pos].city, des))
			return pos;// 找到则返回下标 
	}  
	return -1;     //若未找到则返回-1 
} 

//此函数建立网图时将重复的信息也读取了并存储了
bool CreateGraph(const char* pathname,Graph& G, vector<City>city)
{  //该函数用于建立城市网图G，城市信息来自city，边的信息来自route.csv文件  

	//初始化
	G.vexnum = city.size();//顶点数为city个数  
	G.arcnum = 0;//边数初始为0  
	for (int i = 0; i < G.vexnum; i++) 
	{
		//初始化定点向量  
		G.vertices[i].data = city[i];
		G.vertices[i].firstArc = nullptr;
	}  

	FILE* fp = nullptr;
	fopen_s(&fp, pathname, "rt");
	if (!fp) 
	{   
		printf("Cannot open the route file: %s",pathname);
		return false;
	}  

	char* record = nullptr;
	char* next_token = nullptr;  //存储分割后剩余的
	char buf[1024];        //每一行的信息缓冲区 
	char* line = nullptr;

	while ((line = fgets(buf, 1024, fp)) != nullptr) 
	{   
		int j = 0;      //j用于循环计数
		int v1, v2;     //v1为起点，v2 为终点   
		ArcNode* tmparc = new ArcNode;    //tmparc临时储存边信息   
		tmparc->nextarc = nullptr;

		record = strtok_s(buf, ",",&next_token); 
		while (j <= 6) 
		{    
			if (j == 0) 
			{   //j == 0时找到起点序号     
				char temp[1024] = { 0 };      //temp用于存储两个逗号之间字符串信息
				strcpy_s(temp, record);
				v1 = SearchCity(temp, city);  //v1为起点下标     
				if (v1 == -1)break;
				record = strtok_s(NULL, ",",&next_token);
				j++;
			}
		   else if (j == 1) 
			{   //j == 1时找到终点序号     
				char temp[1024] = { 0 };
				strcpy_s(temp, record);
				v2 = SearchCity(temp, city);     //v2为终点下标
				if (v2 == -1)break;
				tmparc->adjvex = v2;           //该边的adjvex为v2

				record = strtok_s(NULL, ",", &next_token);
				j++;    
			}    
		   else if (j == 2) 
			{    //j == 2时读取交通方式     
				char temp[1024] = { 0 };
				strcpy_s(temp, record); 

				if (!strcmp(temp, "plane")) 
				{     
					strcpy_s(tmparc->kind, "plane");
				}     
				else if (!strcmp(temp, "bus")) 
				{      
					strcpy_s(tmparc->kind, "bus");  
				}     
				else if (!strcmp(temp, "train")) 
				{      
					strcpy_s(tmparc->kind, "train");
				}     

				record = strtok_s(NULL, ",", &next_token);
				j++;    
			}   
			else if (j == 3) 
			{   //j == 3时输入所需时间     
				char temp[1024] = { 0 };
				strcpy_s(temp, record);
				tmparc->time = atof(temp); 

				record = strtok_s(NULL, ",", &next_token);
				j++;
			}    
			else if (j == 4) 
			{   //j == 4时输入所需费用    
				char temp[1024] = { 0 }; 
				strcpy_s(temp, record); 
				tmparc->money = atof(temp);         //atoi将字符串转换为整形  atof为double

				record = strtok_s(NULL, "\n", &next_token); 
				//由于后续信息为边的附加信息，所以不以逗号为分隔符，而以换行符为分隔符，吞去\n  
				j++;
			}    
			 
			else if (j == 5) 
			{   //j == 5时输入附加信息
				char temp[1024] = { 0 };
				strcpy_s(temp, record);
				//因为info里有'，到时候输出到html中可视化时会有小问题，所以需要补入\转义
				char p[1024] = { 0 };
				int i = 0; int k = 0;
				if(strstr(temp, "\'"))
				{
					while (temp[k] != '\0')
					{
						while (temp[k] != '\''&& temp[k] != '\0')
						{
							p[i] = temp[k];
							i++, k++;
						}
						if (temp[k] == '\'')
						{
							p[i++] = '\\';
							p[i++] = '\'';
							k++;
						}
						else if (temp[k] == '\0')
						{
							p[i] = '\0';
						}
					}
				}
				strcpy_s(tmparc->info, p);
				j++;    
			}   
			else if (j == 6) 
			{   //j == 6时将该边连接到v1的邻接表上去
				if (G.vertices[v1].firstArc == nullptr)
				{
					G.vertices[v1].firstArc = tmparc;//若头节点为空，则连接到头节点 
					G.vertices[v1].firstArc->nextarc = nullptr;
				}
				else 
				{   //若头节点不空，则连接到尾结点上去
					ArcNode* p= G.vertices[v1].firstArc;
					while (p->nextarc!=nullptr) 
					{      
						p = p->nextarc; 
					} 
					p->nextarc = tmparc;
					p->nextarc->nextarc = nullptr;
					G.arcnum++;
				}    
				j++; 
			}  
		} 
		//一行信息读完
    } //while循环读取结束
	fclose(fp);
	printf("建立网图成功\n");
	return true; 
}

bool First = true;
void DFS(Graph &G, int v0, vector<City>& city)
{
	if (First)
	{
		printf("%s", city[v0].city);
		First = false;
	}
	else 
	{
		printf("->%s", city[v0].city);
	}
	city[v0].pass = true;
	int v;
	ArcNode* p = G.vertices[v0].firstArc;
	while (p != nullptr)
	{
		v = p->adjvex;
		if (city[v].pass == false)
		{
			DFS(G, v, city);
		}
		p = p->nextarc;
	}
}
void BFS(Graph& G, int v0, vector<City>& city)
{
	int w;
	ArcNode* p=nullptr;
	queue<int> qu;		//定义环形队列指针
	printf("%s",city[v0].city); 		//输出被访问顶点的编号
	city[v0].pass = true;              	//置已访问标记
	qu.push(v0);
	while (!qu.empty())       	//队不空循环
	{
		w=qu.front();			//出队一个顶点w
		qu.pop();
		p = G.vertices[w].firstArc; 	//指向w的第一个邻接点
		while (p != nullptr)		//查找w的所有邻接点
		{
			int v = p->adjvex;
			if (city[v].pass == false) 	//若当前邻接点未被访问
			{
				printf("->%s",city[v].city);  //访问该邻接点
				city[v].pass = true;	//置已访问标记
				qu.push(v);	//该顶点进队
			}
			p = p->nextarc;              	//找下一个邻接点
		}
	}
	printf("\n");
}

double GetWeight(Graph &G, int v0, int v, char kind) 
{  //该函数用于返回v0到v的边的权重，其中G为所用的图  
   //v0为起点，v为终点，kind为'T'时则返回所需时间， 为'M'时则返回所需费用  
   //若从v0到v有多于一种方式，则返回权值较小的值  
   //若不存在该边，则返回INFINITY  
	double min = INFINITY*1.0;//最小值初始化为无穷大  
	if (kind == 'T') 
	{//求所需时间   
		ArcNode *p = G.vertices[v0].firstArc;   
		while (p!=nullptr)   //遍历所有存在与V0相邻的边寻找终点为v的
		{    
			if (p->adjvex == v) 
			{//若终点为v则判断权值是否小于min     
				if (p->time < min)
					min = p->time;    
			}   
			p = p->nextarc;
		}   
		return min;  
	}  
	else if (kind == 'M') 
	{//求所需费用   
		ArcNode *p = G.vertices[v0].firstArc;
		while( p != nullptr) 
		{    
			if (p->adjvex == v) 
			{//若终点为v则判断权值是否小于min    
				if (p->money < min)
					min = p->money;    
			}   
			p = p->nextarc;
		}      
		return min;  
	} 
	else
	{
		printf("kind 输入有误\n");
		return 0;
	}
} 

double GetWeight2(Graph& G, int v0, int v, char Kind,char*kind)
{  //该函数用于返回v0到v的边的权重，其中G为所用的图  
   //v0为起点，v为终点，kind为'T'时则返回所需时间， 为'M'时则返回所需费用  
   //若从v0到v有多于一种方式，则返回权值较小的值  
   //若不存在该边，则返回INFINITY  
	double min = INFINITY * 1.0;//最小值初始化为无穷大  
	if (Kind == 'T')
	{//求所需时间   
		ArcNode* p = G.vertices[v0].firstArc;
		while (p!= nullptr)   //遍历所有存在与V0相邻的边寻找终点为v的
		{
			if (p->adjvex == v&& strcmp(p->kind,kind)==0)
			{//若终点为v则判断权值是否小于min     
				if (p->time < min)
					min = p->time;
			}
			p = p->nextarc;
		}
		return min;
	}
	else if (Kind == 'M')
	{//求所需费用   
		ArcNode* p = G.vertices[v0].firstArc;
		while (p!= nullptr)
		{
			if (p->adjvex == v&& strcmp(p->kind, kind) == 0)
			{//若终点为v则判断权值是否小于min    
				if (p->money < min)
					min = p->money;
			}
			p = p->nextarc;
		}
		return min;
	}
	else
	{
		printf("kind 输入有误\n");
		return 0;
	}
}

typedef double ShortPathTable[MAX_VERTEX_NUM];
int Path[MAX_VERTEX_NUM];      //Path[]中储存了每个点在其最短路径中上一个点的下标，若不存在上一个点，即为起点，则其值为-1 
void ShortestPath(Graph &G, int v0, ShortPathTable &D,char kind) 
{ 
	//该函数使用Dijkstra算法求解图G中以v0为起点到达其他点的最短路径，其值存在数组D中  
   //kind为所求权值的类型，'T'为时间,'M'为费用  
	if (kind != 'T' && kind != 'M')
	{
		printf("kind 输入有误\n");
		return;
	}
	int v, w, i;  
	double min;  
	int final[MAX_VERTEX_NUM];  //final[v]=1 表示v0到v的最短路径已求得 

	for (v = 0; v < G.vexnum; v++) 
	{//初始化final[]，D[]，Path[]   
		final[v] =0; 
		D[v] = GetWeight(G, v0, v, kind);
		if (D[v] != INFINITY)
		{
			Path[v] = v0;
		}
		else
		{
			Path[v] = -1;
		}
	}  
	D[v0] = 0;   //初始化D[v0] = 0,即到自己是最短路径
	final[v0] = 1;     //初始化final[v0] = 1 
   //开始主循环，每次求得v0到某个v顶点的最短路径， 并将final置为1  
	for (i = 1; i < G.vexnum; i++) 
	{//求其余剩下的 G.vexnum - 1个顶点   
		min = INFINITY*1.0;//当前所知离v0顶点的最近距离   
		for (w = 0; w < G.vexnum; w++) 
		{    
			if (!final[w] && D[w] < min) //寻找一开始就能直接到当前第i+1个顶点的最短路径（一开始是求直接连接到V0的最短值）
			{ 
				v = w;
				min = D[w];
			}
			//w顶点离v0顶点更近   
		}   
		//循环结束找到最近的v
		final[v] = 1;//离v0顶点最近的v的final[v]置为1
		for (w = 0; w < G.vexnum; w++) 
		{//更新当前最短路径及距离    
			if (final[w]==0 && (min + GetWeight(G, v, w, kind) < D[w]))  //如果通过经过当前的min的顶点V再到顶点w的值比一开始记录的到w的最短路径短就需要更新
			{     
				D[w] = min + GetWeight(G, v, w, kind);
				Path[w] = v;    //记录到达w路径的上一个顶点v的值    
			}   
		}  
	} 
}

void ShortestPath2(Graph& G, int v0, ShortPathTable& D, char Kind,char*kind)
{
	//该函数使用Dijkstra算法求解图G中以v0为起点到达其他点的最短路径，其值存在数组D中  
   //kind为所求权值的类型，'T'为时间,'M'为费用  
	if (Kind != 'T' && Kind != 'M')
	{
		printf("Kind 输入有误\n");
		return;
	}
	if (strcmp(kind,"bus")!=0 && strcmp(kind, "plane")!=0 && strcmp(kind, "train")!=0)
	{
		printf("交通方式 输入有误\n");
		return;
	}
	int v, w, i;
	double min;
	int final[MAX_VERTEX_NUM];  //final[v]=1 表示v0到v的最短路径已求得 

	for (v = 0; v < G.vexnum; v++)
	{//初始化final[]，D[]，Path[]   
		final[v] = 0;
		D[v] = GetWeight2(G, v0, v, Kind,kind);
		if (D[v] != INFINITY)
		{
			Path[v] = v0;
		}
		else
		{
			Path[v] = -1;
		}
	}
	D[v0] = 0;   //初始化D[v0] = 0,即到自己是最短路径
	final[v0] = 1;     //初始化final[v0] = 1 
   //开始主循环，每次求得v0到某个v顶点的最短路径， 并将final置为1  
	for (i = 1; i < G.vexnum; i++)
	{//求其余剩下的 G.vexnum - 1个顶点   
		min = INFINITY * 1.0;//当前所知离v0顶点的最近距离   
		for (w = 0; w < G.vexnum; w++)
		{
			if (!final[w] && D[w] < min) //寻找一开始就能直接到当前第i+1个顶点的最短路径（一开始是求直接连接到V0的最短值）
			{
				v = w;
				min = D[w];
			}
			//w顶点离v0顶点更近   
		}
		//循环结束找到最近的v
		final[v] = 1;//离v0顶点最近的v的final[v]置为1
		for (w = 0; w < G.vexnum; w++)
		{//更新当前最短路径及距离    
			if (final[w] == 0 && (min + GetWeight2(G, v, w, Kind,kind) < D[w]))  //如果通过经过当前的min的顶点V再到顶点w的值比一开始记录的到w的最短路径短就需要更新
			{
				D[w] = min + GetWeight2(G, v, w, Kind,kind);
				Path[w] = v;    //记录到达w路径的上一个顶点v的值    
			}
		}
	}
}


void GetArc(Graph& graph, int w1, int w2, ArcNode& arc, char kind)   //寻找w1到w2的边存到arc里
{
	double min = INFINITY * 1.0;//最小值初始化为无穷大  
	if (kind == 'T')
	{
		ArcNode* p = graph.vertices[w1].firstArc;
		while (p != nullptr)   //遍历所有存在与w1相邻的边寻找终点为w2的
		{
			if (p->adjvex == w2)
			{//若终点为w2则判断权值是否小于min     
				if (p->time < min)
				{
					min = p->time;
					arc = *p;
				}
			}
			p = p->nextarc;
		}
	}
	else if (kind == 'M')
	{  //求所需费用   
		ArcNode* p = graph.vertices[w1].firstArc;
		while (p != nullptr)
		{
			if (p->adjvex == w2)
			{//若终点为v则判断权值是否小于min    
				if (p->money < min)
				{
					min = p->money;
					arc = *p;
				}
			}
			p = p->nextarc;
		}
	}
}
void GetArc2(Graph& graph, int w1, int w2, ArcNode& arc, char Kind,char*kind)   //寻找w1到w2以kind交通方式的边存到arc里
{
	double min = INFINITY * 1.0;//最小值初始化为无穷大  
	if (Kind == 'T')
	{
		ArcNode* p = graph.vertices[w1].firstArc;
		while (p != nullptr)   //遍历所有存在与w1相邻的边寻找终点为w2的
		{
			if (p->adjvex == w2&&strcmp(p->kind,kind)==0)
			{//若终点为w2则判断权值是否小于min     
				if (p->time < min)
				{
					min = p->time;
					arc = *p;
				}
			}
			p = p->nextarc;
		}
	}
	else if (Kind == 'M')
	{  //求所需费用   
		ArcNode* p = graph.vertices[w1].firstArc;
		while (p != nullptr)
		{
			if (p->adjvex == w2 && strcmp(p->kind, kind) == 0)
			{//若终点为v则判断权值是否小于min    
				if (p->money < min)
				{
					min = p->money;
					arc = *p;
				}
			}
			p = p->nextarc;
		}
	}
}

bool createhtml(Graph&graph,char*start,char*des,int v,char kind)
{
	FILE* fp = nullptr;
	fopen_s(&fp, "tmp.html", "wt");
	if (!fp)
	{
		printf("Cannot open the file:tmp.html\n");
		return false;
	}
	fprintf(fp, "<!DOCTYPE html><html><head><style type='text/css'>body, html{width: 100%%;height: 100%%;margin:0;font-family:'微软雅黑 ';}#allmap{height:100%%;width:100%%;}#r-result{width:100%%;}</style><script type='text/javascript' src='http://api.map.baidu.com/api?v=2.0&ak=nSxiPohfziUaCuONe4ViUP2N'></script><title>Shortest path from %s to %s</title></head><body><div id='allmap'></div></div></body></html><script type='text/javascript'>var map = new BMap.Map('allmap');var point = new BMap.Point(0, 0);map.centerAndZoom(point, 2);map.enableScrollWheelZoom(true);", start, des);

	//根据格式输出  
	stack<int>q;//由于Path[]中每个顶点储存的为上一个顶点的下标，故新建一个栈，栈底为终点，栈顶为起点 
	int w = v;//终点
	while (Path[w] != -1)
	{
		q.push(w);
		w = Path[w];
	}
	q.push(w); //将起点压入栈中

	int j = 0;
	while (q.size() != 1) //即起点为-1
	{
		int w1 = q.top();//第j次边的开始顶点   //j=0是起点
		q.pop();
		int w2 = q.top(); //结束顶点
		ArcNode arc;
		GetArc(graph, w1, w2, arc, kind);
		fprintf(fp, "var marker%d = new BMap.Marker(new BMap.Point(%.4lf, %.4lf));map.addOverlay(marker%d);\n", j, graph.vertices[w1].data.longitude, graph.vertices[w1].data.latitude, j);
		fprintf(fp, "var infoWindow%d = new BMap.InfoWindow(\"<p style = 'fontsize:14px;'>country: %s<br/>city : %s</p>\");marker%d.addEventListener(\"click\", function(){this.openInfoWindow(infoWindow%d);}); var marker%d = new BMap.Marker(new BMap.Point(%.4f, %.4f));map.addOverlay(marker%d);\n", j, graph.vertices[w1].data.country, graph.vertices[w1].data.city, j, j, j + 1, graph.vertices[w2].data.longitude, graph.vertices[w2].data.latitude, j + 1);
		fprintf(fp, "var infoWindow%d = new BMap.InfoWindow(\"<p style = 'fontsize:14px;'>country: %s<br/>city : %s</p>\");marker%d.addEventListener(\"click\", function(){this.openInfoWindow(infoWindow%d);}); var contentString%.2d = '%s, %s --> %s, %s (%s - %.2f hours - $%.2f - %s)';var path%d = new BMap.Polyline([new BMap.Point(%.4f, %.4f),new BMap.Point(%.4f, %.4f)], {strokeColor:'#25a45b', strokeWeight:8, strokeOpacity:0.8});map.addOverlay(path%d);path%d.addEventListener(\"click\", function(){alert(contentString%.2d);});", j + 1, graph.vertices[w2].data.country, graph.vertices[w2].data.city, j + 1, j + 1, j + 1, graph.vertices[w1].data.city, graph.vertices[w1].data.country, graph.vertices[w2].data.city, graph.vertices[w2].data.country, arc.kind, arc.time, arc.money, arc.info, j + 1, graph.vertices[w1].data.longitude, graph.vertices[w1].data.latitude, graph.vertices[w2].data.longitude, graph.vertices[w2].data.latitude, j + 1, j + 1, j + 1);
		j++;
	}
	fprintf(fp, "</script>");
	fclose(fp);
	return true;
}

void help()
{
	
	printf("**************\n");
	printf("/*菜单*/\n");
	printf("1.求最短路径\n");
	printf("2.规定交通方式的最短路径\n");
	printf("3.求DFS路径\n");
	printf("4.求BFS路径\n");
	printf("5.help\n");
	printf("6.exit\n"); 
	
	printf("**************\n");
}
void outputDist(int v, ShortPathTable& D, char kind)
{
	if (kind == 'T')
	{
		printf("The shortest time is: %.2lf\n ", D[v]);
	}
	else if (kind == 'M')
	{
		printf("The shortest money is: %.2lf\n", D[v]);
	}
}
void outputPATH(int v, Graph& graph)
{
	stack<int>q;//由于Path[]中每个顶点储存的为上一个顶点的下标，故新建一个栈，栈底为终点，栈顶为起点 
	int w = v;//终点
	while (Path[w] != -1)
	{
		q.push(w);
		w = Path[w];
	}
	q.push(w); //将起点压入栈中
	int first = 1;
	while (q.size() != 0)
	{
		int tmpw = q.top();//第j次边的开始顶点   //j=0是起点
		q.pop();
		if (first)
		{
			printf("%s", &graph.vertices[tmpw].data.city);
			first = 0;
		}
		else
		{
			printf("->%s", &graph.vertices[tmpw].data.city);
		}
	}
	printf("\n");
}
void output(int v, Graph& graph, char kind)
{
	stack<int>q;//由于Path[]中每个顶点储存的为上一个顶点的下标，故新建一个栈，栈底为终点，栈顶为起点 
	int w = v;//终点
	while (Path[w] != -1)
	{
		q.push(w);
		w = Path[w];
	}
	q.push(w); //将起点压入栈中
	while (q.size() != 1)
	{
		int w1 = q.top();//第j次边的开始顶点   //j=0是起点
		q.pop();
		int w2=q.top();
		ArcNode arc;
		GetArc(graph, w1, w2, arc, kind);
		printf("%s->%s\n", graph.vertices[w1].data.city,graph.vertices[w2].data.city);
		printf("Money：%.2lf\n", arc.money);
		printf("Time: %.2lf\n", arc.time);
		printf("Means of Transportation：%s\n", arc.kind);
		printf("\n");
	}
}
void output2(int v, Graph& graph, char Kind,char* kind)
{
	stack<int>q;//由于Path[]中每个顶点储存的为上一个顶点的下标，故新建一个栈，栈底为终点，栈顶为起点 
	int w = v;//终点
	while (Path[w] != -1)
	{
		q.push(w);
		w = Path[w];
	}
	q.push(w); //将起点压入栈中
	while (q.size() != 1)
	{
		int w1 = q.top();//第j次边的开始顶点   //j=0是起点
		q.pop();
		int w2 = q.top();
		ArcNode arc;
		GetArc2(graph, w1, w2, arc, Kind,kind);
		printf("%s->%s\n", graph.vertices[w1].data.city, graph.vertices[w2].data.city);
		printf("Money：%.2lf\n", arc.money);
		printf("Time: %.2lf\n", arc.time);
		printf("Means of Transportation：%s\n", arc.kind);
		printf("\n");
	}
}
int main()
{
	vector<City> city;
	Graph graph;
	ShortPathTable Dist;
	char start[32] = { 0 };//起点城市
	char des[32] = { 0 }; //目的地
	char Kind;//选择的权值类型,‘T’为时间，‘M’为距离 
	char kind[6] = "\0";
	CreateCities("cities.csv", city);
	CreateGraph("routes.csv", graph, city);

	help();
	while (true)
	{
		int flag;
		printf("\n\n请输入数字：");
		scanf_s("%d", &flag);
		switch (flag)
		{
		case 1:
		{
			printf("请输入起点城市:");
			scanf_s("%s", &start, 32);
			char c = getchar();//滤去换行符  
			printf("请输入终点城市:");
			scanf_s("%s", &des, 32);
			c = getchar();
			printf("如果求最少money请输入M，如果求最少time请输入T:");
			scanf_s("%c", &Kind, 1);

			int v0 = SearchCity(start, city);//v0为起点下标  
			int v = SearchCity(des, city);//v为终点下标
			Timer t;
			t.Start();
			ShortestPath(graph, v0, Dist, Kind);//求最短路径 
			t.Stop();
			printf(" Time elapsed is : %lf (ms). \n ", t.ElapsedTime());

			printf("The shortest path from %s to %s is:\n", start, des);
			outputPATH(v, graph);
			outputDist(v, Dist, Kind);
			printf("是否查看每次旅程信息，Y or N：");
			c = getchar();
			char option='O';
			scanf_s("%c", &option, 1);
			if (option == 'Y')
			{
				output(v, graph, Kind);
			}
			while (option != 'Y' && option != 'N')
			{
				printf("是否查看每次旅程信息，Y or N：");
				c = getchar();
				scanf_s("%c", &option, 1);
				if (option == 'Y')
				{
					output(v, graph, Kind);
				}
			}

			if (createhtml(graph, start, des, v, Kind))
			{
				printf("创建网页成功。\n");
				ShellExecuteA(NULL, "open", "chrome.exe", "tmp.html", NULL, SW_SHOWNORMAL);
			}
		}break;
		case 2:
		{
			printf("请输入起点城市:");
			scanf_s("%s", &start, 32);
			char c = getchar();//滤去换行符  
			printf("请输入终点城市:");
			scanf_s("%s", &des, 32);
			c = getchar();
			printf("如果求最少money请输入M，如果求最少time请输入T:");
			scanf_s("%c", &Kind, 1);
			c = getchar();
			printf("请输入交通方式:");
			scanf_s("%s", &kind, 6);

			int v0 = SearchCity(start, city);//v0为起点下标  
			int v = SearchCity(des, city);//v为终点下标
			Timer t;
			t.Start();
			ShortestPath2(graph, v0, Dist, Kind, kind);//求最短路径 
			t.Stop();
			printf(" Time elapsed is : %lf (ms). \n ", t.ElapsedTime());
			printf("The shortest path from %s to %s is:\n", start, des);
			outputPATH(v, graph);
			outputDist(v, Dist, Kind);

			printf("是否查看每次旅程信息，Y or N：");
			c = getchar();
			char option = 'O';
			scanf_s("%c", &option, 1);
			if (option == 'Y')
			{
				output2(v, graph, Kind, kind);
			}
			while (option != 'Y' && option != 'N')
			{
				printf("是否查看每次旅程信息，Y or N：");
				c = getchar();
				scanf_s("%c", &option, 1);
				if (option == 'Y')
				{
					output2(v, graph, Kind, kind);
				}
			}
			if (createhtml(graph, start, des, v, Kind))
			{
				printf("创建网页成功。\n");
				ShellExecuteA(NULL, "open", "chrome.exe", "tmp.html", NULL, SW_SHOWNORMAL);
			}
		}break;
		case 3:
		{
			for (int i = 0; i <int(city.size()); i++)
			{
				city[i].pass = false;
			}
			printf("请输入起点城市:");
			scanf_s("%s", &start, 32);
			int v0 = SearchCity(start, city);//v0为起点下标  
			
			Timer t;
			t.Start();
			DFS(graph, v0, city);
			t.Stop();
			printf("\n Time elapsed is : %lf (ms). \n ", t.ElapsedTime());
		}break;
		case 4:
		{
			for (int i=0;i<int(city.size());i++)
			{
				city[i].pass = false;
			}
			printf("请输入起点城市:");
			scanf_s("%s", &start, 32);
			int v0 = SearchCity(start, city);//v0为起点下标  
			
			Timer t;
			t.Start();
			BFS(graph, v0, city);
			t.Stop();
			printf("\n Time elapsed is : %lf (ms). \n ", t.ElapsedTime());	
		}break;
		case 5:
		{
			help();
		}break;
		case 6: {
			return 0;
		}break;
		default:
			break;
		}
	}
	return 0;
}


