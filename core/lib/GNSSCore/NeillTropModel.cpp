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


#include "YDSTime.hpp"
#include "NeillTropModel.hpp"

#define THROW_IF_INVALID_DETAILED() {if (!valid) {                 \
         InvalidTropModel e;                                            \
         if(!validHeight) e.addText("Invalid trop model: Rx Height");   \
         if(!validLat)  e.addText("Invalid trop model: Rx Latitude");   \
         if(!validDOY)   e.addText("Invalid trop model: day of year");  \
         GNSSTK_THROW(e);}}

namespace gnsstk
{
   NeillTropModel::NeillTropModel( const Position& RX,
                                   const CommonTime& time )
   {
      setReceiverHeight(RX.getAltitude());
      setReceiverLatitude(RX.getGeodeticLatitude( ));
      setDayOfYear(time);
   }



      /* Parameters borrowed from Saastamoinen tropospheric model
       * Constants for wet mapping function
       */
   static const double NeillWetA[5] =
   { 0.00058021897, 0.00056794847, 0.00058118019,
     0.00059727542, 0.00061641693 };
   static const double NeillWetB[5] =
   { 0.0014275268, 0.0015138625, 0.0014572752,
     0.0015007428, 0.0017599082 };
   static const double NeillWetC[5] =
   { 0.043472961, 0.046729510, 0.043908931,
     0.044626982, 0.054736038 };

      // constants for dry mapping function
   static const double NeillDryA[5] =
   { 0.0012769934, 0.0012683230, 0.0012465397,
     0.0012196049, 0.0012045996 };
   static const double NeillDryB[5] =
   { 0.0029153695, 0.0029152299, 0.0029288445,
     0.0029022565, 0.0029024912 };
   static const double NeillDryC[5] =
   { 0.062610505, 0.062837393, 0.063721774,
     0.063824265, 0.064258455 };

   static const double NeillDryA1[5] =
   { 0.0, 0.000012709626, 0.000026523662,
     0.000034000452, 0.000041202191 };
   static const double NeillDryB1[5] =
   { 0.0, 0.000021414979, 0.000030160779,
     0.000072562722, 0.00011723375 };
   static const double NeillDryC1[5] =
   { 0.0, 0.000090128400, 0.000043497037,
     0.00084795348, 0.0017037206 };


   double NeillTropModel::correction(double elevation) const
   {
      THROW_IF_INVALID_DETAILED();

         // Neill mapping functions work down to 3 degrees of elevation
      if(elevation < 3.0)
      {
         return 0.0;
      }

      double map_dry(NeillTropModel::dry_mapping_function(elevation));

      double map_wet(NeillTropModel::wet_mapping_function(elevation));

         // Compute tropospheric delay
      double tropDelay( (NeillTropModel::dry_zenith_delay() * map_dry) +
                        (NeillTropModel::wet_zenith_delay() * map_wet) );

      return tropDelay;
   }


   double NeillTropModel::correction( const Position& RX,
                                      const Position& SV )
   {

      try
      {
         setReceiverHeight( RX.getAltitude() );
         setReceiverLatitude(RX.getGeodeticLatitude());
         setWeather();
      }
      catch(GeometryException& e)
      {
         valid = false;
      }

      if(!valid)
      {
         throw InvalidTropModel("Invalid model");
      }

      double c;
      try
      {
         c = NeillTropModel::correction(RX.elevationGeodetic(SV));
      }
      catch(InvalidTropModel& e)
      {
         GNSSTK_RETHROW(e);
      }

      return c;
   }


   double NeillTropModel::correction( const Position& RX,
                                      const Position& SV,
                                      const CommonTime& tt )
   {
      setDayOfYear(tt);

      return NeillTropModel::correction(RX,SV);
   }


   double NeillTropModel::correction( const Position& RX,
                                      const Position& SV,
                                      const int& doy )
   {
      setDayOfYear(doy);

      return NeillTropModel::correction(RX,SV);
   }


   double NeillTropModel::correction( const Xvt& RX,
                                      const Xvt& SV )
   {
      Position R(RX),S(SV);
      return NeillTropModel::correction(R,S);
   }


   double NeillTropModel::correction( const Xvt& RX,
                                      const Xvt& SV,
                                      const CommonTime& tt )
   {
      setDayOfYear(tt);
      Position R(RX),S(SV);

      return NeillTropModel::correction(R,S);
   }


   double NeillTropModel::correction( const Xvt& RX,
                                      const Xvt& SV,
                                      const int& doy )
   {
      setDayOfYear(doy);
      Position R(RX),S(SV);

      return NeillTropModel::correction(R,S);
   }


   double NeillTropModel::dry_zenith_delay() const
   {
      THROW_IF_INVALID();

         // Note: 1.013*2.27 = 2.29951
      double ddry( 2.29951*std::exp( (-0.000116 * NeillHeight) ) );

         /* where does above come from? Not Neill 1996
          * probably ought to use SaasDryDelay
          */
      //return SaasDryDelay(press,NeillLat,NeillHeight);

      return ddry;
   }


