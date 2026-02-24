#include <bits/stdc++.h>

using namespace std;

void scanline()
{
    int n, m; cin >> n >> m;
    vector<int> starts;
    vector<int> ends;

    while(n--) {
        int start, end; cin >> start >> end;
        starts.push_back(min(start, end));
        ends.push_back(max(end, start));
    }

    sort(starts.begin(), starts.end());
    sort(ends.begin(), ends.end());

    vector<int> points(m);
    for(int i = 0; i < m; i++) {
        int point; cin >> point;
        points[i] = point;
    }

    vector<int> result;

    for(int x : points) {
        int started = upper_bound(starts.begin(), starts.end(), x) - starts.begin();
        int ended = lower_bound(ends.begin(), ends.end(), x) - ends.begin();

        result.push_back(started - ended);
    }
	
    for(int el : result) cout << el << " ";

}
