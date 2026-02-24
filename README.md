# Competitive Programming Library Notes

## Should everything be `.h` instead of `.cpp`?

Short answer: **no**.

Use this split:

- **`.h`** for reusable components that are meant to be `#include`d (templates, inline utilities, small structs/classes).
- **`.cpp`** for standalone examples, snippets with `main()`, benchmarks, or one-off implementations.

In a CP library, many files are template-heavy and naturally belong in headers. But renaming all `.cpp` files to `.h` usually makes organization worse, not better.

## Include strategy for a shared `header.h`

If you keep a common `header.h` with aliases/macros, it is fine to include it from multiple files **as long as it is idempotent**:

```cpp
#pragma once
```

or classic include guards:

```cpp
#ifndef CP_HEADER_H
#define CP_HEADER_H
// ...
#endif
```

Then accidental double-includes are safe.

## Practical structure recommendation

1. Keep `header.h` only for truly global CP conveniences.
2. Keep each library header self-contained where possible (include what it uses).
3. Keep standalone runnable snippets as `.cpp`.
4. For editor sanity (Neovim/LSP), prefer reducing macro clutter in global headers.

This keeps your repo easy to navigate while still being contest-friendly.
