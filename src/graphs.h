#include <iostream>
#include <limits>
#include <vector>
#include <queue>
using namespace std;

#define INF numeric_limits<T>::max()

namespace error {
    enum graphs {INVALID_INPUT=0xff, ERR_READ_FILE, NOT_CONNECTED, NOT_SIMPLE_DIRECTED, NOT_SIMPLE_LOOP, INV_MEM, INV_COORD, OUT_OF_BOUNDS};
}

struct edge {
    size_t w, a, b;
    edge (size_t W, size_t A, size_t B):  w(W), a(A), b(B) {}
};

struct comp { bool operator() (edge e1, edge e2) { return e1.w > e2.w; } };

template <class T>
class adj {

    mutable vector <bool> fets;
    mutable vector <size_t> vbfs;

    public:

    static bool simple;

    vector <vector <T> > v;

    adj (size_t N=0) { v = vector <vector <T> > (N, vector <T> (N,0)); }

    vector <T>& operator[] (const size_t& p) { if (p < v.size()) return v[p]; else throw error::INV_MEM; }
    T& operator[] (const edge& e) { if(e.a < v.size() and e.b < v.size()) return v[e.a][e.b]; else throw error::INV_MEM; }

    size_t size() { return v.size(); }

    friend ostream& operator<< (ostream& out, const adj& mat) {
        for (size_t i = 0; i < mat.v.size(); i++) {
            for (size_t j = 0; j < mat.v[i].size(); j++) {
                if (j!=0) cout << ' ';
                cout << mat.v[i][j];
            }
            cout << endl;
        }
        return out;
    }

    friend istream& operator>> (istream& in, adj& mat) {
        for (size_t i = 0; i < mat.v.size(); i++)
            for (size_t j = 0; j < mat.v[i].size(); j++) if (!(in >> mat.v[i][j])) throw error::INVALID_INPUT;
        if (in.fail()) throw error::INVALID_INPUT;
        if (adj::simple) mat.check();
        return in;
    }

    bool dfs (size_t b, const size_t& e, bool flag=1) const {
        if (flag) fets = vector <bool> (v.size(),false);
        if (b == e) return true;
        if (fets[b]) return false;
        fets[b]=true;
        for (size_t i = 0; i < v.size(); i++)
            if (v[b][i]!=0) if (dfs(i,e,0)) return true;
        return false;
    }

    int bfs (size_t b=0, const size_t& e=-1) const {
        vbfs.resize(fets.size(),-1);
        vbfs[b]=0;
        queue <T> q;
        q.push(b);
        do {
            b = q.top(); q.pop();
            if (b==e) return fets[b];
            for (size_t  i = 0; i < v.size(); i++) if (v[b][i] and vbfs[i]==-1) {
                q.push(v[b][i]);
                vbfs[i]=vbfs[b]+1;
            }
        } while (!q.empty());
        return -1;
    }

    adj kruskal () const { 
        priority_queue <edge,vector <edge>, comp > q; 
        for (size_t i = 0; i < v.size(); i++) for (int j = i+1; j < v.size(); j++) q.push(edge(v[i][j],i,j));
        adj <T> mat (v.size());
        size_t c = 0;
        while (!q.empty()) {
            edge ed = q.top(); q.pop();
            if (c == v.size()-1) return mat;
            if (ed.w != 0) 
                if (!mat.dfs(ed.a,ed.b)) {
                    mat[ed.a][ed.b] = ed.w;
                    if (simple) mat[ed.b][ed.a] = ed.w;
                    c++;
                }   
        }
        throw error::NOT_CONNECTED;
    }

    adj prim () const {
        priority_queue <edge,vector <edge>, comp> q; 
        for (size_t i = 1; i < v.size(); i++) q.push(edge(v[0][i],0,i));
        adj <T> mat (v.size());
        vector <bool> fets (v.size(),false);
        size_t c = 0;
        fets[0]=true;
        while (!q.empty()) {
            edge ed = q.top(); q.pop();
            if (c == v.size()-1) return mat;
            if (!fets[ed.b] and ed.w != 0) {
                mat[ed.a][ed.b] = ed.w;
                if (simple) mat[ed.b][ed.a] = ed.w;
                c++;
                fets[ed.b] = true;
                for (size_t i = 0; i < v.size(); i++) q.push(edge(v[ed.b][i],ed.b,i));
            }
        }
        throw error::NOT_CONNECTED;
    }

    T dijkstra (size_t b, const size_t& e) const {
        vector <T> d (v.size(), INF);
        d[b]=0;
        priority_queue <pair <T, size_t>, vector <pair <T, size_t> >, greater<pair<T,size_t> > > q;
        q.push(pair <T, size_t> (0,b));
        while (!q.empty()) {
            size_t pos = q.top().second; 
            if (q.top().first > d[pos]) {
                q.pop();
                continue;
            }
            if (pos == e) return d[pos];
            q.pop();
            for (size_t i = 0; i < v.size(); i++)
                if (v[pos][i]!=0 and d[i] > d[pos] + v[pos][i]) {
                    d[i] = d[pos] + v[pos][i];
                    q.push(pair <T, size_t> (d[i],i));
                }
        }
        throw error::NOT_CONNECTED;
    }

    T weight () const {
        T c = 0;
        for (int i = 0; i < v.size(); i++) 
            for (int j = 0; j < v[i].size(); j++) c += v[i][j];
        return (simple)? c/2 : c;
    }

    private:

    bool check () const {
        for (size_t i  = 0; i < v.size(); i++)
            for (size_t j = 0; j < v[i].size(); j++) if (v[i][j] != v[j][i]) throw error::NOT_SIMPLE_DIRECTED;
        for (size_t i = 0; i < v.size(); i++) if (v[i][i] != 0) throw error::NOT_SIMPLE_LOOP;
        return true;
    }
};

template <class T>
class adjlist {

    vector <bool> fets;

    public:

    vector <vector < pair <T,T> > > v;

    adjlist () {}

    adjlist (const adj<T>& a) {
        v.resize(a.v.size());
        for (int i=0; i < a.v.size(); i++) 
            for (int j=0; j < a.v[i].size(); j++)
                if (a.v[i][j]!=0) v[i].push_back(j);
    }

};

template <class T>
bool adj<T>::simple = true;

