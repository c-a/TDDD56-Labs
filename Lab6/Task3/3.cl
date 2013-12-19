/*
 * Placeholder for wavelet transform.
 * Currently just a simple invert.
 */

__kernel void kernelmain(__global unsigned char *image, __global unsigned char *data, const unsigned int WH)
{
      int i1r, i1g, i1b, i2r, i2g, i2b, i3r, i3g, i3b, i4r, i4g, i4b;

      unsigned int i = get_global_id(1), j = get_global_id(0);
      unsigned int i_2 = 2*get_global_id(1), j_2 = 2*get_global_id(0);

      i1r = image[(i_2*WH+j_2)*3+0];
      i1g = image[(i_2*WH+j_2)*3+1];
      i1b = image[(i_2*WH+j_2)*3+2];

      i2r = image[(i_2*WH+(j_2+1))*3+0];
      i2g = image[(i_2*WH+(j_2+1))*3+1];
      i2b = image[(i_2*WH+(j_2+1))*3+2];

      i3r = image[((i_2+1)*WH+j_2)*3+0];
      i3g = image[((i_2+1)*WH+j_2)*3+1];
      i3b = image[((i_2+1)*WH+j_2)*3+2];

      i4r = image[((i_2+1)*WH+(j_2+1))*3+0];
      i4g = image[((i_2+1)*WH+(j_2+1))*3+1];
      i4b = image[((i_2+1)*WH+(j_2+1))*3+2];

      /* Out 1 */
      data[(i*WH + j)*3+0] = (i1r+i2r+i3r+i4r) / 4;
      data[(i*WH + j)*3+1] = (i1g+i2g+i3g+i4g) / 4;
      data[(i*WH + j)*3+2] = (i1b+i2b+i3b+i4b) / 4;

      /* Out 2 */
      data[(i*WH + WH/2 + j)*3+0] = (i1r+i2r-i3r-i4r) / 4 + 128;
      data[(i*WH + WH/2 + j)*3+1] = (i1g+i2g-i3g-i4g) / 4 + 128;
      data[(i*WH + WH/2 + j)*3+2] = (i1b+i2b-i3b-i4b) / 4 + 128;

      /* Out 3 */
      data[((WH/2+i)*WH + j)*3+0] = (i1r-i2r+i3r-i4r) / 4 + 128;
      data[((WH/2+i)*WH + j)*3+1] = (i1g-i2g+i3g-i4g) / 4 + 128;
      data[((WH/2+i)*WH + j)*3+2] = (i1b-i2b+i3b-i4b) / 4 + 128;

      /* Out 4 */
      data[((WH/2+i)*WH + WH/2 + j)*3+0] = (i1r-i2r-i3r+i4r) / 4 + 128;
      data[((WH/2+i)*WH + WH/2 + j)*3+1] = (i1g-i2g-i3g+i4g) / 4 + 128;
      data[((WH/2+i)*WH + WH/2 + j)*3+2] = (i1b-i2b-i3b+i4b) / 4 + 128;
}
