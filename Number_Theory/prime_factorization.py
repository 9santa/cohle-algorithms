def prime_factorizatio(n):
    ans = []
    p = 2
    while p*p <= n:
        while n % p == 0:
            ans.append(p)
            n //= p
        p += 1

    if n > 1:
        ans.append(n)

    return ans
