#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <cstdlib>

using namespace std;

// Set to true to print detailed neighbor update debug info for the first few iterations.
const bool detailedNeighborDebug = true;
const int detailedNeighborDebugLimit = 10;

// ----- Graph structure using vector of sorted neighbor lists -----
struct Graph
{
    vector<vector<int>> adj; // each vertex's neighbor list
    int vertexCount;

    Graph(int n) : vertexCount(n)
    {
        adj.resize(n);
        cout << "Graph: Initialized with " << n << " vertices." << endl;
    }
};

// ----- Read graph from file and build a 0-indexed graph -----
Graph readGraphFromFile(const string &filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error opening input file: " << filename << endl;
        exit(1);
    }

    unordered_map<int, int> vertexMap;
    vector<pair<int, int>> edges;
    int u, v;
    while (file >> u >> v)
    {
        edges.emplace_back(u, v);
        if (vertexMap.find(u) == vertexMap.end())
            vertexMap[u] = vertexMap.size();
        if (vertexMap.find(v) == vertexMap.end())
            vertexMap[v] = vertexMap.size();
    }
    file.close();

    int n = vertexMap.size();
    cout << "readGraphFromFile: " << edges.size() << " edges read. Creating graph with " << n << " vertices." << endl;
    Graph graph(n);
    for (const auto &edge : edges)
    {
        int u_new = vertexMap[edge.first];
        int v_new = vertexMap[edge.second];
        graph.adj[u_new].push_back(v_new);
        graph.adj[v_new].push_back(u_new);
    }
    // Sort each neighbor list.
    for (int i = 0; i < n; i++)
    {
        sort(graph.adj[i].begin(), graph.adj[i].end());
    }
    cout << "readGraphFromFile: Graph creation complete." << endl;
    return graph;
}

// ----- Optimized Compute Degeneracy Ordering with additional debug -----
// This version uses buckets and tracks each vertex's position in its bucket.
// Detailed neighbor-update debugging is printed for the first few processed vertices.
vector<int> degeneracyOrder(const Graph &graph)
{
    int n = graph.vertexCount;
    vector<int> degree(n);
    int maxDeg = 0;
    for (int i = 0; i < n; i++)
    {
        degree[i] = graph.adj[i].size();
        if (degree[i] > maxDeg)
            maxDeg = degree[i];
    }

    // Create buckets for degrees 0..maxDeg.
    vector<vector<int>> buckets(maxDeg + 1);
    vector<int> pos(n, -1);
    for (int i = 0; i < n; i++)
    {
        int d = degree[i];
        pos[i] = buckets[d].size();
        buckets[d].push_back(i);
    }

    vector<bool> removed(n, false);
    vector<int> order;
    order.reserve(n);
    int debugInterval = 100000; // Print progress every 100,000 removals.

    for (int k = 0; k < n; k++)
    {
        // Find the smallest bucket index that is non-empty.
        int minBucket = -1;
        for (int d = 0; d <= maxDeg; d++)
        {
            if (!buckets[d].empty())
            {
                minBucket = d;
                break;
            }
        }
        if (minBucket == -1)
        {
            cout << "degeneracyOrder: All buckets empty at iteration " << k << ". Breaking out." << endl;
            break;
        }
        int v = buckets[minBucket].back();
        buckets[minBucket].pop_back();
        removed[v] = true;
        order.push_back(v);

        if (k % debugInterval == 0)
        {
            cout << "degeneracyOrder: Processed " << k
                 << " vertices. Current vertex: " << v
                 << " from bucket[" << minBucket << "], bucket size now: "
                 << buckets[minBucket].size() << endl;
        }

        // Update the degrees for each neighbor of v.
        for (int u : graph.adj[v])
        {
            if (!removed[u])
            {
                int d_old = degree[u];
                // Check that the bucket is not empty.
                if (buckets[d_old].empty())
                {
                    cout << "Error: buckets[" << d_old << "] is unexpectedly empty for vertex " << u << endl;
                    continue;
                }
                int pos_u = pos[u];
                if (pos_u >= buckets[d_old].size())
                {
                    cout << "Error: pos[" << u << "] = " << pos_u << " out of range for bucket[" << d_old
                         << "] of size " << buckets[d_old].size() << endl;
                    continue;
                }
                int last_vertex = buckets[d_old].back();
                // Debug print for detailed neighbor updates for early iterations.
                if (k < detailedNeighborDebugLimit && detailedNeighborDebug)
                {
                    cout << "Updating neighbor " << u << ": d_old = " << d_old << ", pos[u] = " << pos_u
                         << ", last_vertex = " << last_vertex << endl;
                }
                // Swap u with the last element in its bucket.
                buckets[d_old][pos_u] = last_vertex;
                pos[last_vertex] = pos_u;
                buckets[d_old].pop_back();

                // Decrease degree.
                degree[u]--;
                int d_new = degree[u];
                pos[u] = buckets[d_new].size();
                buckets[d_new].push_back(u);
                if (k < detailedNeighborDebugLimit && detailedNeighborDebug)
                {
                    cout << "Neighbor " << u << " moved to bucket[" << d_new << "]." << endl;
                }
            }
        }
    }
    cout << "degeneracyOrder: Completed ordering of " << order.size() << " vertices." << endl;
    return order;
}

