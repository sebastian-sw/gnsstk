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
 * @file GNSSconstants.hpp
 * Constants as defined in the GPS-ICD-200D and by all RINEX GNSSs
 */

#ifndef GNSSTK_GNSS_CONSTANTS_HPP
#define GNSSTK_GNSS_CONSTANTS_HPP

#include "Exception.hpp"
#include "SatID.hpp"
#include "DeprecatedConsts.hpp"

namespace gnsstk
{
      /**
       * @ingroup geodeticgroup
       * @name GNSS Constants
       * Time constants are in TimeConstants.hpp
       */
      //@{

      // ---------------- independent of GNSS ----------------------
      /// GPS value of PI; also specified by GAL
   const double PI        = 3.141592653589793238462643383280;
      /// Multiply degrees by DEG2RAD to get radians.
   const double DEG2RAD   = PI / 180.0;
      /// Multiply radians by RAD2DEG to get degrees.
   const double RAD2DEG   = 180.0 / PI;
      /// GPS value of PI*2
   const double TWO_PI    = 6.283185307179586476925286766559;
      /// GPS value of PI**0.5
   const double SQRT_PI   = 1.772453850905516027298167483341;
      /// relativity constant (sec/sqrt(m))
   const double REL_CONST = -4.442807633e-10;
      /// m/s, speed of light; this value defined by GPS but applies to GAL and GLO.
   const double C_MPS = 2.99792458e8;
      /// Conversion Factor from degrees to radians (units: degrees^-1)
   static const double DEG_TO_RAD = 1.7453292519943e-2;
      /// Conversion Factor from radians to degrees (units: degrees)
   static const double RAD_TO_DEG = 57.295779513082;
      /// degrees per milliarcsecond (1e-3/3600.)
   static const double DEG_PER_MAS = 2.77777777777e-7;
      /// radians per milliarcsecond
   static const double RAD_PER_MAS = 4.84813681e-9;
      /// parts per billion
   static const double PPB = 1.e-9;

      // ---------------- GPS --------------------------------------
      /// Hz, GPS Oscillator or chip frequency
   const double OSC_FREQ_GPS  = 10.23e6;
      /// Hz, GPS chip rate of the P & Y codes
   const double PY_CHIP_FREQ_GPS = OSC_FREQ_GPS;
      /// Hz, GPS chip rate of the C/A code
   const double CA_CHIP_FREQ_GPS = OSC_FREQ_GPS / 10.0;
      /// Hz, GPS Base freq w/o relativisitic effects
   const double RSVCLK_GPS    = 10.22999999543e6;
      /// GPS L1 frequency in units of oscillator frequency
   const double L1_MULT_GPS   = 154.0;
      /// GPS L2 frequency in units of oscillator frequency
   const double L2_MULT_GPS   = 120.0;
      /// GPS L5 frequency in units of oscillator frequency.
   const double L5_MULT_GPS   = 115.0;
      /// GPS Gamma constants
   const double GAMMA_GPS_12 = (L1_MULT_GPS/L2_MULT_GPS) * (L1_MULT_GPS/L2_MULT_GPS);
   const double GAMMA_GPS_15 = (L1_MULT_GPS/L5_MULT_GPS) * (L1_MULT_GPS/L5_MULT_GPS);
   const double GAMMA_GPS = 1.646944444;  // legacy notation and hard-coded value; wherefore the extra "44"??
      /// Reference Semi-major axis. From IS-GPS-800 Table 3.5-2 in meters.
   const double A_REF_GPS = 26559710.0;
      /// Omega reference value from Table 30-I converted to radians
   const double OMEGADOT_REF_GPS = -2.6e-9 * PI;

   inline
   short getLegacyFitInterval(const short iodc, const short fiti)
   {
         /* check the IODC */
      if (iodc < 0 || iodc > 1023)
      {
            /* error in iodc, return minimum fit */
         return 4;
      }

      if ( ( ( (fiti == 0) && (iodc & 0xFF) < 240 )
             || (iodc & 0xFF) > 255 ) )
      {
            /* fit interval of 4 hours */
         return 4;
      }
      else if (fiti == 1)
      {
         if( ((iodc & 0xFF) < 240 || (iodc & 0xFF) > 255))
         {
               /* fit interval of 6 hours */
            return 6;
         }
         else if(iodc >=240 && iodc <=247)
         {
               /* fit interval of 8 hours */
            return 8;
         }
         else if( ( (iodc >= 248) && (iodc <= 255) ) || iodc == 496 )
         {
               /* fit interval of 14 hours */
            return 14;
         }
         else if((iodc >= 497 && iodc <=503) || (iodc >= 1021 && iodc <= 1023))
         {
               /* fit interval of 26 hours */
            return 26;
         }
         else if(iodc >= 504 && iodc <=510)
         {
               /* fit interval of 50 hours */
            return 50;
         }
         else if( iodc == 511 || ( (iodc >= 752) && (iodc <= 756) ) )
         {
               /* fit interval of 74 hours */
            return 74;
         }
         else if(iodc == 757)
         {
               /* fit interval of 98 hours */
            return 98;
         }
         else
         {
            InvalidRequest exc("Invalid IODC Value For sv Block");
            GNSSTK_THROW(exc);
         }
      }
      else
      {
            /* error in ephemeris/iodc, return minimum fit */
         return 4;
      }

      return 0; // never reached
   }

