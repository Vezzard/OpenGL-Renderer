#pragma once
#include "glm/detail/type_vec3.hpp"
#include "glm/detail/type_mat.hpp"

namespace Math {

   void matGetForward(const glm::mat4& mat, glm::vec3& frw);



   void matGetForward(const glm::mat4& mat, glm::vec3& frw)
   {
      frw.x = -mat[2][0];
      frw.y = -mat[2][1];
      frw.z = -mat[2][2];;
   }
}