List data structure (template class)
===

Interface
---

### Constructors

- Default constructor. Creates an empty list.
- Count constructor.
- Copy constructor.
- Move constructor.
- Initializer list constructor.

### Operators

- Copy assignment operator.
- Move assignment operator.
- Assignment opreator from initializer list.

### Element access

- front
- back

### Capacity

- empty. Checks whether the list is empty.
- size. Returns number of elements.

### Iterators

- begin
- end
- cbegin
- cend
- rbegin
- rend
- crbegin
- crend

### Modifiers

- push_back / push_front. Adds element to back / front of the list.
- pop_back / push_front. Removes element from back / front of the list.
- clear. Clears the content.
- swap. Swaps the contents
- insert. Inserts the lements.
- erase. Erases the elements.

### Operations

- splice. Moves elements from another list.
