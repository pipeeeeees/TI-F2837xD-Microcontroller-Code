/*
 * examples.h
 *
 *  Created on: Jul 14, 2020
 *      Author: David Taylor
 */

#ifndef EXAMPLES_H_
#define EXAMPLES_H_

#if (choice == 1)
#define N 3
float32 a[N][N+1] =
{
 {  3,  2, -1, 10  },
 { -1,  3,  2,  5  },
 {  1, -1, -1, -1  }
};
float32 x[N] = { -2,  5, -6  };
#endif

#if (choice == 2)
#define N 3
float32 a[N][N+1] =
{
 {  1,  -1,   2,   8  },
 {  0,   0,  -1, -11  },
 {  0,   2,  -1,  -3  }
};
float32 x[N] = { -10,  4,  11  };
#endif

#if (choice == 3)
#define N 2
float32 a[N][N+1] =
{
 {  0.003,  59.140,  59.170  },
 {  5.291,  -6.130,  46.780  }
};
float32 x[N] = {  10,  1  };
#endif

#if (choice == 4)
#define N 6
float32 a[N][N+1] =
{
 {  1,  1,  2, -4, -5,  4,  0  },
 {  0,  1,  3,  5,  3, -2,  1  },
 {  3,  5, -3,  3,  5, -5,  2  },
 {  2, -4, -3,  1,  0, -1, -5  },
 {  5, -2,  5, -2,  2, -3, -4  },
 {  2, -2,  5,  3,  1, -1,  2  }
};
float32 x[N] =
{ -62.90,  12.06,  17.56,  11.46, -180.52, -210.26  };
#endif

#if (choice == 5)
#define N 4
float32 a[N][N+1] =
{
 {  1,  1,  1,  0,  1  },
 {  1,  1,  1,  1,  0  },
 {  1,  0,  1,  1,  1  },
 {  0,  0,  0,  1,  0  },
};
float32 x[N];
#endif

#endif
