# Flapmapp_Ass1
Android Assignment Set 1

Q1- 
This LRU Cache uses a combination of a doubly linked list and unordered map to acheive O(1) time complexity for get and put operations. The doubly linked list maintains the order of usage with the most recently used items near the head of the list and the least used items near the tail. The unordered map allows for keys to access nodes in constant time.

Logic:

- On get(key), check for the existence of the value in the map.
- Return -1 if it does not exist.
- If the key does exists...
- We remove the node from its current position in the list.
- Move the node to the front of the list (most recently used position)
- Return the value of the node.
- On put(key, value), check whether or not the key already exists.
- If true, remove the existing node from the list and the map.
- If the cache is at its limit, remove the least recently used node (tail - 1) and remove its key from the map.
- Now create a new node with the provided key and value.
- Insert the new node right after the head - this makes it the most recently used node.
- Finally, add the new node to the map.


-------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Q2-
Core Idea:

Create a basic HashMap by using an array of linked lists (chaining) to resolve collisions. Each bucket contains a linked list of nodes with keys and values.

Logic:

- Maintain a fixed-size array buckets of length 1000 for storing and retrieving the keys. (i.e., for hashing)
- Hash function: index = key % size of the array
- Have a linked list for each bucket having a head and tail dummy node to facilitate easy node insertion and deletion.
- On put(key, val):
- Calculate the index of the bucket.
- If bucket is uninitialized, create it with a head and tail dummy node.
- Look for the corresponding linked list head node latch to mark the boundary.
- Check if the node in the last bucket is the required one.
- If node has the value, return it else return -1.
- If key saturates list return the dummy node and search the last node for node with key super capped by closest greater than hard verife hudeement.

- On get(key):
-- Calculate bucket index.
-- If bucket empty return head node.
-- Look for the corresponding linked list that corresponds to the last node latch for head.
-- If in the last bucket head not has available due value edge return head node, else return -1.

- On remove(key):
-- Calculate bucket index.
-- If head node bucket unlinked as a cap, do nothing.
-- Search linked list for head lut for last bucket.
-- If latching to second node does return the dummy which enables mites terminate, de terror graph to clear next check.

- helper function:
--Iterate the edge starting on the head node corresponding rotor from cap lock accessing to enable can travers ls side cap veridge cur by Bounding Twoaregh the list key CPU ponstr write capped rot lock node lst lie that val.


-------------------------------------------------------------------------------------------------------------------------------------------------------------------------


Q4- Mini Solar System Visualization - Task Summary
============================================

1. Render a simple solar system scene:
   - A static Sun is rendered at the center using a yellow circle.
   - Two planets orbit the Sun at different speeds and distances, animated using time-based transformations.
   - One planet has a moon that orbits it, also animated.

2. Implement custom shaders:
   - All shaders are written in GLSL and embedded in the code.
   - The Sun uses a custom fragment shader for a glow and pulsing effect based on time.
   - Planets and moon use a fragment shader with a radial gradient for simple visual variety.

3. Apply transformations:
   - Matrix transformations (scale, rotation, translation) are used for all objects.
   - Planets and moon orbit using translation, and rotate on their own axes using rotation matrices.

4. User interaction:
   - Camera controls allow rotating the view with mouse drag (left button).
   - Zoom in/out is supported with the mouse scroll wheel.
   - All transformations are applied via a view matrix.

5. Performance:
   - The application uses a single VAO and VBO for all circle geometry.
   - Rendering is efficient, with no per-frame geometry uploads.
   - The application runs smoothly and meets the 30 FPS or higher requirement.