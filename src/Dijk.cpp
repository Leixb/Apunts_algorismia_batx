#include <iostream>
#include <queue>
#include <vector>
#include <new>
using namespace std;

enum err {BAD_INPUT=0xf, NOT_SIMPLE};

class vertex;

class node {
    public:
        int w;
        vertex* v, *prev;
        node (int _w=0, vertex* _v=NULL, vertex* _prev=NULL) : w(_w), v(_v),prev(_prev) {}
        inline friend bool operator> (const node& a, const node& b) { return a.w > b.w; }
        node operator+ (int n) {return node(w+n,v,prev);}
};

class vertex {
    vertex* prev;
    int d;
    int id;
    public:
    static int n;
    vector <node> v;
    vertex () : prev(NULL), d(-1), id(n++) {}

    friend int dijkstra (vertex *a, const vertex *b);
    friend void undopath(vertex *p);
};

int vertex::n=0;

int dijkstra (vertex* a, const vertex* b) {
    priority_queue <node, vector <node>, greater <node> > q;
    q.push(node(0,a));
    while (!q.empty()) {
        node n = q.top(); q.pop();
        if (n.v->d!=-1) continue;
        n.v->prev=n.prev;
        n.v->d=n.w;
        if (n.v==b) return n.w;
        for (auto i : n.v->v) q.push(i+n.w);
    }
    return -1;
}

void undopath (vertex *p) {
    if (p->prev==NULL) cout << p->id;
    else {
        undopath(p->prev);
        cout << '-' << p->id;
    }
}

int main () {
    int n, e;
    cin >> n >> e;
    vertex *V = new vertex[n];
    while (e--) {
        int a,b,w;
        cin >> a >> b >> w;
        (V+a)->v.push_back(node(w,V+b,V+a));
        (V+b)->v.push_back(node(w,V+a,V+b));
    }
    int a,b;
    cin >> a >> b;
    cout << dijkstra(V+a,V+b) << endl;
    undopath(V+b);
}

