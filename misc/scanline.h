
// Finds the min/max value in every window of size k
void scanline() {
    int n, k; cin >> n >> k;
    vector<int> a(n);
    vector<int> ans;

    for(int& el : a) cin >> el;

    deque<int> dq;
    for(int i = 0; i < n; i++) {
        if(!dq.empty() && dq.front() <= i-k) {
            dq.pop_front();
        }

        while(!dq.empty() && a[dq.back()] >= a[i]) {
            dq.pop_back();
        }

        dq.push_back(i);

        if(i >= k-1) {
            ans.push_back(a[dq.front()]);
        }
    }

    for(int el : ans) {
        cout << el << "\n";
    }

}
