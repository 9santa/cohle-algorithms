
/*
Если p — простое и n < p (тогда у всех чисел 1..n существуют обратные по модулю
p), все обратные можно посчитать за O(n + log p) так: один раз делаем быстрое
возведение в степень (по Ферма), потом один проход назад.

Идея:
Считаем префиксные произведения:
pref[i] = 1 * 2 * ... * i (mod p), то есть pref[i] = i! mod p.

Тогда обратный к i выражается так:
inv(i) = (i!)^{-1} * (i-1)! (mod p).
То есть inv[i] = inv_factorial(i) * pref[i-1] (mod p), где inv_factorial(i) =
(i!)^{-1}.

Чтобы получить inv_factorial(n) = (n!)^{-1}, используем Ферма:
(n!)^{-1} = (n!)^(p-2) (mod p).
Это делается за O(log p).

Дальше идём i = n..1 (вниз) и поддерживаем x = (i!)^{-1}:

inv[i] = x * pref[i-1] % p

затем обновляем x для следующего шага (к (i-1)!):
x = x * i % p
потому что если x = (i!)^{-1}, то x * i = (i!)^{-1} * i = ((i-1)!)^{-1}.

Алгоритм:

pref[0] = 1

для i = 1..n:
pref[i] = pref[i-1] * i % p

x = pow(pref[n], p-2, p) // x = (n!)^{-1} mod p

для i = n..1:
inv[i] = x * pref[i-1] % p
x = x * i % p

Итоговая сложность: O(n) на префиксы + O(log p) на одну степень + O(n) на
обратный проход = O(n + log p).
*/

#include "../header.h"
#include "free_mod.h"

static vl all_inverses(int n, ll p) {
    assert(n < p);
    vl pref(n+1), inv(n+1, 0);
    for (int i = 1; i <= n; i++) pref[i] = (i128)pref[i-1] * i % p;

    ll x = modpow(pref[n], p-2, p); // (n!)^{-1}

    for (int i = n; i >= 1; i--) {
        inv[i] = (i128)x * pref[i-1] % p; // i^{-1}
        x = (i128)x * i % p;              // ((i-1)!)^{-1}
    }
    return inv; // inv[1..n]
}
