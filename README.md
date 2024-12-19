# TwinArray.h
A c++20 data structure in two halves, a derivitive of a
[gapbuffer](https://en.wikipedia.org/wiki/Gap_buffer)

### Justification
During development of my text editor project, I wanted to build a core data 
structure that was more than just a `vec<char>`. My first approach was to 
write a gapbuffer, which added a significant amount of complexity and was 
raising more and more memory errors. To counter this, I wanted to bring it back
to something simpler.

My goals with this data structure was to build something that could keep track
of a "head" in the middle of the buffer, representing the cursor in the middle 
of the text for `O(1)` insertion and deletion at this point. I also wanted fast
searching (`O(n)`) and didn't care about ordering or reordering.

This data structure instead splits a vector in half, with two buffers of the 
same size, and treating the total capacity as the size of one half. Values are
moved from one side to the other as the cursor/head is moved around, and when
the total size of both halves are equal to the capacity, we treat the buffer as
full. This prevents us from having too many values to store in one half -- 
imagine the cursor at the very start of the text, every value should be stored
in the rhs sub-buffer inside this structure.

### How to use