   double NeillTropModel::dry_mapping_function(double elevation) const
   {
      THROW_IF_INVALID_DETAILED();

      if(elevation < 3.0)
      {
         return 0.0;
      }

      double lat, t, ct;
      lat = fabs(NeillLat);         // degrees
      t = static_cast<double>(NeillDOY) - 28.0;  // mid-winter

      if(NeillLat < 0.0)              // southern hemisphere
      {
         t += 365.25/2.;
      }

      t *= 360.0/365.25;            // convert to degrees
      ct = ::cos(t*DEG_TO_RAD);

      double a, b, c;
      if(lat < 15.0)
      {
         a = NeillDryA[0];
         b = NeillDryB[0];
         c = NeillDryC[0];
      }
      else if(lat < 75.)      // coefficients are for 15,30,45,60,75 deg
      {
         int i=int(lat/15.0)-1;
         double frac=(lat-15.*(i+1))/15.;
         a = NeillDryA[i] + frac*(NeillDryA[i+1]-NeillDryA[i]);
         b = NeillDryB[i] + frac*(NeillDryB[i+1]-NeillDryB[i]);
         c = NeillDryC[i] + frac*(NeillDryC[i+1]-NeillDryC[i]);

         a -= ct * (NeillDryA1[i] + frac*(NeillDryA1[i+1]-NeillDryA1[i]));
         b -= ct * (NeillDryB1[i] + frac*(NeillDryB1[i+1]-NeillDryB1[i]));
         c -= ct * (NeillDryC1[i] + frac*(NeillDryC1[i+1]-NeillDryC1[i]));
      }
      else
      {
         a = NeillDryA[4] - ct * NeillDryA1[4];
         b = NeillDryB[4] - ct * NeillDryB1[4];
         c = NeillDryC[4] - ct * NeillDryC1[4];
      }

      double se = ::sin(elevation*DEG_TO_RAD);
      double map = (1.+a/(1.+b/(1.+c)))/(se+a/(se+b/(se+c)));

      a = 0.0000253;
      b = 0.00549;
      c = 0.00114;
      map += ( NeillHeight/1000.0 ) *
         ( 1./se - ( (1.+a/(1.+b/(1.+c))) / (se+a/(se+b/(se+c))) ) );

      return map;
   }


   double NeillTropModel::wet_mapping_function(double elevation) const
   {
      THROW_IF_INVALID_DETAILED();

      if(elevation < 3.0)
      {
         return 0.0;
      }

      double a,b,c,lat;
      lat = fabs(NeillLat);         // degrees
      if(lat < 15.0)
      {
         a = NeillWetA[0];
         b = NeillWetB[0];
         c = NeillWetC[0];
      }
      else if(lat < 75.)          // coefficients are for 15,30,45,60,75 deg
      {
         int i=int(lat/15.0)-1;
         double frac=(lat-15.*(i+1))/15.;
         a = NeillWetA[i] + frac*(NeillWetA[i+1]-NeillWetA[i]);
         b = NeillWetB[i] + frac*(NeillWetB[i+1]-NeillWetB[i]);
         c = NeillWetC[i] + frac*(NeillWetC[i+1]-NeillWetC[i]);
      }
      else
      {
         a = NeillWetA[4];
         b = NeillWetB[4];
         c = NeillWetC[4];
      }

      double se = ::sin(elevation*DEG_TO_RAD);
      double map = ( 1.+ a/ (1.+ b/(1.+c) ) ) / (se + a/(se + b/(se+c) ) );

      return map;

   }  // end NeillTropModel::wet_mapping_function()


   void NeillTropModel::setWeather()
   {

      if(!validLat)
      {
         valid = false;
         InvalidTropModel e(
            "NeillTropModel must have Rx latitude before computing weather");
         GNSSTK_THROW(e);
      }
      if(!validDOY)
      {
         valid = false;
         InvalidTropModel e(
            "NeillTropModel must have day of year before computing weather");
         GNSSTK_THROW(e);
      }

      valid = validHeight && validLat && validDOY;

   }


   void NeillTropModel::setReceiverHeight(const double& ht)
   {
      NeillHeight = ht;
      validHeight = true;

         // Change the value of field "valid" if everything is already set
      valid = validHeight && validLat && validDOY;

         // If model is valid, set the appropriate parameters
      if (valid) setWeather();
   }


   void NeillTropModel::setReceiverLatitude(const double& lat)
   {
      NeillLat = lat;
      validLat = true;

         // Change the value of field "valid" if everything is already set
      valid = validHeight && validLat && validDOY;

         // If model is valid, set the appropriate parameters
      if (valid) setWeather();
   }


   void NeillTropModel::setDayOfYear(const int& doy)
   {
      if( (doy>=1) && (doy<=366) )
      {
         validDOY = true;
      }
      else
      {
         validDOY = false;
      }

      NeillDOY = doy;

         // Change the value of field "valid" if everything is already set
      valid = validHeight && validLat && validDOY;

         // If model is valid, set the appropriate parameters
      if (valid) setWeather();
   }


   void NeillTropModel::setDayOfYear(const CommonTime& time)
   {

      NeillDOY = static_cast<int>((static_cast<YDSTime>(time)).doy);
      validDOY = true;

         // Change the value of field "valid" if everything is already set
      valid = validHeight && validLat && validDOY;

         // If model is valid, set the appropriate parameters
      if (valid) setWeather();
   }


   void NeillTropModel::setAllParameters( const CommonTime& time,
                                          const Position& rxPos )
   {
      YDSTime ydst = static_cast<YDSTime>(time);
      NeillDOY = static_cast<int>(ydst.doy);
      validDOY = true;
      NeillLat = rxPos.getGeodeticLatitude();
      validLat = true;
      NeillHeight = rxPos.getHeight();
      validHeight = true;

         // Change the value of field "valid" if everything is already set
      valid = validHeight && validLat && validDOY;

         // If model is valid, set the appropriate parameters
      if (valid) setWeather();
   }

}
