//==============================================================================
//
//  This file is part of GNSSTk, the ARL:UT GNSS Toolkit.
//
//  The GNSSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GNSSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GNSSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2022, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public
//                            release, distribution is unlimited.
//
//==============================================================================

/**
 * @file AshtechStream.hpp
 * gnsstk::AshtechStream - Decode data from Ashtech Z(Y)-12 recerivers.
 */

#ifndef ASHTECHSTREAM_HPP
#define ASHTECHSTREAM_HPP

#include "FFBinaryStream.hpp"

namespace gnsstk
{
   /**
    * This is a stream used to decode data in the Ashtech format.
    */
   class AshtechStream : public FFBinaryStream
   {
   public:
      AshtechStream():header(false) {}

      /**
       * @param fn the name of the Ashtech file to be opened
       * @param mode the ios::openmode to be used on \a fn
       */
      AshtechStream(const char* fn, std::ios::openmode mode = std::ios::in)
         : FFBinaryStream(fn, mode)
      {}

      /// destructor per the coding standards
      virtual ~AshtechStream()
      {}

      /// overrides open to reset the header
      virtual void open(const char* fn, std::ios::openmode mode = std::ios::in)
      {
         FFBinaryStream::open(fn, mode);
      }

      /// The raw bytes read from the file.
      std::string rawData;

      // set true when a header was the last piece read, set false when the
      // a body is read.
      bool header;

      // Offset of the first character in rawData in the file
      std::streampos getRawPos()
      {
         std::streampos t = tellg();
         if (static_cast<long>(t)==-1)
            return -1;
         else
            return t - static_cast<std::streampos>(rawData.length());
      }

         /// Ashtech data is always big endian
      virtual bool isStreamLittleEndian() const noexcept
      { return false; }

   }; // class AshtechStream
} // namespace gnsstk

#endif
