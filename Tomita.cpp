#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <algorithm>

using namespace std;

class Graph
{
public:
    unordered_map<int, unordered_set<int>> adjacencyList;
    int vertexCount;

    Graph(int n) : vertexCount(n)
    {
        for (int i = 0; i < n; i++)
        {
            adjacencyList[i] = unordered_set<int>();
        }
    }

    void addEdge(int u, int v)
    {
        adjacencyList[u].insert(v);
        adjacencyList[v].insert(u);
    }

    const unordered_set<int> &getNeighbors(int v) const
    {
        return adjacencyList.at(v);
    }
};

class MaximalCliques
{
private:
    const Graph &graph;
    ofstream outputFile;

public:
    MaximalCliques(const Graph &g, const string &outputFilename) : graph(g)
    {
        outputFile.open(outputFilename);
        if (!outputFile.is_open())
        {
            cerr << "Error opening output file: " << outputFilename << endl;
            exit(1);
        }
    }

    ~MaximalCliques()
    {
        if (outputFile.is_open())
        {
            outputFile.close();
        }
    }

    void findCliques()
    {
        set<int> SUBG, CAND;
        for (int i = 0; i < graph.vertexCount; i++)
        {
            SUBG.insert(i);
            CAND.insert(i);
        }
        expand(SUBG, CAND);
    }

private:
    void expand(set<int> &SUBG, set<int> &CAND)
    {
        if (SUBG.empty())
        {
            outputFile << "clique," << endl;
            return;
        }

        int pivot = *SUBG.begin();
        size_t maxIntersection = 0;
        for (int u : SUBG)
        {
            size_t intersectionSize = intersectionSizeWithSet(CAND, graph.getNeighbors(u));
            if (intersectionSize > maxIntersection)
            {
                maxIntersection = intersectionSize;
                pivot = u;
            }
        }

        set<int> EXTU;
        set_difference(CAND.begin(), CAND.end(),
                       graph.getNeighbors(pivot).begin(), graph.getNeighbors(pivot).end(),
                       inserter(EXTU, EXTU.begin()));

        for (int q : EXTU)
        {
            outputFile << q << "," << endl;

            set<int> SUBGq, CANDq;
            set_intersection(SUBG.begin(), SUBG.end(),
                             graph.getNeighbors(q).begin(), graph.getNeighbors(q).end(),
                             inserter(SUBGq, SUBGq.begin()));
            set_intersection(CAND.begin(), CAND.end(),
                             graph.getNeighbors(q).begin(), graph.getNeighbors(q).end(),
                             inserter(CANDq, CANDq.begin()));

            expand(SUBGq, CANDq);

            CAND.erase(q);
            outputFile << "back," << endl;
        }
    }

    size_t intersectionSizeWithSet(const set<int> &a, const unordered_set<int> &b) const
    {
        return count_if(a.begin(), a.end(), [&b](int x)
                        { return b.find(x) != b.end(); });
    }
};

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

    Graph graph(vertexMap.size());
    for (const auto &edge : edges)
    {
        graph.addEdge(vertexMap[edge.first], vertexMap[edge.second]);
    }

    return graph;
}
#include <chrono>
int main()
{
    string inputFile = "as-skitter.txt";
    string outputFile = "Wiki-Vote-Max-output.txt";
    auto startTime = chrono::high_resolution_clock::now();

    cout << "Reading graph from file..." << endl;
    Graph graph = readGraphFromFile(inputFile);

    cout << "Graph created with " << graph.vertexCount << " vertices." << endl;
    cout << "Finding maximal cliques..." << endl;

    MaximalCliques mc(graph, outputFile);
    mc.findCliques();

    cout << "Maximal cliques written to " << outputFile << endl;

    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
    cout << "Execution time: " << duration.count() << "ms" << endl;
    return 0;
}