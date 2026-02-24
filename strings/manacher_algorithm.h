#include "../header.h"

// Manacher: O(n)
// odd[i] = # of chars in radius for odd palindrome centered at i (includes center)
//          palindrome length = 2 * odd[i] - 1
// even[i] = # of chars in radius for even palindrome centered at i-1 and i
//          palindrome length = 2 * even[i]
pair<vector<int>, vector<int>> manacher(const string& s) {
    int n = sz(s);
    vi even(n), odd(n);

    // odd-length pals
    for (int i = 0, l = 0, r = -1; i < n; i++) {
        int k;
        if (i > r) k = 1;
        else {
            int mirror = l + r - i; // mirror: 2*j - i
            k = min(odd[mirror], r - i + 1);
        }
        while (i - k >= 0 && i + k < n && s[i-k] == s[i+k]) k++;
        odd[i] = k;
        if (i + k - 1 > r) {
            l = i - k + 1;
            r = i + k - 1;
        }
    }

    // even-length pals
    for (int i = 0, l = 0, r = -1; i < n; i++) {
        int k;
        if (i > r) k = 0;
        else {
            int mirror = l + r - i + 1;
            k = min(even[mirror], r - i + 1);
        }
        while (i - k - 1 >= 0 && i + k < n && s[i-k-1] == s[i+k]) k++;
        even[i] = k;
        if (i + k - 1 > r) {
            l = i - k;
            r = i + k - 1;
        }
    }

    return {odd, even};
}
