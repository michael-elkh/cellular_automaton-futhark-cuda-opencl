inline uint index1D(uint2 i2d, uint2 size){
  return (uint)(i2d.y * size.x + i2d.x);
}

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
inline uint4 get_neighborhood(__global uint *src, uint2 i2d, uint2 size){
  uint left = (i2d.x + size.x - 1) % size.x;
  left = src[index1D((uint2)(left, i2d.y), size)];

  uint up = (i2d.y + size.y - 1) % size.y;
  up = src[index1D((uint2)(i2d.x, up), size)];

  uint right = (i2d.x + 1) % size.x;
  right = src[index1D((uint2)(right, i2d.y), size)];

  uint down = (i2d.y + 1) % size.y;
  down = src[index1D((uint2)(i2d.x, down), size)];

  return (uint4)(left, up, right, down);
}

__kernel void parity_automaton(__global uint *img, __global uint *src, uint2 size, uint max_val) {
  // get the index of the current element to be processed
  uint2 i2d = (uint2)(get_global_id(0), get_global_id(1));  

  // compute the 1D index
  uint i1d = index1D(i2d, size);

  // get direct neighbors of the pixel
  uint4 neighbors = get_neighborhood(src, i2d, size);

  // update pixel in destination matrix
  img[i1d] = parity(neighbors.x, neighbors.y, neighbors.z, neighbors.w);
}

__kernel void cyclic_automaton(__global uint *img, __global uint *src, uint2 size, uint max_val) {
  // get the index of the current element to be processed
  uint2 i2d = (uint2)(get_global_id(0), get_global_id(1));  

  // compute the 1D index
  uint i1d = index1D(i2d, size);

  // get direct neighbors of the pixel
  uint4 neighbors = get_neighborhood(src, i2d, size);

  // set the pixel value in destination matrix
  img[i1d] = cyclic(src[i1d], neighbors.x, neighbors.y, neighbors.z, neighbors.w, max_val);
}
