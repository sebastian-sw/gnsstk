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

// gnsstk
#include "ENUUtil.hpp"

namespace gnsstk
{

//using namespace std; 

ENUUtil::ENUUtil(const double refLatRad,
                 const double refLonRad)
{
   compute( refLatRad, refLonRad ); 
}

//
//
void ENUUtil::compute( const double refLat,
                       const double refLon )
{
   rotMat.resize(3,3);
   rotMat (0,0) =  -std::sin(refLon);
   rotMat (1,0) =  -std::sin(refLat)*std::cos(refLon);
   rotMat (2,0) =   std::cos(refLat)*std::cos(refLon);
   rotMat (0,1) =   std::cos(refLon);
   rotMat (1,1) =  -std::sin(refLat)*std::sin(refLon);
   rotMat (2,1) =   std::cos(refLat)*std::sin(refLon);
   rotMat (0,2) =   0.0;
   rotMat (1,2) =   std::cos(refLat);
   rotMat (2,2) =   std::sin(refLat);
}

void ENUUtil::updatePosition( const double refLatRad,
                              const double refLonRad )
{
   compute( refLatRad, refLonRad );
}


gnsstk::Vector<double> ENUUtil::convertToENU( const gnsstk::Vector<double>& inV ) const
{
   gnsstk::Vector<double> outV(3);

   if (inV.size()!=3)
   {
      gnsstk::Exception e("Incompatible dimensions for Vector");
      GNSSTK_THROW(e);
   }
   outV = rotMat * inV;
   return(outV);
}
   
gnsstk::Triple ENUUtil::convertToENU( const gnsstk::Triple& inVec ) const
{
   gnsstk::Vector<double> v(3); 
   v[0] = inVec[0];
   v[1] = inVec[1];
   v[2] = inVec[2];
   
   gnsstk::Vector<double> vOut = convertToENU( v );
   gnsstk::Triple outVec( vOut[0], vOut[1], vOut[2] );
   return(outVec);
}
   
gnsstk::Xvt ENUUtil::convertToENU( const gnsstk::Xvt& in ) const
{
   gnsstk::Xvt out;
   out.clkbias = in.clkbias;
   out.relcorr = in.relcorr;
   out.clkdrift = in.clkdrift;
   out.x = convertToENU( in.x );
   out.v = convertToENU( in.v );
   return(out);
}
}     // end namespace gnsstk
