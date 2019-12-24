#include "pch.h"
#include "FileReader.h"
#include <fstream>

namespace Engine {


   bool FileReader::Parse(std::string& out, bool cache) const
   {
      if (m_cached) {
         out = m_sorce;
         return true;
      }

      std::ifstream stream(m_path);
      if (!stream) {
         WARN_CORE("Couldn't open file {0}", m_path.c_str());
         return false;
      }

      std::string& buff = cache ? m_sorce : out;
      stream.seekg(0, std::ios::end);
      buff.resize(stream.tellg());
      stream.seekg(0, std::ios::beg);
      stream.read(&buff[0], buff.size());
      stream.close();

      if (cache) {
         out = m_sorce;
         m_cached = true;
      }

      return true;
   }


}
