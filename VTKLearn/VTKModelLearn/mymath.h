#ifndef MYMATH_H
#define MYMATH_H

#include<QDebug>
#include<math.h>
#define VTK_RIGHT 0
#define VTK_LEFT 1
#define VTK_MIDDLE 2

void Rotate(double angle,double x, double y, double z,double matrix[16]);
void MatrixMultiplyVector(double matrix[16],double in[3],double outvec[4]);

double RadiansFromDegrees( double x )
{
  return x * 0.017453292519943295;
}

inline void Multiply4x4(const double a[16], const double b[16],
                                      double c[16])
{
  double tmp[16];

  for (int i = 0; i < 16; i += 4)
  {
    for (int j = 0; j < 4; j++)
    {
      tmp[i + j] = a[i + 0] * b[j + 0] +
                   a[i + 1] * b[j + 4] +
                   a[i + 2] * b[j + 8] +
                   a[i + 3] * b[j + 12];
    }
  }

  for (int k = 0; k < 16; k++)
  {
    c[k] = tmp[k];
  }
}

void Translate(double x, double y, double z,double matrix[16])
{
  if (x == 0.0 && y == 0.0 && z == 0.0)
  {
    return;
  }

  double tempmatrix[16]={0};
  tempmatrix[0]=tempmatrix[5]=tempmatrix[10]=tempmatrix[15]=1;

  tempmatrix[3] = x;
  tempmatrix[7] = y;
  tempmatrix[11] = z;

  Multiply4x4(matrix,tempmatrix,matrix);
}

void Azimuth(double angle,double viewup[3],double camerapos[3],double focalpos[3],double matrix[16]){
    Translate(focalpos[0],focalpos[1],focalpos[2],matrix);
    double tempmatrix[16]={0};
    tempmatrix[0]=tempmatrix[5]=tempmatrix[10]=tempmatrix[15]=1;
    Rotate(angle,viewup[0],viewup[1],viewup[2],tempmatrix);
    Multiply4x4(matrix,tempmatrix,matrix);
    Translate(-focalpos[0],-focalpos[1],-focalpos[2],matrix);

    double newcamerapos[3];
    MatrixMultiplyVector(matrix,camerapos,newcamerapos);
    camerapos[0]=newcamerapos[0];
    camerapos[1]=newcamerapos[1];
    camerapos[2]=newcamerapos[2];
}

void Rotate(double angle,double x, double y, double z,double matrix[16]){
    if (angle == 0.0 || (x == 0.0 && y == 0.0 && z == 0.0))
    {
      return;
    }

    // convert to radians
    angle = RadiansFromDegrees( angle );

    // make a normalized quaternion
    double w = cos(0.5*angle);
    double f = sin(0.5*angle)/sqrt(x*x+y*y+z*z);
    x *= f;
    y *= f;
    z *= f;

    double ww = w*w;
    double wx = w*x;
    double wy = w*y;
    double wz = w*z;

    double xx = x*x;
    double yy = y*y;
    double zz = z*z;

    double xy = x*y;
    double xz = x*z;
    double yz = y*z;

    double s = ww - xx - yy - zz;

    matrix[0] = xx*2 + s;
    matrix[4] = (xy + wz)*2;
    matrix[8] = (xz - wy)*2;

    matrix[1] = (xy - wz)*2;
    matrix[5] = yy*2 + s;
    matrix[9] = (yz + wx)*2;

    matrix[2] = (xz + wy)*2;
    matrix[6] = (yz - wx)*2;
    matrix[10] = zz*2 + s;
}

inline void Cross(const double a[3], const double b[3], double c[3])
{
  double Cx = a[1] * b[2] - a[2] * b[1];
  double Cy = a[2] * b[0] - a[0] * b[2];
  double Cz = a[0] * b[1] - a[1] * b[0];
  c[0] = Cx; c[1] = Cy; c[2] = Cz;
}

void MatrixMultiplyVector(double matrix[16],double in[3],double outvec[4]){
    outvec[0] = matrix[0]*in[0] + matrix[1]*in[1] + matrix[2]*in[2] + matrix[3];
    outvec[1] = matrix[4]*in[0] + matrix[5]*in[1] + matrix[6]*in[2] + matrix[7];
    outvec[2] = matrix[8]*in[0] + matrix[9]*in[1] + matrix[10]*in[2] + matrix[11];
    outvec[3] = matrix[12]*in[0] + matrix[13]*in[1] + matrix[14]*in[2] + matrix[15];
}

