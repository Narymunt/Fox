// Fox v0.5a
// by Jaroslaw Rozynski
//===
// TODO:
// - zmienna wielkosc macierzy
// - zmienne typy macierzy
// - w jaki sposob zapisac te macierz ? jako a[3][3] ?
// - poprawic wszystko




// mnozenie macierzy przez macierz
// wynik jest umieszczany w macierzy a

void FOX_matrix_mull(int* matrix_a[][], int* matrix_b[][])
{

	int matrix_tmp[3][3];

  matriisi tmp;
  int i, j;
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      tmp[i][j] = a[i][0] * b[0][j] +
                  a[i][1] * b[1][j] +
                  a[i][2] * b[2][j];
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      a[i][j] = tmp[i][j];


	

//   - a b c -   - k l m -   - ak+bn+cq al+bo+cr am+bp+cs -
//   - d e f - * - n o p - = - dk+en+fq dl+eo+fr dm+ep+fs -
//   - h i j -   - q r s -   - hk+in+jq hl+io+jr hm+ip+js -

}

// mnozenie wektora przez macierz

void FOX_matrix_vector_mull(wektor,wektor_wynik, macierz)
{

 vertextype vek = vekto;

 result->x = mat[0][0] * vek.x + mat[1][0] * vek.y + mat[2][0] * vek.z;
 result->y = mat[0][1] * vek.x + mat[1][1] * vek.y + mat[2][1] * vek.z;
 result->z = mat[0][2] * vek.x + mat[1][2] * vek.y + mat[2][2] * vek.z;

//                - a b c 0 -
//   (Xi+Yj+Zk) * - e f g 0 - = (aX+eY+iZ+m)i + (bX+fY+jZ+n)j +
//                - i j k 0 -   (cX+gY+kZ+o)k
//                - m n o 1 -


}

// mnozenie wierzcholka przez macierz

void FOX_matrix_vertex_mull(macierz, sx, sy, sz, cx, cy, cz)
{

 m[0][0] = cy * cz;
 m[0][1] = cy * sz;
 m[0][2] = -sy;

 m[1][0] = sx * sy * cz - cx * sz;
 m[1][1] = sx * sy * sz + cx * cz;
 m[1][2] = sx * cy;

 m[2][0] = cx * sy * cz + sx * sz;
 m[2][1] = cx * sy * sz - sx * cz;
 m[2][2] = cx * cy;



//                     - cy*cz          cy*sz          -sy    0 -
//                     - sx*sy*cz-cx*sz sx*sy*sz+cx*cz  sx*cy 0 -
//       [X]*[Y]*[Z] = - cx*sy*cz+sx*sz cx*sy*sz-sx*cz  cx*cy 0 -
//                     - 0              0               0     1 -

}

// macierz jednostkowa

void FOX_matrix_one(macierz)
{
 m[0][0] = 1; m[1][0] = 0; m[2][0] = 0;
 m[0][1] = 0; m[1][1] = 1; m[2][1] = 0;
 m[0][2] = 0; m[1][2] = 0; m[2][2] = 1;

//     - 1 0 0 0 -
//     - 0 1 0 0 -
//     - 0 0 1 0 -
//     - 0 0 0 1 -


}
