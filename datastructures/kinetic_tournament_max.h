#include "../header.h"

struct Line {
    double a = 0.0, b = 0.0; // f(t) = a*t + b
    double eval(double t) const { return a * t + b;}
};

struct Event {
    double time;
    int node;
    int version;
    bool operator>(const Event& rhs) const {
        if (time != rhs.time) return time > rhs.time;
        return node > rhs.node;
    }
};

struct KineticTournamentMax {
private:
    int n;
    int base;                   // first leaf index in segment-tree layout
    double now = 0.0;           // current processed time (monotone non-decreasing)
    vector<Line> f;             // trajectories (size n)
    vector<int> winner;         // winner[node] = index of winning line (or -1)
    vector<double> fail_time;   // fail_time[node] = certificate failure time
    vector<int> ver;            // version counter per node

    priority_queue<Event, vector<Event>, std::greater<Event>> pq;

    static constexpr double INF = numeric_limits<double>::infinity();
    static constexpr double EPS = 1e-12;

    void build() {
        base = 1;
        while (base < n) base <<= 1;
        int N = 2 * base;
        winner.assign(N, -1);
        fail_time.assign(N, INF);
        ver.assign(N, 0);
        // init leaves
        for (int i = 0; i < n; i++) winner[base+i] = i;
        // build internal nodes bottom-up
        for (int i = base-1; i >= 1; i--) pull(i);
    }

    bool better(int a, int b, double t) const {
        // true if a >= b at time t, under tie-break by smaller index
        if (a < 0) return false;
        if (b < 0) return true;
        double va = f[a].eval(t);
        double vb = f[b].eval(t);
        if (fabs(va - vb) > 1e-9) return va > vb; // EPS for double presicion mistakes
        return a < b;
    }

    double next_failure_time(int w, int l, double tnow) const {
        // w is the current winner, l is the current loser; compute earliest t > tnow when l overtakes w
        if (w < 0 || l < 0) return INF;
        double aw = f[w].a, bw = f[w].b;
        double al = f[l].a, bl = f[l].b;

        // If slopes are equal, overtake never happens
        if (fabs(aw - al) < 1e-15) return INF;

        // Crossing time: solve aw*t + bw = al*t + bl
        double t_cross = (bl - bw) / (aw - al);
        if (!(t_cross > tnow + EPS)) return INF; // must be in the future

        // For linear functions: after crossing, the larger slope wins
        if (al > aw) return t_cross;
        return INF;
    }

    void schedule(int v, double t_fail) {
        fail_time[v] = t_fail;
        ver[v]++;
        if (t_fail < INF) {
            pq.push(Event{t_fail, v, ver[v]});
        }
    }

    void pull(int v) {
        int L = v<<1, R = v<<1|1;
        int wl = winner[L];
        int wr = winner[R];

        if (wl < 0 && wr < 0) {
            winner[v] = -1;
            schedule(v, INF);
            return;
        }

        int w = better(wl, wr, now) ? wl : wr;
        int l = (w == wl ? wr : wl);
        winner[v] = w;

        // certificate: w >= l until fail time
        double t_fail = next_failure_time(w, l, now);
        schedule(v, t_fail);
    }

    void advance_to(double t_new) {
        if (t_new < now) return;

        while (!pq.empty()) {
            Event e = pq.top();
            if (e.time > t_new) break;
            pq.pop();

            if (e.node <= 0 || e.node >= (int)fail_time.size()) continue;
            if (e.version != ver[e.node]) continue;
            if (fabs(e.time - fail_time[e.node]) > 1e-9) continue;

            // Move time to just after the event (when certificate just failed)
            now = nextafter(e.time, INF);

            // Recompute this node and all its ancestors, because winners may change above aswell
            int v = e.node;
            for (; v >= 1; v >>= 1) {
                pull(v);
                if (v == 1) break;
            }
        }

        // Finally set 'now' to t_new
        now = t_new;
    }

public:
    explicit KineticTournamentMax(vector<Line> lines) : n((int)lines.size()), f(std::move(lines)) {
        build();
    }

    // Return index of maximum at time t (0-indexed)
    int argmax(double t) {
        advance_to(t);
        return winner[1];
    }

    // Retutn maximum value at time t
    double max_value(double t) {
        int idx = argmax(t);
        if (idx < 0) return -INF;
        return f[idx].eval(t);
    }

    // Update one trajectory at current processed time
    void set_line(int idx, Line L) {
        if (idx < 0 || idx >= n) return;
        f[idx] = L;
        int leaf = base + idx;
        // Recompute along the path to root at current time 'now'
        for (int v = leaf >> 1; v >= 1; v >>= 1) {
            pull(v);
            if (v == 1) break;
        }
    }
};

// Example usage
signed main() {
    vector<Line> lines = {
    {1.0, 0.0},
    {0.0, 5.0},
    {2.0, -3.0},
    {-1.0, 8.0}
    };

    KineticTournamentMax kinetic(lines);

    vector<double> queries = {0.0, 1.0, 2.0, 3.0, 10.0};
    for (auto t : queries) {
        int idx = kinetic.argmax(t);
        double val = kinetic.max_value(t);
        cout << "t=" << t << "  argmax=" << idx << "  max=" << val << "\n";
    }

    kinetic.set_line(1, Line{3.0, 0.0});
    queries.push_back(12.0);
    cout << "\nadded line\n";
    auto t = queries.back();
    int idx = kinetic.argmax(t);
    double val = kinetic.max_value(t);
    cout << "t=" << t << "  argmax=" << idx << "  max=" << val << "\n";

    return 0;
}
