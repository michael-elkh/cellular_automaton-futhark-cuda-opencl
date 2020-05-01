// parity next state function
inline uint parity(uint left, uint up, uint right, uint down){
  return left ^ up ^ right ^ down;
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

__kernel void parity_automaton(__global uint *img, __global uint *src, uint2 size, uint max_val) {
  // compute the 1D index
  int i1d = get_global_id(0);

  // get direct neighbors of the pixel
  int4 neighbors = get_neighborhood(i1d, size.x, size.x * size.y);

  // update pixel in destination matrix
  img[i1d] = parity(src[neighbors.x], src[neighbors.y], src[neighbors.z], src[neighbors.w]);
}

__kernel void cyclic_automaton(__global uint *img, __global uint *src, uint2 size, uint max_val) {
  // compute the 1D index
  int i1d = get_global_id(0);

  // get direct neighbors of the pixel
  int4 neighbors = get_neighborhood(i1d, size.x, size.x * size.y);

  // set the pixel value in destination matrix
  img[i1d] = cyclic(src[i1d], src[neighbors.x], src[neighbors.y], src[neighbors.z], src[neighbors.w], max_val);
}