// ----- Global output file -----
ofstream outputFile;

// ----- Write a clique to the output file -----
void writeClique(const vector<int> &clique)
{
    ostringstream oss;
    for (int v : clique)
    {
        oss << v << ",";
    }
    oss << "\n";
    outputFile << oss.str();
}

// ----- Bron-Kerbosch algorithm with pivoting -----
void BronKerbosch(vector<int> &R, vector<int> &P, vector<int> &X, const Graph &graph)
{
    if (P.empty() && X.empty())
    {
        writeClique(R);
        return;
    }
    int pivot = -1;
    int maxCount = -1;
    vector<int> unionPX;
    unionPX.insert(unionPX.end(), P.begin(), P.end());
    unionPX.insert(unionPX.end(), X.begin(), X.end());
    for (int u : unionPX)
    {
        int count = 0;
        for (int w : P)
        {
            if (binary_search(graph.adj[u].begin(), graph.adj[u].end(), w))
                count++;
        }
        if (count > maxCount)
        {
            maxCount = count;
            pivot = u;
        }
    }
    vector<int> P_without_neighbors;
    for (int v : P)
    {
        if (!binary_search(graph.adj[pivot].begin(), graph.adj[pivot].end(), v))
            P_without_neighbors.push_back(v);
    }
    for (int v : P_without_neighbors)
    {
        vector<int> newR = R;
        newR.push_back(v);
        vector<int> newP;
        vector<int> newX;
        for (int u : P)
        {
            if (binary_search(graph.adj[v].begin(), graph.adj[v].end(), u))
                newP.push_back(u);
        }
        for (int u : X)
        {
            if (binary_search(graph.adj[v].begin(), graph.adj[v].end(), u))
                newX.push_back(u);
        }
        BronKerbosch(newR, newP, newX, graph);
        auto it = find(P.begin(), P.end(), v);
        if (it != P.end())
            P.erase(it);
        X.push_back(v);
    }
}

// ----- Enumerate maximal cliques using degeneracy ordering -----
void enumerateMaximalCliques(const Graph &graph, const vector<int> &degOrder)
{
    int n = graph.vertexCount;
    vector<int> orderIndex(n, 0);
    for (int i = 0; i < n; i++)
    {
        orderIndex[degOrder[i]] = i;
    }
    for (int i = 0; i < n; i++)
    {
        int v = degOrder[i];
        vector<int> R = {v};
        vector<int> P;
        vector<int> X;
        for (int w : graph.adj[v])
        {
            if (orderIndex[w] > orderIndex[v])
                P.push_back(w);
        }
        BronKerbosch(R, P, X, graph);
    }
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string inputFile = "email-Enron.txt";
    string outputFileName = "email-Enron-output.txt";

    cout << "main: Reading graph from file..." << endl;
    Graph graph = readGraphFromFile(inputFile);
    cout << "main: Graph created with " << graph.vertexCount << " vertices." << endl;

    cout << "main: Computing degeneracy ordering..." << endl;
    vector<int> degOrder = degeneracyOrder(graph);

    cout << "main: Enumerating maximal cliques..." << endl;
    outputFile.open(outputFileName);
    if (!outputFile.is_open())
    {
        cerr << "main: Error opening output file." << endl;
        return 1;
    }
    enumerateMaximalCliques(graph, degOrder);
    cout << "main: Clique enumeration complete." << endl;

    outputFile.close();
    cout << "main: Maximal cliques written to " << outputFileName << endl;
    return 0;
}