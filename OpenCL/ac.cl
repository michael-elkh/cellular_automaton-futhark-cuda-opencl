// Recover direct neighborhood in vector of dim 4. The domain is periodic.
inline int4 get_neighborhood(int index, int width, int length){
	int4 neighbors;

	int col = index % width;
	
	//left
	neighbors.x = col == 0 ? index + (width - 1) : index - 1;
	//up
	neighbors.y = index < width ? length - (width - index) : index - width; 
	//right
	neighbors.z = col == (width - 1) ? index - (width - 1) : index + 1;
	//down
	neighbors.w = (index + width) >= length ? col /* idx mod width */ : index + width;

	return neighbors;
}

__kernel void parity_automaton(__global uint *src, __global uint *dst, int width, int length) {
  // Recover the index
  int index = get_global_id(0);

  // get direct neighbors of the pixel
  int4 neighbors = get_neighborhood(index, width, length);

  // update pixel in destination matrix
  dst[index] = src[neighbors.x] ^ src[neighbors.y] ^ src[neighbors.z] ^ src[neighbors.w];
}

// cyclic next state function
inline uint cyclic(uint center, uint left, uint up, uint right, uint down, uint max){
  uint k1 = (center + 1) % (max + 1);
  if (left == k1)
    return left;
  if (up == k1)
    return up;
  if (right == k1)
    return right;
  if (down == k1)
    return down;
  return center;
}

__kernel void cyclic_automaton(__global uint *src, __global uint *dst, int width, int length, uint max_val) {
  // Recover the index
  int index = get_global_id(0);

  // get direct neighbors of the pixel
  int4 neighbors = get_neighborhood(index, width, length);

  // set the pixel value in destination matrix
  dst[index] = cyclic(src[index], src[neighbors.x], src[neighbors.y], src[neighbors.z], src[neighbors.w], max_val);
}
