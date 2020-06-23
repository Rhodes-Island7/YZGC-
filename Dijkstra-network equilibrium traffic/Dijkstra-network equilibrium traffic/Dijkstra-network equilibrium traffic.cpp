#include <iostream>
#include<cstdio>
#include<cstring>
#include<vector>
#include <stdlib.h>
#include <fstream>
#include <string>

#define Infinity 2147483647

using namespace std;
//存放原始拓扑信息
vector<int> gridTopo[956];
//存放所有业务需求信息
vector<int> request[4001];
vector<int> TempRequest[4001];

int **AdjacencyMatrix, **TempAdjacencyMatrix, **CapacityArray, **TempCapacityArray, *RequestArray;

struct Dis 
{
	int Cost;
	bool IsVisit;
	string Path;
	Dis()
	{
		Cost = 0;
		IsVisit = false;
		Path = "";
	}
};
Dis * DisArray;

struct ResultPrint
{
	int Cost;
	int YZG;
	string Path;
};
ResultPrint * ResultPrintArray;

void clearVec()
{
	for (int i = 0; i<956; i++) gridTopo[i].clear();
	for (int i = 0; i<4001; i++) request[i].clear();
}

//void readTxt()   //改进后未用
//{
//	char readLine[1000];
//	const char *delim = " ";
//	char *p;
//	for (int i = 0; i<956; i++)
//	{
//		cin.getline(readLine, 1000);
//		p = strtok(readLine, delim);
//		while (p)
//		{
//			gridTopo[i].push_back(atoi(p));
//			p = strtok(NULL, delim);
//		}
//	}
//	for (int i = 0; i<4001; i++)
//	{
//		cin.getline(readLine, 1000);
//		p = strtok(readLine, delim);
//		while (p)
//		{
//			request[i].push_back(atoi(p));
//			p = strtok(NULL, delim);
//		}
//	}
//}

void readTxt()
{
	ifstream Data("gridtopoAndRequest.txt");
	string Dataline;

	if (Data)
	{
		char readLine[1000];
		const char *delim = " ";
		char *p;
		for (int i = 0; i<956; i++)
		{
			getline(Data, Dataline);
			strcpy(readLine, Dataline.c_str());
			p = strtok(readLine, delim);
			while (p)
			{
				gridTopo[i].push_back(atoi(p));
				p = strtok(NULL, delim);
			}
		}
		for (int i = 0; i<4001; i++)
		{
			getline(Data, Dataline);
			strcpy(readLine, Dataline.c_str());
			p = strtok(readLine, delim);
			while (p)
			{
				request[i].push_back(atoi(p));
				TempRequest[i].push_back(atoi(p));
				p = strtok(NULL, delim);
			}
		}
	}
	else
	{
		cout << "File path error!" << endl;
	}
}

void Init()
{
	AdjacencyMatrix = new int*[gridTopo[0].at(0)];   //邻接矩阵
	TempAdjacencyMatrix = new int*[gridTopo[0].at(0)];   //临时邻接矩阵
	CapacityArray = new int*[gridTopo[0].at(0)];   //容量数组
	TempCapacityArray = new int*[gridTopo[0].at(0)];   //临时容量数组
	for (int i = 0; i < gridTopo[0].at(0); i++)
	{
		AdjacencyMatrix[i] = new int[gridTopo[0].at(0)];
		TempAdjacencyMatrix[i] = new int[gridTopo[0].at(0)];
		CapacityArray[i] = new int[gridTopo[0].at(0)];
		TempCapacityArray[i] = new int[gridTopo[0].at(0)];
	}
	for (int i = 0; i < gridTopo[0].at(0); i++)
	{
		for (int j = 0; j < gridTopo[0].at(0); j++)
		{
			//AdjacencyMatrix[i][j] = INT_MAX;
			AdjacencyMatrix[i][j] = Infinity;
			CapacityArray[i][j] = 0;
		}
	}
	for (int i = 0; i < gridTopo[0].at(1); i++)
	{
		AdjacencyMatrix[gridTopo[i + 1].at(0)][gridTopo[i + 1].at(1)] = gridTopo[i + 1].at(3);
		AdjacencyMatrix[gridTopo[i + 1].at(1)][gridTopo[i + 1].at(0)] = gridTopo[i + 1].at(3);
		CapacityArray[gridTopo[i + 1].at(0)][gridTopo[i + 1].at(1)] = gridTopo[i + 1].at(2) * 0.8;
		CapacityArray[gridTopo[i + 1].at(1)][gridTopo[i + 1].at(0)] = gridTopo[i + 1].at(2) * 0.8;
	}
	TempAdjacencyMatrix = AdjacencyMatrix;
	TempCapacityArray = CapacityArray;

	RequestArray = new int[request[0].at(0)];   //各线所需带宽
	for (int i = 0; i < request[0].at(0); i++)
	{
		RequestArray[i] = request[i * (request[0].at(1) + 1) + 1].at(1);
	}

	ResultPrintArray = new ResultPrint[request[0].at(0)];
}

