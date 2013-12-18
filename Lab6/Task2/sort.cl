/*
 * Rank sorting in sorting OpenCL
 * This kernel has a bug. What?
 */

__kernel void sort(__global unsigned int *in,
                   __global unsigned int *out,
                   const unsigned int length)
{ 
  __local unsigned int S[512];
  unsigned int pos = 0;
  unsigned int group_base, i;
  unsigned int val;

  val = in[get_global_id(0)];
  group_base = 0;
  for (i = 0; i < get_num_groups(0); i++, group_base += get_local_size(0)) {
    unsigned int in_index, local_size, j;

    in_index = group_base + get_local_id(0);
    if (in_index < length)
      S[get_local_id(0)] = in[in_index];

    barrier(CLK_LOCAL_MEM_FENCE);

    //find out how many values are smaller
    local_size = min(length-group_base, get_local_size(0));
    for (j = 0; j < local_size; j++)
      if (val > S[j])
        pos++;

    barrier(CLK_LOCAL_MEM_FENCE);
  }

  out[pos]=val;
}
