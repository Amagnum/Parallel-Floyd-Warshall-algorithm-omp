/*

Problem Statement:

Write a multi-core program for the “all-pairs shortest paths” problem. 
The input is a weighted graph with no negative cycles and the expected 
output are lengths of the shortest paths between all pairs of vertices 
(where the length of a path is a sum of weights along the edges that 
the path consists of). Write a sequential program (no OpenMP directives 
at all) as well. Compare the running time of the sequential program 
with the running time of the multi-core program and compute the speedup
achieved.

1. for different number of cores and different number of threads per core, and
2. for different number of vertices and different number of edges.

How to run?
$ g++ -fopenmp apsp.cpp -o apsp
$ ./apsp < file10_10.in

*/

#include <bits/stdc++.h>
#include <time.h>
#include <omp.h>

using namespace std;

#define INF 99999

vector<vector<int>> floydWarshall (vector<vector<int>> graph)
{
	int V = graph.size();
	vector<vector<int>> dist(V,vector<int>(V));
	int i, j, k;

	dist = graph;

	for (k = 0; k < V; k++)
	{
		for (i = 0; i < V; i++)
		{
			for (j = 0; j < V; j++)
			{
				if (dist[i][k] + dist[k][j] < dist[i][j])
					dist[i][j] = dist[i][k] + dist[k][j];
			}
		}
	}

	return dist;
}


vector<vector<int>> floydWarshall_Parallel (vector<vector<int>> graph)
{
	int V = graph.size();
	vector<vector<int>> dist(V,vector<int>(V));
	int j, i, k, nthreads;

	dist = graph;

	for(nthreads=1; nthreads <= 10; nthreads++) {

	    omp_set_num_threads(nthreads);
	    
	    double start_time = omp_get_wtime();
	    
	    #pragma omp parallel shared(dist)
	    for (k = 0; k < V; k++)
	    {
		      vector<int> *dm = &dist[k];
		      #pragma omp parallel for private(i, j) schedule(dynamic)
		      for (i = 0; i < V; i++)
		      {
			        vector<int> *ds = &dist[i];
			        for (j = 0; j < V; j++)
			        {
			        	#pragma omp critical
			          	ds->at(j) = min(ds->at(j),ds->at(k)+dm->at(j));
			        }
		      }
		}
		    
	    double time = omp_get_wtime() - start_time;
	    printf("Total time for thread %d (in sec): %.8f\n", nthreads, time);
 		//printf("%.8f%d\n", time,nthreads);
 	
 	}
	return dist;
}

void printDiatance(vector<vector<int>> dist)
{
	int V = dist.size();
	for (int i = 0; i < V; i++)
	{
		for (int j = 0; j < V; j++)
		{
			if (dist[i][j] != INF)
				cout<<i<<" -> "<<j<<" : "<<dist[i][j]<<endl;
		}
	}
}

int main()
{

	int nV, nE;
	cin >> nV;
	cin >> nE;

	cout<<"No. of Nodes: "<<nV<<endl;
	cout<<"No. of Edges: "<<nE<<endl;
	
	vector<vector<int>> graph(nV,vector<int>(nV));

	for (int i = 0; i < nV; ++i)
		for (int j = 0; j < nV; ++j)
			graph[i][j] = i==j?0:INF;

	
	/* 
	*	Takeing Graph Input
	*/
	int to,from, weight;
	for (int i = 0; i < nE; ++i)
	{
		cin >> from >> to >> weight;
		graph[to][from] = weight;
	}

	/* 
	*	Sequential All pair shortest path
	*/

	double start_time = omp_get_wtime();
	vector<vector<int>> ans = floydWarshall(graph);
	double time = omp_get_wtime() - start_time;
    printf("\nTotal time for Sequential (in sec): %.8f\n\n", time);
	
	/* 
	*	Multiprocessing All pair shortest path
	*/
	vector<vector<int>> ans2 = floydWarshall_Parallel(graph);
	
	//assert(ans==ans2);
	printDiatance(ans);

    printf("\nProgram Finished...\n");
	
	return 0;
}

