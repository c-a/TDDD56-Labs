/*
 * Placeholder for wavelet transform.
 * Currently just a simple invert.
 */

__kernel void kernelmain(__global unsigned char *image, __global unsigned char *data, const unsigned int length)
{
  data[get_global_id(0)] = 255 - image[get_global_id(0)];
}