void BubbleSort()
{
	int TempValue, TempMinSize, TempMaxSize;
	vector<int> TempDeposit[1];
	for (int i = 1; i < request[0].at(0); i++)
	{
		for (int j = 1; j  < request[0].at(0); j++)
		{
			if (request[j * (request[0].at(1) + 1) - request[0].at(1)].at(1) < request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1)].at(1))
			{
				TempValue = request[j * (request[0].at(1) + 1) - request[0].at(1)].at(1);
				request[j * (request[0].at(1) + 1) - request[0].at(1)].at(1) = request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1)].at(1);
				request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1)].at(1) = TempValue;

				TempValue = request[j * (request[0].at(1) + 1) - request[0].at(1)].at(0);
				request[j * (request[0].at(1) + 1) - request[0].at(1)].at(0) = request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1)].at(0);
				request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1)].at(0) = TempValue;

				if (request[j * (request[0].at(1) + 1) - request[0].at(1) + 1].size() < request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 1].size())
				{
					TempDeposit[0].clear();
					TempMinSize = request[j * (request[0].at(1) + 1) - request[0].at(1) + 1].size();
					TempMaxSize = request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 1].size();
					for (int k = 0; k < TempMinSize; k++)
					{
						TempDeposit[0].push_back(request[j * (request[0].at(1) + 1) - request[0].at(1) + 1].at(k));
					}
					request[j * (request[0].at(1) + 1) - request[0].at(1) + 1].clear();
					for (int k = 0; k < TempMaxSize; k++)
					{
						request[j * (request[0].at(1) + 1) - request[0].at(1) + 1].push_back(request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 1].at(k));
					}
					request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 1].clear();
					for (int k = 0; k < TempMinSize; k++)
					{
						request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 1].push_back(TempDeposit[0].at(k));
					}
				}
				else
				{
					TempDeposit[0].clear();
					TempMinSize = request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 1].size();
					TempMaxSize = request[j * (request[0].at(1) + 1) - request[0].at(1) + 1].size();
					for (int k = 0; k < TempMinSize; k++)
					{
						TempDeposit[0].push_back(request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 1].at(k));
					}
					request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 1].clear();
					for (int k = 0; k < TempMaxSize; k++)
					{
						request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 1].push_back(request[j * (request[0].at(1) + 1) - request[0].at(1) + 1].at(k));
					}
					request[j * (request[0].at(1) + 1) - request[0].at(1) + 1].clear();
					for (int k = 0; k < TempMinSize; k++)
					{
						request[j * (request[0].at(1) + 1) - request[0].at(1) + 1].push_back(TempDeposit[0].at(k));
					}
				}

				if (request[j * (request[0].at(1) + 1) - request[0].at(1) + 2].size() < request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 2].size())
				{
					TempDeposit[0].clear();
					TempMinSize = request[j * (request[0].at(1) + 1) - request[0].at(1) + 2].size();
					TempMaxSize = request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 2].size();
					for (int k = 0; k < TempMinSize; k++)
					{
						TempDeposit[0].push_back(request[j * (request[0].at(1) + 1) - request[0].at(1) + 2].at(k));
					}
					request[j * (request[0].at(1) + 1) - request[0].at(1) + 2].clear();
					for (int k = 0; k < TempMaxSize; k++)
					{
						request[j * (request[0].at(1) + 1) - request[0].at(1) + 2].push_back(request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 2].at(k));
					}
					request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 2].clear();
					for (int k = 0; k < TempMinSize; k++)
					{
						request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 2].push_back(TempDeposit[0].at(k));
					}
				}
				else
				{
					TempDeposit[0].clear();
					TempMinSize = request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 2].size();
					TempMaxSize = request[j * (request[0].at(1) + 1) - request[0].at(1) + 2].size();
					for (int k = 0; k < TempMinSize; k++)
					{
						TempDeposit[0].push_back(request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 2].at(k));
					}
					request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 2].clear();
					for (int k = 0; k < TempMaxSize; k++)
					{
						request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 2].push_back(request[j * (request[0].at(1) + 1) - request[0].at(1) + 2].at(k));
					}
					request[j * (request[0].at(1) + 1) - request[0].at(1) + 2].clear();
					for (int k = 0; k < TempMinSize; k++)
					{
						request[j * (request[0].at(1) + 1) - request[0].at(1) + 2].push_back(TempDeposit[0].at(k));
					}
				}

				if (request[j * (request[0].at(1) + 1) - request[0].at(1) + 3].size() < request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 3].size())
				{
					TempDeposit[0].clear();
					TempMinSize = request[j * (request[0].at(1) + 1) - request[0].at(1) + 3].size();
					TempMaxSize = request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 3].size();
					for (int k = 0; k < TempMinSize; k++)
					{
						TempDeposit[0].push_back(request[j * (request[0].at(1) + 1) - request[0].at(1) + 3].at(k));
					}
					request[j * (request[0].at(1) + 1) - request[0].at(1) + 3].clear();
					for (int k = 0; k < TempMaxSize; k++)
					{
						request[j * (request[0].at(1) + 1) - request[0].at(1) + 3].push_back(request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 3].at(k));
					}
					request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 3].clear();
					for (int k = 0; k < TempMinSize; k++)
					{
						request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 3].push_back(TempDeposit[0].at(k));
					}
				}
				else
				{
					TempDeposit[0].clear();
					TempMinSize = request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 3].size();
					TempMaxSize = request[j * (request[0].at(1) + 1) - request[0].at(1) + 3].size();
					for (int k = 0; k < TempMinSize; k++)
					{
						TempDeposit[0].push_back(request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 3].at(k));
					}
					request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 3].clear();
					for (int k = 0; k < TempMaxSize; k++)
					{
						request[(j + 1) * (request[0].at(1) + 1) - request[0].at(1) + 3].push_back(request[j * (request[0].at(1) + 1) - request[0].at(1) + 3].at(k));
					}
					request[j * (request[0].at(1) + 1) - request[0].at(1) + 3].clear();
					for (int k = 0; k < TempMinSize; k++)
					{
						request[j * (request[0].at(1) + 1) - request[0].at(1) + 3].push_back(TempDeposit[0].at(k));
					}
				}
			}
		}
	}
}

