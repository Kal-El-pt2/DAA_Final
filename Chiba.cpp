#include <bits/stdc++.h>
#pragma GCC target("avx2")
#pragma GCC optimization("O3")
#pragma GCC optimization("unroll-loops")
using namespace std;

// Fast I/O routines (unchanged)
struct FastInput
{
    static const int BUF_SIZE = 1 << 16;
    int buf_pos, buf_len;
    char buffer[BUF_SIZE];
    FILE *in;
    FastInput(FILE *in = stdin) : buf_pos(0), buf_len(0), in(in) {}
    int getChar()
    {
        if (buf_pos == buf_len)
        {
            buf_len = (int)fread(buffer, 1, BUF_SIZE, in);
            buf_pos = 0;
            if (!buf_len)
                return EOF;
        }
        return buffer[buf_pos++];
    }
    bool readInt(int &number)
    {
        int c = getChar();
        while (c != EOF && isspace(c))
            c = getChar();
        if (c == EOF)
            return false;
        bool neg = false;
        if (c == '-')
        {
            neg = true;
            c = getChar();
        }
        number = 0;
        while (c != EOF && isdigit(c))
        {
            number = number * 10 + c - '0';
            c = getChar();
        }
        if (neg)
            number = -number;
        return true;
    }
};

struct FastOutput
{
    static const int BUF_SIZE = 1 << 16;
    int buf_pos;
    char buffer[BUF_SIZE];
    FILE *out;
    FastOutput(FILE *out = stdout) : buf_pos(0), out(out) {}
    ~FastOutput() { flush(); }
    void flush()
    {
        if (buf_pos)
        {
            fwrite(buffer, 1, buf_pos, out);
            buf_pos = 0;
        }
    }
    void putChar(char c)
    {
        if (buf_pos == BUF_SIZE)
            flush();
        buffer[buf_pos++] = c;
    }
    void writeInt(int number)
    {
        if (number == 0)
        {
            putChar('0');
            return;
        }
        if (number < 0)
        {
            putChar('-');
            number = -number;
        }
        char temp[12];
        int pos = 0;
        while (number)
        {
            temp[pos++] = '0' + (number % 10);
            number /= 10;
        }
        for (int i = pos - 1; i >= 0; --i)
            putChar(temp[i]);
    }
    void writeStr(const char *s)
    {
        while (*s)
        {
            putChar(*s);
            ++s;
        }
    }
};

FastInput fin(stdin);
FastOutput fout(stdout);

class Graph
{
public:
    unordered_map<int, unordered_set<int>> adjList;
    unordered_map<int, int> flagS;
    unordered_map<int, int> flagT;

    void addEdge(int u, int v)
    {
        adjList[u].insert(v);
        adjList[v].insert(u);
    }

    void CLIQUE()
    {
        vector<int> vertices;
        for (const auto &p : adjList)
            vertices.push_back(p.first);
        sort(vertices.begin(), vertices.end(), [this](int a, int b)
             { return adjList[a].size() < adjList[b].size(); });
        for (int v : vertices)
        {
            flagS[v] = 0;
            flagT[v] = 0;
        }
        vector<int> C;
        UPDATE(0, C, vertices);
    }

private:
    void UPDATE(int i, vector<int> &C, const vector<int> &cand)
    {
        if (i == 0)
        {
            vector<int> N = cand;
            vector<int> T;
            FIND_CLIQUE(C, N, T);
        }
    }

    void FIND_CLIQUE(vector<int> &C, vector<int> &N, vector<int> &T)
    {
        sort(N.begin(), N.end(), [this](int a, int b)
             { return adjList[a].size() < adjList[b].size(); });

        if (N.empty() && T.empty())
        {
            if (C.size() >= 3)
            {
                fout.writeStr("Clique:");
                for (int v : C)
                {
                    fout.putChar(' ');
                    fout.writeInt(v);
                }
                fout.putChar('\n');
            }
            return;
        }
        int p = -1;
        {
            vector<int> U = N;
            U.insert(U.end(), T.begin(), T.end());
            if (!U.empty())
                p = U[0];
        }
        vector<int> N_without;
        for (int v : N)
        {
            if (adjList[p].find(v) == adjList[p].end())
                N_without.push_back(v);
        }
        for (int v : N_without)
        {
            vector<int> newC = C;
            newC.push_back(v);
            vector<int> newN, newT;
            for (int w : N)
                if (adjList[v].find(w) != adjList[v].end())
                    newN.push_back(w);
            for (int w : T)
                if (adjList[v].find(w) != adjList[v].end())
                    newT.push_back(w);
            FIND_CLIQUE(newC, newN, newT);
            auto it = find(N.begin(), N.end(), v);
            if (it != N.end())
                N.erase(it);
            T.push_back(v);
        }
    }
};

int main()
{
    freopen("Wiki-Vote.txt", "r", stdin);
    freopen("test-output.txt", "w", stdout);

    Graph g;
    int n, m;
    fin.readInt(n);
    fin.readInt(m);
    for (int i = 0; i < m; i++)
    {
        int u, v;
        fin.readInt(u);
        fin.readInt(v);
        g.addEdge(u, v);
    }
    fout.writeStr("Listing maximal cliques:\n");
    g.CLIQUE();
    return 0;
}
