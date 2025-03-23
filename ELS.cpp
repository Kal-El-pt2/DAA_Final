#include <iostream>
#include <vector>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <fstream>

using namespace std;

typedef set<int> Set;
typedef unordered_map<int, Set> Graph;

Set set_difference(const Set &A, const Set &B)
{
    Set result;
    for (int elem : A)
    {
        if (B.find(elem) == B.end())
        {
            result.insert(elem);
        }
    }
    return result;
}

Set set_intersection(const Set &A, const Set &B)
{
    Set result;
    for (int elem : A)
    {
        if (B.find(elem) != B.end())
        {
            result.insert(elem);
        }
    }
    return result;
}

Set set_union(const Set &A, const Set &B)
{
    Set result = A;
    result.insert(B.begin(), B.end());
    return result;
}

void bronKerboschPivot(Set P, Set R, Set X, Graph &graph, ofstream &outfile)
{
    if (P.empty() && X.empty())
    {
        for (int v : R)
            outfile << v << " ";
        outfile << endl;
        return;
    }

    int pivot = -1;
    int maxNeighbors = -1;
    for (int u : P)
    {
        int neighbors = set_intersection(P, graph[u]).size();
        if (neighbors > maxNeighbors)
        {
            maxNeighbors = neighbors;
            pivot = u;
        }
    }

    Set P_copy = P;
    for (int v : P_copy)
    {
        if (graph[pivot].count(v))
            continue;
        bronKerboschPivot(set_intersection(P, graph[v]), set_union(R, {v}), set_intersection(X, graph[v]), graph, outfile);
        P.erase(v);
        X.insert(v);
    }
}

void bronKerboschDegeneracy(vector<int> V, vector<pair<int, int>> E, ofstream &outfile)
{
    Graph graph;
    for (const auto &edge : E)
    {
        int u = edge.first, v = edge.second;
        graph[u].insert(v);
        graph[v].insert(u);
    }

    sort(V.begin(), V.end(), [&](int a, int b)
         { return graph[a].size() < graph[b].size(); });

    Set P(V.begin(), V.end()), X;
    for (int vi : V)
    {
        P.erase(vi);
        bronKerboschPivot(set_intersection(P, graph[vi]), {vi}, set_intersection(X, graph[vi]), graph, outfile);
        X.insert(vi);
    }
}
#include <chrono>
int main()
{
    auto start = chrono::high_resolution_clock::now();

    ifstream infile("email-Enron.txt");
    ofstream outfile("email-Enron.txt_output.txt");

    vector<int> V;
    vector<pair<int, int>> E;
    int u, v;

    while (infile >> u >> v)
    {
        V.push_back(u);
        V.push_back(v);
        E.emplace_back(u, v);
    }

    sort(V.begin(), V.end());
    V.erase(unique(V.begin(), V.end()), V.end());

    bronKerboschDegeneracy(V, E, outfile);

    infile.close();
    outfile.close();

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = end - start;
    cout << "Execution time: " << diff.count() << " s" << endl;

    return 0;
}