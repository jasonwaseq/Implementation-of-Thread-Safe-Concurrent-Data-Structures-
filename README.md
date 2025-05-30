# Implementation-of-Thread-Safe-Concurrent-Data-Structures-

## Description:
In this assignment, I implemented a bounded buffer (thread-safe FIFO queue) and a flexible reader-writer lock in C, leveraging POSIX threads and synchronization primitives to coordinate multiple producers, consumers, and readers/writers without busy-waiting. The bounded buffer supports blocking push and pop operations, guaranteeing FIFO ordering and data integrity under concurrent access, while the reader-writer lock offers configurable priority policies (READERS, WRITERS, or N-WAY) to prevent starvation and enforce N-way blocking semantics. Rigorous testing via provided autograder scripts, adherence to a clang-format style guide, and compilation with -Wall -Wextra -Werror -pedantic flags ensured a robust, deadlock-free implementation suitable for real-world systems design. 


