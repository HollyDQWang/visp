/****************************************************************************
 *
 * $Id: vpFeatureThetaU.cpp,v 1.7 2007-04-20 14:22:24 asaunier Exp $
 *
 * Copyright (C) 1998-2006 Inria. All rights reserved.
 *
 * This software was developed at:
 * IRISA/INRIA Rennes
 * Projet Lagadic
 * Campus Universitaire de Beaulieu
 * 35042 Rennes Cedex
 * http://www.irisa.fr/lagadic
 *
 * This file is part of the ViSP toolkit.
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech AS of Norway and appearing in the file
 * LICENSE included in the packaging of this file.
 *
 * Licensees holding valid ViSP Professional Edition licenses may
 * use this file in accordance with the ViSP Commercial License
 * Agreement provided with the Software.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Contact visp@irisa.fr if any conditions of this licensing are
 * not clear to you.
 *
 * Description:
 * ThetaU visual feature.
 *
 * Authors:
 * Eric Marchand
 *
 *****************************************************************************/


#include <visp/vpBasicFeature.h>
#include <visp/vpFeatureThetaU.h>
#include <visp/vpMath.h>

// Exception
#include <visp/vpException.h>
#include <visp/vpMatrixException.h>
#include <visp/vpFeatureException.h>

// Debug trace
#include <visp/vpDebug.h>


/*!
  \file vpFeatureThetaU.cpp
  \brief class that defines the thetaU visual feature
*/
/*

attributes and members directly related to the vpBasicFeature needs
other functionalities ar usefull but not mandatory

*/

void
vpFeatureThetaU::init()
{
    //feature dimension
    dim_s = 3 ;

    // memory allocation
    s.resize(dim_s) ;

}

vpFeatureThetaU::vpFeatureThetaU() : vpBasicFeature()
{
    init() ;
}

vpFeatureThetaU::vpFeatureThetaU(vpRotationMatrix &cdRc) : vpBasicFeature()
{
    init() ;

    vpThetaUVector tu(cdRc) ;
    buildFrom(tu) ;
}

vpFeatureThetaU::vpFeatureThetaU(vpHomogeneousMatrix &cdMc) : vpBasicFeature()
{
    init() ;
    vpRotationMatrix cdRc ;
    cdMc.extract(cdRc)  ;
    vpThetaUVector tu(cdRc) ;
    buildFrom(tu) ;
}

void
vpFeatureThetaU::buildFrom(vpThetaUVector &tu)
{
  s[0] = tu[0] ;
  s[1] = tu[1] ;
  s[2] = tu[2] ;
}

void vpFeatureThetaU::set_TUx(const double TUx)
{
    s[0] = TUx ;
}

double vpFeatureThetaU::get_TUx()  const
{
    return s[0] ;
}

void vpFeatureThetaU::set_TUy(const double TUy)
{
    s[1] = TUy ;
}

double vpFeatureThetaU::get_TUy()   const
{
    return s[1] ;
}

void
vpFeatureThetaU::set_TUz(const double TUz)
{
    s[2] = TUz ;
}

double
vpFeatureThetaU::get_TUz() const
{
    return  s[2]  ;
}

void
vpFeatureThetaU::buildFrom(const vpRotationMatrix &cdRc)
{
    vpThetaUVector tu(cdRc) ;
    buildFrom(tu) ;
}

void
vpFeatureThetaU::buildFrom(const vpHomogeneousMatrix &cdMc)
{
    vpRotationMatrix cdRc ;
    cdMc.extract(cdRc)  ;
    vpThetaUVector tu(cdRc) ;
    buildFrom(tu) ;
}

//! compute the interaction matrix from a subset a the possible features
vpMatrix
vpFeatureThetaU::interaction(const int select) const
{

  vpMatrix L ;
  L.resize(0,6) ;

  vpMatrix Lw(3,3) ;
  Lw.setIdentity() ;

  double  theta = sqrt(s.sumSquare()) ;

  vpColVector U(3)  ;
  for (int i=0 ; i < 3 ; i++) U[i] = s[i]/theta ;

  vpMatrix skewU ;
  skewU = vpColVector::skew(U) ;

 if (theta < 1e-6)
  {
    Lw.setIdentity() ;
  }
 else
   Lw += -theta/2.0*skewU +
    (1-vpMath::sinc(theta)/vpMath::sqr(vpMath::sinc(theta/2.0)))*skewU*skewU ;



  //This version is a simplification
  if (vpFeatureThetaU::selectTUx() & select )
  {
    vpMatrix Lx(1,6) ;

    Lx[0][0] = 0 ;    Lx[0][1] = 0 ;    Lx[0][2] = 0 ;
    for (int i=0 ; i < 3 ; i++) Lx[0][i+3] = Lw[0][i] ;


    L = vpMatrix::stackMatrices(L,Lx) ;
  }

  if (vpFeatureThetaU::selectTUy() & select )
  {
    vpMatrix Ly(1,6) ;

    Ly[0][0] = 0 ;    Ly[0][1] = 0 ;    Ly[0][2] = 0 ;
    for (int i=0 ; i < 3 ; i++) Ly[0][i+3] = Lw[1][i] ;

    L = vpMatrix::stackMatrices(L,Ly) ;
  }

  if (vpFeatureThetaU::selectTUz() & select )
  {
    vpMatrix Lz(1,6) ;

    Lz[0][0] = 0 ;    Lz[0][1] = 0 ;    Lz[0][2] = 0 ;
    for (int i=0 ; i < 3 ; i++) Lz[0][i+3] = Lw[2][i] ;

    L = vpMatrix::stackMatrices(L,Lz) ;
  }

  return L ;
}

//! compute the error between two visual features from a subset
//! a the possible features
vpColVector
vpFeatureThetaU::error(const vpBasicFeature &s_star,
	       const int select)
{

  if (fabs(s_star.get_s().sumSquare()) > 1e-6)
  {

    vpERROR_TRACE("s* should be zero ! ") ;
        throw(vpFeatureException(vpFeatureException::badInitializationError,
				 "s* should be zero !")) ;

  }

    vpColVector e(0) ;


    if (vpFeatureThetaU::selectTUx() & select )
    {
	vpColVector ex(1) ;
	ex[0] = s[0]  ;
	e = vpMatrix::stackMatrices(e,ex) ;
    }

    if (vpFeatureThetaU::selectTUy() & select )
    {
	vpColVector ey(1) ;
	ey[0] = s[1] ;
	e = vpMatrix::stackMatrices(e,ey) ;
    }

    if (vpFeatureThetaU::selectTUz() & select )
    {
	vpColVector ez(1) ;
	ez[0] = s[2] ;
	e = vpMatrix::stackMatrices(e,ez) ;
    }


    return e ;

}


void
vpFeatureThetaU::print(const int select) const
{
  std::cout <<"ThetaU: " << s.t() ;
}

vpFeatureThetaU *vpFeatureThetaU::duplicate() const
{
  vpFeatureThetaU *feature  = new vpFeatureThetaU ;
  return feature ;
}

void
vpFeatureThetaU::display(const vpCameraParameters &cam,
			 vpImage<unsigned char> &I,
			 vpColor::vpColorType color) const
{
  static int firsttime =0 ;

  if (firsttime==0)
  {
    firsttime=1 ;
    vpERROR_TRACE("not implemented") ;
    // Do not throw and error since it is not subject
    // to produce a failure
  }
}

/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
