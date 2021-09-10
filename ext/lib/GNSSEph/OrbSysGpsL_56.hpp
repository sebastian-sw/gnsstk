//==============================================================================
//
//  This file is part of GNSSTk, the GNSS Toolkit.
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
//  Copyright 2004-2021, The Board of Regents of The University of Texas System
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
 * @file OrbSysGpsL_56.hpp
 * Designed to support loading GPS LNAV Iono/UTC data
 * Subframe 4, Page 18
 */

#ifndef SGLTK_ORBSYSGPSL_56_HPP
#define SGLTK_ORBSYSGPSL_56_HPP


#include <string>
#include <iostream>
#include <math.h>

#include "OrbSysGpsL.hpp"
#include "OrbDataUTC.hpp"
#include "TimeSystemCorr.hpp"    // For class TimeSystemCorrection

namespace gnsstk
{
   class OrbSysGpsL_56 : public OrbSysGpsL, public OrbDataUTC
   {
   public:
      static const double SIX_HOURS;

         /// Default constructor
      OrbSysGpsL_56();

         /** Constructor for creating directly from a PackedNavBits object
          * @throw InvalidParameter
          */
      OrbSysGpsL_56(const PackedNavBits& msg);

         /// Destructor
      virtual ~OrbSysGpsL_56() {}

         /// Clone method
      virtual OrbSysGpsL_56* clone() const;

         /**
          * Store the contents of Subframe 4, Page 18 in this object.
          * @param msg - 300 bits of Subframe 4, Page 18.
          * @throw InvalidParameter if message data is invalid
          */
      virtual void loadData(const PackedNavBits& msg);

      virtual bool isSameData(const OrbData* right) const;

      virtual std::string getName() const
      {
         return "UTC/I";
      }

      virtual std::string getNameLong() const
      {
         return "GPS LNAV Iono/UTC Parameters";
      }

        /**
         * Determine if UTC values are valid based on limitations
         * expressed in IS-GPS-200 20.3.3.5.2.4 and Karl Kovach's
         * interpretation of same following UTC Offset Error
         * anomaly of Jan 25-26, 2016
         */
      virtual bool isUtcValid(const CommonTime& ct,
                              const bool initialXMit=false) const;

         /**
          * Compute GPS-UTC offset as per IS-GPS-200 20.3.3.5.2.4.
          * NOTE: See preceding method, isUtcValid( ) to determine
          * if provided parameters are OK to use.
          */
      virtual double getUtcOffset(const CommonTime& ct) const;
      virtual double getUtcOffsetModLeapSec(const CommonTime& ct) const;

         /**
          * @throw InvalidRequest
          */
      virtual TimeSystemCorrection getTSC() const;

         /**
          * @throw InvalidRequest
          */
      virtual void dumpUtcTerse(std::ostream& s = std::cout,
                                const std::string tform="%02m/%02d/%04Y %03j %02H:%02M:%02S") const;

         /** Output the contents of this orbit data to the given stream.
          * @throw InvalidRequest if the required data has not been stored.
          */
      virtual void dumpTerse(std::ostream& s = std::cout) const
      { dumpUtcTerse(s); }

      //virtual void dumpHeader(std::ostream& s = std::cout) const

      /**
       * @throw InvalidRequest
       */
      virtual void dumpBody(std::ostream& s = std::cout) const;

         // Iono parameters
      double alpha[4];
      double beta[4];

         // UTC Parameters
      //double A0;
      //double A1;
      //gnsstk::CommonTime ctEpoch;

      short dtLS;
      short dtLSF;
      unsigned short WN_LSF;
      unsigned short DN;

      unsigned short WN_LSF_full;    // derived
      gnsstk::CommonTime ctLSF;       // derived

   }; // end class OrbSysGpsL_56

} // end namespace gnsstk

#endif

