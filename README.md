# TwinArray.h
A c++20 data structure in two halves, a derivitive of a
[gapbuffer](https://en.wikipedia.org/wiki/Gap_buffer)

### Justification
I wanted a data structure with `O(1)` insertions and deletions at any point in
the structure tracked as a "head" point. The easiest way to achieve this is
to split a contiguous buffer into two halves. In essence, this data structure
is just two arrays in one.

### How to use