void JudgeCapacity(int LineNumber)
{
	for (int i = 0; i < gridTopo[0].at(0); i++)
	{
		for (int j = 0; j < gridTopo[0].at(0); j++)
		{
			if (TempCapacityArray[i][j] < request[LineNumber * (request[0].at(1) + 1) + 1].at(1))
			{
				//TempAdjacencyMatrix[i][j] = INT_MAX;
				TempAdjacencyMatrix[i][j] = Infinity;
			}
		}
	}
}

//void DataCalculation(int SourcePoint)   //原型算法
//{
//	DisArray = new Dis[gridTopo[0].at(0)];   //Dis数组，存放计算信息
//	for (int i = 0; i < gridTopo[0].at(0); i++)
//	{
//		DisArray[i].Cost = TempAdjacencyMatrix[SourcePoint][i];
//		DisArray[i].Path = to_string(SourcePoint) + " " + to_string(i);
//	}
//	DisArray[SourcePoint].Cost = 0;
//	DisArray[SourcePoint].IsVisit = true;
//
//	int Counts = 1;
//	while (Counts != gridTopo[0].at(0))
//	{
//		int TempPoint = 0;
//		int MinCost = INT_MAX;
//		for (int i = 0; i < gridTopo[0].at(0); i++)
//		{
//			if (!DisArray[i].IsVisit && DisArray[i].Cost < MinCost)
//			{
//				MinCost = DisArray[i].Cost;
//				TempPoint = i;
//			}
//		}
//
//		DisArray[TempPoint].IsVisit = true;
//		Counts++;
//
//		for (int i = 0; i < gridTopo[0].at(0); i++)
//		{
//			if (!DisArray[i].IsVisit && TempAdjacencyMatrix[TempPoint][i] != INT_MAX && DisArray[TempPoint].Cost + TempAdjacencyMatrix[TempPoint][i] < DisArray[i].Cost)
//			{
//				DisArray[i].Cost = DisArray[TempPoint].Cost + TempAdjacencyMatrix[TempPoint][i];
//				DisArray[i].Path = DisArray[TempPoint].Path + " " + to_string(i);
//			}
//		}
//	}
//}

