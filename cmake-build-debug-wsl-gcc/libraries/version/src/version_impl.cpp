/**
 *  \warning This file is machine generated. DO NOT EDIT.  See version_impl.cpp.in for changes.
 */

#include "version_impl.hpp"

namespace eosio { namespace version {

   const std::string version_major {"4" };
   const std::string version_minor {"0" };
   const std::string version_patch {"4" };
   const std::string version_suffix{""};
   const std::string version_hash  {"526c3a4cfac8aba27b6241cf84f29a4968fd41e8"  };
   const bool        version_dirty { true  };

   std::string _version_client() {
      if( version_major == "unknown" || version_major.empty() || version_minor == "unknown" || version_minor.empty()) {
         return "unknown";
      } else {
         std::string version{'v' + version_major + '.' + version_minor};
         if( !version_patch.empty() )  version += '.' + version_patch;
         if( !version_suffix.empty() ) version += '-' + version_suffix;
         return version;
      }
   }

   std::string _version_full() {
      if( version_major == "unknown" || version_major.empty() || version_minor == "unknown" || version_minor.empty()) {
         return "unknown";
      } else {
         std::string version{'v' + version_major + '.' + version_minor};
         if( !version_patch.empty() )  version += '.' + version_patch;
         if( !version_suffix.empty() ) version += '-' + version_suffix;
         if( !version_hash.empty() )   version += '-' + version_hash;
         if( version_dirty )           version += "-dirty";
         return version;
      }
   }

   std::string _version_hash() {
      return version_hash;
   }

} }
