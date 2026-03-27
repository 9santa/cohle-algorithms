# cohle-algorithms

Header-heavy competitive programming and algorithms library, organized by topic.

Main directories:
- `alg/`: algebraic interfaces and reusable monoid / acted-monoid building blocks.
- `datastructures/`: contest-oriented and reusable data structures.
- `graphs/`: graph algorithms, plus `graphs/flow/` for max-flow related code.
- `trees/`: tree algorithms and tree DP utilities, with `trees/hld/` for HLD variants and `trees/dp/` for tree-DP patterns.
- `dynamicprogramming/`: general DP techniques and sequence / interval / digit / optimization DP.
- `geometry/`: geometric primitives and polygon / segment helpers.
- `numbertheory/`, `polynomials/`, `linalg/`, `strings/`, `bitmask/`, `math/`, `misc/`, `random/`: topic-specific helpers.
- `legacy/`: older superseded implementations kept for reference.
- `benchmarks/`: performance experiments and comparisons.

Conventions:
- Prefer reusable header-only code.
- Demo-only programs should not live in reusable headers.
- Legacy implementations belong under `legacy/`, not mixed with current files.