void DataCalculation(int LineNumber)
{
	DisArray = new Dis[gridTopo[0].at(0)];   //Dis数组，存放计算信息
	for (int i = 0; i < gridTopo[0].at(0); i++)
	{
		DisArray[i].Cost = TempAdjacencyMatrix[request[LineNumber * (request[0].at(1) + 1) + 2].at(0)][i];
		DisArray[i].Path = to_string(request[LineNumber * (request[0].at(1) + 1) + 2].at(0)) + " " + to_string(i);
	}
	DisArray[request[LineNumber * (request[0].at(1) + 1) + 2].at(0)].Cost = 0;
	DisArray[request[LineNumber * (request[0].at(1) + 1) + 2].at(0)].IsVisit = true;

	int Counts = 1;
	while (Counts != gridTopo[0].at(0))
	{
		int TempPoint = 0;
		//int MinCost = INT_MAX;
		int MinCost = Infinity;
		for (int i = 0; i < gridTopo[0].at(0); i++)
		{
			if (!DisArray[i].IsVisit && DisArray[i].Cost < MinCost)
			{
				MinCost = DisArray[i].Cost;
				TempPoint = i;
			}
		}

		DisArray[TempPoint].IsVisit = true;
		Counts++;

		for (int i = 0; i < gridTopo[0].at(0); i++)
		{
			//if (!DisArray[i].IsVisit && TempAdjacencyMatrix[TempPoint][i] != INT_MAX && DisArray[TempPoint].Cost + TempAdjacencyMatrix[TempPoint][i] < DisArray[i].Cost)
			if (!DisArray[i].IsVisit && TempAdjacencyMatrix[TempPoint][i] != Infinity && DisArray[TempPoint].Cost + TempAdjacencyMatrix[TempPoint][i] < DisArray[i].Cost)
			{
				DisArray[i].Cost = DisArray[TempPoint].Cost + TempAdjacencyMatrix[TempPoint][i];
				DisArray[i].Path = DisArray[TempPoint].Path + " " + to_string(i);
			}
		}
	}
}

void DataMove(int LineNumber)
{
	ResultPrintArray[LineNumber].Cost = DisArray[request[LineNumber * (request[0].at(1) + 1) + 2].at(request[LineNumber * (request[0].at(1) + 1) + 2].size() - 1)].Cost * request[LineNumber * (request[0].at(1) + 1) + 1].at(1);
	ResultPrintArray[LineNumber].Path = DisArray[request[LineNumber * (request[0].at(1) + 1) + 2].at(request[LineNumber * (request[0].at(1) + 1) + 2].size() - 1)].Path;
}

void PostTreat(int LineNumber)
{
	vector<int> TempArray;
	char * TempString = (char *)ResultPrintArray[LineNumber].Path.c_str();
	char * TempP = strtok(TempString, " ");
	while (TempP)
	{
		TempArray.push_back(atoi(TempP));
		TempP = strtok(NULL, " ");
	}
	for (int i = 0; i < TempArray.size() - 1; i++)
	{
		CapacityArray[TempArray.at(i)][TempArray.at(i + 1)] = CapacityArray[TempArray.at(i)][TempArray.at(i + 1)] - request[LineNumber * (request[0].at(1) + 1) + 1].at(1);
	}
	TempAdjacencyMatrix = AdjacencyMatrix;
	TempCapacityArray = CapacityArray;
}

void PrintResult()
{
	int TotalCost = 0;
	for (int i = 0; i < request[0].at(0); i++)
	{
		TotalCost = TotalCost + ResultPrintArray[i].Cost;
	}
	cout << TotalCost << endl;
	for (int i = 0; i < request[0].at(0); i++)
	{
		cout << request[i * 4 + 1].at(0) << " " << request[i * 4 + 1].at(1) << endl;
		cout << ResultPrintArray[i].Path << endl;
	}
}

void Release()
{
	for (int i = 0; i < gridTopo[0].at(0); i++)   //释放空间
	{
		delete[] AdjacencyMatrix[i];
		//delete[] TempAdjacencyMatrix[i];
		delete[] CapacityArray[i];
		//delete[] TempCapacityArray[i];
	}
	AdjacencyMatrix = NULL;
	TempAdjacencyMatrix = NULL;
	CapacityArray = NULL;
	TempCapacityArray = NULL;
	delete[] RequestArray;
	delete[] DisArray;
	delete[] ResultPrintArray;
}

void DataDeal()
{
	Init();
	BubbleSort();
	for (int i = 0; i < request[0].at(0); i++)
	{
		JudgeCapacity(i);
		DataCalculation(i);
		DataMove(i);
		PostTreat(i);
	}
	PrintResult();
	Release();
}

int main()
{
	clearVec();
	//1.输入
	readTxt();

	//2.write your code
	DataDeal();
	system("Pause");

	return 0;
}
