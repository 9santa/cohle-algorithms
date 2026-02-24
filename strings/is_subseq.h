
template<typename STRING>
bool is_subseq(const STRING& S, const STRING& T) {
    int p = 0;
    for (const auto&& c : S) {
        while (p < sz(S) && T[p] != c) p++;
        if (p == sz(T)) return false;
        p++;
    }
    return true;
}
