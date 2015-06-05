#ifndef SMALLPHOTONUTIL_H_
#define SMALLPHOTONUTIL_H_

// compiling macros will give error if these classes are not included.
#include <TMath.h>

#include <iostream>

Double_t getDR( Double_t eta1, Double_t phi1, Double_t eta2, Double_t phi2);
Double_t getDPHI( Double_t phi1, Double_t phi2);
Double_t getDETA(Double_t eta1, Double_t eta2);

using  std::cout;
using  std::endl;

/*
 * copied from https://github.com/CmsHI/HiForestAnalysis/blob/master/commonUtility.h
 */
Double_t getDR( Double_t eta1, Double_t phi1, Double_t eta2, Double_t phi2){
  Double_t theDphi = getDPHI( phi1, phi2);
  Double_t theDeta = eta1 - eta2;
  return TMath::Sqrt ( theDphi*theDphi + theDeta*theDeta);
}

/*
 * copied from https://github.com/CmsHI/HiForestAnalysis/blob/master/commonUtility.h
 */
Double_t getDPHI( Double_t phi1, Double_t phi2) {
  Double_t dphi = phi1 - phi2;

  if ( dphi > 3.141592653589 )
    dphi = dphi - 2. * 3.141592653589;
  if ( dphi <= -3.141592653589 )
    dphi = dphi + 2. * 3.141592653589;

  if ( TMath::Abs(dphi) > 3.141592653589 ) {
    cout << " commonUtility::getDPHI error!!! dphi is bigger than 3.141592653589 " << endl;
  }

  return dphi;
}

Double_t getDETA(Double_t eta1, Double_t eta2){
	return eta1 - eta2;
}

#endif /* SMALLPHOTONUTIL_H_ */