int g_num1=0,g_num2=0;
char IntersectBox (double bounds[6], double origin[3], double dir[3],
                           double coord[3], double& t)
{
  bool    inside=true;
  char    quadrant[3];
  int     i, whichPlane=0;
  double  maxT[3], candidatePlane[3];
  //  First find closest planes
  for (i=0; i<3; i++)
  {
    if ( origin[i] < bounds[2*i] )
    {
      quadrant[i] = VTK_LEFT;
      candidatePlane[i] = bounds[2*i];
      inside = false;
    }
    else if ( origin[i] > bounds[2*i+1] )
    {
      quadrant[i] = VTK_RIGHT;
      candidatePlane[i] = bounds[2*i+1];
      inside = false;
    }
    else
    {
      quadrant[i] = VTK_MIDDLE;
    }
  }
  //  Check whether origin of ray is inside bbox
  if (inside)
  {
    coord[0] = origin[0];
    coord[1] = origin[1];
    coord[2] = origin[2];
    t = 0;
    return 1;
  }
  //  Calculate parametric distances to plane
  for (i=0; i<3; i++)
  {
    if ( quadrant[i] != VTK_MIDDLE && dir[i] != 0.0 )
    {
      maxT[i] = (candidatePlane[i]-origin[i]) / dir[i];
    }
    else
    {
      maxT[i] = -1.0;
    }
  }
  //  Find the largest parametric value of intersection
  for (i=0; i<3; i++)
  {
    if ( maxT[whichPlane] < maxT[i] )
    {
      whichPlane = i;
    }
  }
  //  Check for valid intersection along line
//  if ( maxT[whichPlane] > 1.0 || maxT[whichPlane] < 0.0 )
  if ( maxT[whichPlane] < 0.0 )
  {
      ++g_num1;
    return 0;
  }
  else
  {
    t = maxT[whichPlane];
  }

  for (i=0; i<3; i++)
  {
    if (whichPlane != i)
    {
      coord[i] = origin[i] + maxT[whichPlane]*dir[i];
      if ( coord[i] < bounds[2*i] || coord[i] > bounds[2*i+1] )
      {
          ++g_num2;
        return 0;
      }
    }
    else
    {
      coord[i] = candidatePlane[i];
    }
  }

  return 1;
}

double Determinant2x2(double a, double b, double c, double d) {
    return (a * d - b * c);
}

double Determinant3x3(double a1, double a2, double a3,
                                      double b1, double b2, double b3,
                                      double c1, double c2, double c3)
{
    return ( a1 * Determinant2x2( b2, b3, c2, c3 )
           - b1 * Determinant2x2( a2, a3, c2, c3 )
           + c1 * Determinant2x2( a2, a3, b2, b3 ) );
}

double Determinant(const double elem[16])
{
  double a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4;

  // assign to individual variable names to aid selecting
  //  correct elements

  a1 = elem[0];  b1 = elem[1];  c1 = elem[2];  d1 = elem[3];
  a2 = elem[4];  b2 = elem[5];  c2 = elem[6];  d2 = elem[7];
  a3 = elem[8];  b3 = elem[9];  c3 = elem[10]; d3 = elem[11];
  a4 = elem[12]; b4 = elem[13]; c4 = elem[14]; d4 = elem[15];

  return a1 * Determinant3x3(b2, b3, b4, c2, c3, c4, d2, d3, d4)
       - b1 * Determinant3x3(a2, a3, a4, c2, c3, c4, d2, d3, d4)
       + c1 * Determinant3x3(a2, a3, a4, b2, b3, b4, d2, d3, d4)
       - d1 * Determinant3x3(a2, a3, a4, b2, b3, b4, c2, c3, c4);
}

void Adjoint(const double elem[16], double outElem[16])
{
  double a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4;
  a1 = elem[0];  b1 = elem[1];  c1 = elem[2];  d1 = elem[3];
  a2 = elem[4];  b2 = elem[5];  c2 = elem[6];  d2 = elem[7];
  a3 = elem[8];  b3 = elem[9];  c3 = elem[10]; d3 = elem[11];
  a4 = elem[12]; b4 = elem[13]; c4 = elem[14]; d4 = elem[15];
  outElem[0]  =
    Determinant3x3(b2, b3, b4, c2, c3, c4, d2, d3, d4);
  outElem[4]  =
    - Determinant3x3(a2, a3, a4, c2, c3, c4, d2, d3, d4);
  outElem[8]  =
    Determinant3x3(a2, a3, a4, b2, b3, b4, d2, d3, d4);
  outElem[12]  =
    - Determinant3x3(a2, a3, a4, b2, b3, b4, c2, c3, c4);

  outElem[1]  =
    - Determinant3x3(b1, b3, b4, c1, c3, c4, d1, d3, d4);
  outElem[5]  =
    Determinant3x3(a1, a3, a4, c1, c3, c4, d1, d3, d4);
  outElem[9]  =
    - Determinant3x3(a1, a3, a4, b1, b3, b4, d1, d3, d4);
  outElem[13]  =
    Determinant3x3(a1, a3, a4, b1, b3, b4, c1, c3, c4);

  outElem[2]  =
    Determinant3x3(b1, b2, b4, c1, c2, c4, d1, d2, d4);
  outElem[6]  =
    - Determinant3x3(a1, a2, a4, c1, c2, c4, d1, d2, d4);
  outElem[10]  =
    Determinant3x3(a1, a2, a4, b1, b2, b4, d1, d2, d4);
  outElem[14]  =
    - Determinant3x3(a1, a2, a4, b1, b2, b4, c1, c2, c4);

  outElem[3]  =
    - Determinant3x3(b1, b2, b3, c1, c2, c3, d1, d2, d3);
  outElem[7]  =
    Determinant3x3(a1, a2, a3, c1, c2, c3, d1, d2, d3);
  outElem[11]  =
    - Determinant3x3(a1, a2, a3, b1, b2, b3, d1, d2, d3);
  outElem[15]  =
    Determinant3x3(a1, a2, a3, b1, b2, b3, c1, c2, c3);
}

void Invert(const double inElements[16],
                          double outElements[16])
{
  double det = Determinant(inElements);
  if (det == 0.0)
  {
    return;
  }

  Adjoint(inElements, outElements);

  for (int i = 0; i < 16; i++)
  {
    outElements[i] /= det;
  }
}
#endif // MYMATH_H