      // ---------------- GLONASS ----------------------------------
      /// GLO Fundamental chip rate in Hz.
   const double OSC_FREQ_GLO  = 5.11e6;
      /// GLO Chip rate of the P & Y codes in Hz.
   const double PY_CHIP_FREQ_GLO = OSC_FREQ_GLO;
      /// GLO Chip rate of the C/A code in Hz.
   const double CA_CHIP_FREQ_GLO = OSC_FREQ_GLO / 10.0;

      /// GLO Fundamental oscillator freq in Hz.
   const double PSC_FREQ_GLO  = 5.00e6;
      /// GLO Base freq w/o relativisitic effects in Hz.
   const double RSVCLK_GLO    = 4.99999999782e6;

      /// GLO L1 multiplier.
   const double L1_MULT_GLO   = 320.4;
      /// GLO L2 multiplier.
   const double L2_MULT_GLO   = 249.2;

      /// Constant for the max array index in SV accuracy table.
   const int SV_ACCURACY_GLO_INDEX_MAX = 15;
      /// Map from SV accuracy/URA flag to NOMINAL accuracy values in m.
      /// Further details in ICD-GLO-v5.0, Table 4.4 in Section 4.4.
   const double SV_ACCURACY_GLO_INDEX[] = { 1.0,  2.0,   2.5,   4.0,   5.0,
                                            7.0, 10.0,  12.0,  14.0,  16.0,
                                            32.0, 64.0, 128.0, 256.0, 512.0,
                                            9.999999999999e99               };

      // ---------------- BeiDou ----------------------------------
      /// The maximum number of active satellites in the Compass constellation.
   const long MAX_PRN_COM     = 63;
      /// The first ranging code number for BeiDou GEO phase II satellites
   const long MIN_GEO_BDS_II  = 1;
      /// The last ranging code number for BeiDou GEO phase II satellites
   const long MAX_GEO_BDS_II  = 5;
      /// The first ranging code number for BeiDou MEO/IGSO satellites
   const long MIN_MEO_BDS     = 6;
      /// The last ranging code number for BeiDou MEO/IGSO satellites
   const long MAX_MEO_BDS     = 58;
      /// The first ranging code number for BeiDou GEO phase III satellites
   const long MIN_GEO_BDS_III = 59;
      /// The last ranging code number for BeiDou GEO phase III satellites
   const long MAX_GEO_BDS_III = 63;

      /// Return true if the given SatID is BeiDou GEO (expecting D2 nav).
   inline bool isBeiDouGEO(const SatID& sat)
   {
      return ((sat.system == SatelliteSystem::BeiDou) &&
              (((sat.id >= MIN_GEO_BDS_II) && (sat.id <= MAX_GEO_BDS_II)) ||
               ((sat.id >= MIN_GEO_BDS_III) && (sat.id <= MAX_GEO_BDS_III))));
   }

      // ---------------- QZSS ----------------------------------
      // PRN range defined in QZSS IS Table 3.2.2-2
      // PRN 198-202 reserved for maint./test, not to be used
      // by users.
      // These need to be defined in order to differentiate
      // QZSS from GPS in cases where QZSS is broadcasting
      // GPS-similar signals and navigation messages.
      // Note that 203-206 are "alternate" PRNs for satellites, see
      // Table 3.2.1-1
      // Defined as int to be compatible with gnsstk::SatID.id member.
      /// First assigned PRN in QZSS
   const int MIN_PRN_QZS = 193;
      /// Last assigned PRN in QZSS (all signals)
   const int MAX_PRN_QZS = 206;
      /// Last assigned PRN in QZSS (LNAV only)
   const int MAX_PRN_QZS_LNAV = 202;

      //@}

} // namespace

// this is at the end because it needs some of the constants defined above
#include "FreqConsts.hpp"

#endif //GNSSTK_GNSS_CONSTANTS_HPP
