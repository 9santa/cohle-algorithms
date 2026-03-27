#pragma once
#include "core.h"

namespace logic {

/*
    Boolean algebra utilities driven by a truth table:

    - Canonical SDNF / SKNF (a.k.a. SOP/POS)
    - Minimization via Quine–McCluskey:
        * build prime implicants
        * pick a minimum cover (essential primes + Petrick if <=64 primes, else greedy)
    - Karnaugh map ASCII dump (n = 2..4, visualization only)
    - Zhegalkin polynomial / ANF (Algebraic Normal Form) via Möbius transform

    Notes / limitations:
    - Quine–McCluskey is exponential in the worst case.
      In practice it's comfortable for ~10–12 variables depending on #minterms.
*/


// =====================================================================================
// Truth table
// =====================================================================================

/*
    TruthTable stores f(mask) for all assignments of n boolean variables.

    Convention:
    - mask is an integer in [0 .. 2^n - 1]
    - x1 corresponds to the most significant bit (MSB), xn to the least significant bit
      so "100..0" means x1=1, all others are 0.

    f[mask] is stored as 0/1.
*/

struct TruthTable {
    int n = 0;      // number of variables
    vector<u8> f;   // size = 2^nm each in {0,1}

    TruthTable() = default;
    explicit TruthTable(int n_vars) : n(n_vars), f((size_t)1 << n_vars, 0) {}

    size_t size() const { return f.size(); }

    u8 get(int mask) const { return f[(size_t)mask] & 1u; }
    void set(int mask, u8 v) { f[(size_t)mask] = (v & 1u); }

    // Constructor from a list of minterms (indices where f=1).
    static TruthTable from_minterms(int n_vars, const vector<int>& mins) {
        TruthTable tt(n_vars);
        for (auto m : mins) {
            if (0 <= m && m < (1 << n_vars)) tt.set(m, 1);
        }
        return tt;
    }

    vector<int> minterms() const {
        vector<int> ms;
        for (int i = 0; i < (int)size(); i++) if (get(i)) ms.push_back(i);
        return ms;
    }

    vector<int> maxterms() const {
        vector<int> zs;
        for (int i = 0; i < (int)size(); i++) if (!get(i)) zs.push_back(i);
        return zs;
    }
};

inline string var_name(int i) { return "x" + to_string(i + 1); }


// =====================================================================================
// Canonical SDNF / SKNF
// =====================================================================================

/*
    Canonical SDNF (SOP):
      f = OR_{m in minterms}  AND_{i=1..n} (xi or !xi)

    Canonical SKNF (POS):
      f = AND_{m in maxterms} OR_{i=1..n} (xi or !xi)

    For maxterms we use the standard rule:
      - if bit=0 -> xi
      - if bit=1 -> !xi
*/
inline string to_SDNF(const TruthTable& tt) {
    auto ms = tt.minterms();
    if (ms.empty()) return "0";

    ostringstream out;
    for (size_t k = 0; k < ms.size(); k++) {
        int m = ms[k];
        if (k) out << " | ";
        out << "(";
        for (int i = 0; i < tt.n; i++) {
            int bit = (m >> (tt.n - 1 - i)) & 1; // MSB-first: x1 is highest bit
            if (i) out << " & ";
            if (!bit) out << "!";
            out << var_name(i);
        }
        out << ")";
    }
    return out.str();
}

inline string to_SKNF(const TruthTable& tt) {
    auto zs = tt.maxterms();
    if (zs.empty()) return "0";

    ostringstream out;
    for (size_t k = 0; k < zs.size(); k++) {
        int m = zs[k];
        if (k) out << " & ";
        out << "(";
        for (int i = 0; i < tt.n; i++) {
            int bit = (m >> (tt.n - 1 - i)) & 1; // MSB-first: x1 is highest bit
            if (i) out << " | ";
            if (!bit) out << "!";
            out << var_name(i);
        }
        out << ")";
    }
    return out.str();
}

// =====================================================================================
// Quine–McCluskey. SDNF/SKNF Minimization.
// =====================================================================================

/*
    An implicant is represented by (value, mask):

    - mask bit = 1 means that variable is fixed (appears as a literal)
    - mask bit = 0 means "don't care" (variable is not used)

    value stores the fixed bit values.

    Coverage test:
      implicant covers minterm m if (m & mask) == (value & mask)
*/
struct Implicant {
    u32 value = 0;
    u32 mask = 0;

    bool covers(int m) const { return (u32(m) & mask) == (value & mask); }
    int literals() const { return popcnt(mask); } // literal count = number of fixes bits
};


// =====================================================================================
// Zhegalkin polynomial (ANF) via Boole-Möbius transform
// =====================================================================================

/*
    Zhegalkin polynomial / ANF (Algebraic Normal Form):

      f(x) = a0 ^ a1*x1 ^ a2*x2 ^ a12*x1*x2 ^ ...

    where ^ is XOR (addition mod 2) and multiplication is AND.

    To compute ANF coefficients, we apply a Boole-Möbius transform on the boolean cube.
    After transform:
      a[mask] is the coefficient of the monomial corresponding to mask.

    Complexity: O(n * 2^n)
*/

inline string zhegalkin_anf(const TruthTable& tt) {
    int n = tt.n;
    int N = 1 << n;

    vector<u8> a = tt.f; // will become coefficients

    // Boole-Möbius transform
    for (int i = 0; i < n; i++) {
        for (int mask = 0; mask < N; mask++) {
            if (mask & (1 << i)) a[mask] ^= a[mask ^ (1 << i)];
        }
    }

    ostringstream out;
    bool firstTerm = true;

    for (int mask = 0; mask < N; mask++) {
        if (!a[mask]) continue;

        if (!firstTerm) out << " ^ ";
        firstTerm = false;

        if (mask == 0) { out << "1"; continue; }

        bool firstVar = true;
        for (int i = 0; i < n; i++) {
            // MSB-first mapping so that terms print as x1..xn
            if (mask & (1 << (n - 1 - i))) {
                if (!firstVar) out << "*";
                firstVar = false;
                out << var_name(i);
            }
        }
    }

    return firstTerm ? "0" : out.str();
}

} // namespace logic
