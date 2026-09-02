# cohle-algorithms

Personal competitive-programming snippets. The active tree is optimized for **copy/paste use during contests**, not for production-library dependency minimization.

## Conventions

- Active C++ snippets are self-contained paste units whenever practical.
- A normal snippet starts with:

  ```cpp
  #pragma once
  #include <bits/stdc++.h>
  using namespace std;
  ```

- There are no shared topic `core.h` headers. If a snippet needs a small helper, alias, or implementation detail, it carries it locally.
- Some duplication is intentional. For example, `string/kmp.h` contains the prefix-function implementation it needs even though `string/prefix_function.h` also exists.
- Directory structure describes **what the snippet is**, not its include graph.
- Data-structure families with multiple implementations keep a second directory level: `ds/segtree/`, `ds/treap/`, `ds/fenwick/`, `ds/rmq/`, etc.
- `algebra/monoid/` and `algebra/acted_monoid/` are the deliberate exception to the no-local-includes rule. They are reusable algebraic components, and acted monoids compose monoids explicitly.
- `template.cpp` is a convenient contest starting point. Active snippets do not depend on it.
- Old language versions, superseded implementations, and incomplete experiments live under `legacy/` rather than in the active browsing path.
- Graph and tree snippets use ordinary contest representations rather than repository-wide wrappers:

  ```cpp
  vector<vector<int>> g(n);                    // unweighted graph / tree
  vector<vector<pair<int, long long>>> g(n);   // (to, weight)
  vector<pair<int, int>> edges;                // when stable edge ids matter
  ```

  Algorithms compute `parent`, `depth`, traversal order, etc. locally when they need them. The old CSR `Graph`, `RootedTree`, and shared `FlowGraph` helpers are archived under `legacy/graph/` and `legacy/tree/`. Active flow snippets own their residual representation directly, just like the other graph algorithms.

Flow follows the same rule. There is no active shared residual-graph base class: `EdmondsKarp<Cap>` uses the usual adjacency-list + capacity-matrix representation, while `FordFulkerson<Cap>` keeps its reverse-edge residual structure inside the algorithm itself. Typical use is simply `flow.add_edge(u, v, cap); flow.max_flow(s, t);`.

## Layout

```text
algebra/
  monoid/
  acted_monoid/
bitmask/
dp/
ds/
  dsu/
  fenwick/
  heap/
  link_cut/
  rmq/
  segtree/
  sparsetable/
  splay/
  treap/
geometry/
graph/
  flow/
linear_algebra/
math/
misc/
number_theory/
polynomial/
  extra/
random/
string/
tree/
  dp/
  hld/
tools/
legacy/
template.cpp
```

A family directory is useful when there are multiple implementations or closely related variants. A one-off concept stays directly in its topic directory.

## Validation

Run the fast dependency/convention check:

```bash
python3 tools/check_snippets.py
```

For the full independent C++20 syntax pass:

```bash
python3 tools/check_snippets.py --compile
```

Files under `legacy/` are intentionally excluded.

`tools/expand_include.py` is still useful when you want to turn an intentionally compositional snippet (primarily an acted monoid) into one physical paste block.
