
/** SplitMix64 pair hash for 64-bit integer pairs. Space: O(1). */
struct pair_hash {
  /** Mixes a 64-bit value. Time: O(1). */
  static u64 splitmix64(u64 x) {
    x += 0x9e3779b97f4a7c15;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
    x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
    return x ^ (x >> 31);
  }

  /** Hashes a pair of 64-bit values. Time: O(1). */
  size_t operator()(const pair<u64,u64>& p) const {
    static const u64 R = chrono::steady_clock::now().time_since_epoch().count();
    u64 a = splitmix64(p.first + R);
    u64 b = splitmix64(p.second + (R ^ 0x9e3779b97f4a7c15ULL));
    return a ^ (b + 0x9e3779b97f4a7c15ULL + (a<<6) + (a>>2)); // combine
  }
};
