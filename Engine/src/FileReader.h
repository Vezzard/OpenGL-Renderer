#pragma once

#include <string>

namespace Engine {


   class FileReader
   {
   public:
      FileReader(const std::string& path) : m_path(path) { }

      bool Parse(std::string& out, bool cache = false) const;

   private:
      std::string m_path;
      mutable std::string m_sorce;
      mutable bool m_cached = false;
   };


}