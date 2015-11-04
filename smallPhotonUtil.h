/*
 * utilities related to photon physics
 */

#ifndef SMALLPHOTONUTIL_H_
#define SMALLPHOTONUTIL_H_

// compiling macros will give error if these classes are not included.
#include <TMath.h>

#include <iostream>
#include <algorithm>    // std::sort, std::count, std::min_element
#include <vector>       // std::vector

Double_t getDR(Double_t eta1, Double_t phi1, Double_t eta2, Double_t phi2);
Double_t getDPHI(Double_t phi1, Double_t phi2);
Double_t getDETA(Double_t eta1, Double_t eta2);
bool matchParticlePair(Double_t eta1, Double_t phi1, Double_t eta2, Double_t phi2, double thresholdDeltaR = 0.2);
bool matchParticlePair(Double_t eta1, Double_t phi1, Double_t eta2, Double_t phi2, Double_t pt1, Double_t  pt2,
                       double thresholdDeltaR = 0.2, double thresholdDeltapT = 5);
std::vector<int> matchGEN2RECO(const std::vector<float>* genPt,  const std::vector<float>* genEta, const std::vector<float>* genPhi,
                   const std::vector<float>* recoPt, const std::vector<float>* recoEta,const std::vector<float>* recoPhi,
                   const std::vector<bool>&   genValid, const std::vector<bool>&  recoValid,
                   double thresholdDeltaR);
std::vector<int> matchGEN2RECO(const std::vector<float>* genPt,  const std::vector<float>* genEta, const std::vector<float>* genPhi,
                   const std::vector<float>* recoPt, const std::vector<float>* recoEta,const std::vector<float>* recoPhi,
                   const std::vector<bool>&   genValid, const std::vector<bool>&  recoValid,
                   double thresholdDeltaR,
                   int* nGENinsideDeltaR);
std::vector<int> matchGEN2RECOVerbose(const std::vector<float>* genPt,  const std::vector<float>* genEta, const std::vector<float>* genPhi,
                   const std::vector<float>* recoPt, const std::vector<float>* recoEta,const std::vector<float>* recoPhi,
                   const std::vector<bool>&   genValid, const std::vector<bool>&  recoValid,
                   double thresholdDeltaR,
                   int* nGENinsideDeltaR);

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

/*
 * match a pair of particles based on deltaR
 * */
bool matchParticlePair(Double_t eta1, Double_t phi1, Double_t eta2, Double_t phi2, double thresholdDeltaR)
{
    return (getDR(eta1, phi1, eta2, phi2) < thresholdDeltaR);
}

/*
 * match a pair of particles based on deltaR and deltapT
 * */
bool matchParticlePair(Double_t eta1, Double_t phi1, Double_t eta2, Double_t phi2, Double_t pt1, Double_t  pt2,
                       double thresholdDeltaR, double thresholdDeltapT)
{
    return ((getDR(eta1, phi1, eta2, phi2) < thresholdDeltaR) && (TMath::Abs(pt1-pt2) < thresholdDeltapT));
}

/*
 * result is in vector "matched"
 * matched.at(i) = j;            GEN particle at index i matches RECO particle at index j
 * */
std::vector<int> matchGEN2RECO(const std::vector<float>* genPt,  const std::vector<float>* genEta, const std::vector<float>* genPhi,
                   const std::vector<float>* recoPt, const std::vector<float>* recoEta,const std::vector<float>* recoPhi,
                   const std::vector<bool>& genValid, const std::vector<bool>& recoValid,
                   double thresholdDeltaR)
{
   return matchGEN2RECO(genPt,  genEta,  genPhi,
                  recoPt, recoEta, recoPhi,
                  genValid, recoValid,
                  thresholdDeltaR,
                  NULL);
}

/*
 *
  function to match GEN particles to RECO particles. The matching is based on deltaR(GEN, RECO) and deltapT(GEN, RECO).
  Algorithm :
    1. For each GEN particle, a list of RECO particles which have deltaR(GEN, RECO) < "thresholdDeltaR" are marked as a possible match.
    2. From its possible match list, each GEN particle tries to match the RECO particle for which deltaR(GEN, RECO) is minimum.
    3. If there are more than one GEN particles which try to match the same RECO particle,
      then amongs these GEN candidates the one which has the minimum deltapT(GEN, RECO) will match the RECO particle.
      Remaining GEN particles will try to match the RECO particle with which they have their next minimum deltaR(GEN, RECO).
    4. Algorithm continues until each RECO particle has at most one GEN candidate.
 *
 * result is in vector "matched"
 * matched.at(i) = j;            GEN particle at index i matches RECO particle at index j
 * nGENinsideDeltaR[i] = n;      there are "n" GEN particles which have deltaR < "thresholdDeltaR"
 *                               with RECO particle at index i
 * */
std::vector<int> matchGEN2RECO(const std::vector<float>* genPt,  const std::vector<float>* genEta, const std::vector<float>* genPhi,
                   const std::vector<float>* recoPt, const std::vector<float>* recoEta,const std::vector<float>* recoPhi,
                   const std::vector<bool>&   genValid, const std::vector<bool>&  recoValid,
                   double thresholdDeltaR,
                   int* nGENinsideDeltaR)
{
    int nGEN  = genPt->size();
    int nRECO = recoPt->size();

    std::vector<double> deltaR[nGEN];   // array of vectors for GEN particle, each array element is a vector,
                                        // each vector contains deltaR values between the GEN particle and RECO particles
                                        // where deltaR is within the desired limits
    std::vector<double> deltapT[nGEN];
    std::vector<int> indicesRECO[nGEN];

    std::vector<int> indicesGEN[nRECO];

    if (nGENinsideDeltaR != NULL) {
        for (int i=0; i< nRECO; ++i) {
            nGENinsideDeltaR[i] = 0;  // default
        }
    }

    std::vector<int> matched;
    for (int i=0; i< nGEN; ++i)
    {
        matched.push_back(-1);   // by default, no GEN particle has a matched RECO particle.

        if (!genValid.at(i)) continue;

        for (int j=0; j<nRECO; ++j)
        {
            if (!recoValid.at(j)) continue;

            double deltaRtmp  = getDR(genEta->at(i), genPhi->at(i), recoEta->at(j), recoPhi->at(j));
            double deltapTtmp = TMath::Abs(genPt->at(i)-recoPt->at(j));
            bool passedDeltaR = (deltaRtmp < thresholdDeltaR);

            if(passedDeltaR)
            {
                if (nGENinsideDeltaR != NULL)  nGENinsideDeltaR[j]++;

                deltaR[i].push_back(deltaRtmp);
                deltapT[i].push_back(deltapTtmp);
                indicesRECO[i].push_back(j);
            }
        }

        if (indicesRECO[i].size() > 0){

            // sort indices of RECO particles wrt. their deltaR with the GEN particle
            // use lambda expression for that
            // http://stackoverflow.com/questions/25921706/creating-a-vector-of-indices-of-a-sorted-vector/25921907#25921907
            std::sort(indicesRECO[i].begin(), indicesRECO[i].end(),
                    [&](int i1, int i2) { return deltaR[i][i1] < deltaR[i][i2]; } );

            std::sort(deltapT[i].begin(), deltapT[i].end(),
                    [&](int i1, int i2) { return deltaR[i][i1] < deltaR[i][i2]; } );

            std::sort(deltaR[i].begin(), deltaR[i].end());

            // add the GEN particle to the GEN particles list of the RECO particle
            // where the GEN has its minimum deltaR with this RECO
            indicesGEN[indicesRECO[i].at(0)].push_back(i);
        }
    }

    bool eachRECOhasOneGENCand;  // the algorithm runs over the list of RECO particles until
                                 // each RECO particle has at most one GEN candidate

    do {
        eachRECOhasOneGENCand = true;
        for (int i=0; i<nRECO; ++i)
        {
            // if multiple GEN particles have their minimum deltaR with the same RECO particle
            // break ties using deltapT
            int nGEN_Cand = indicesGEN[i].size();
            if(nGEN_Cand > 1)
            {
                eachRECOhasOneGENCand = false;  // after this loop over RECO particles,
                                                // GEN candidate list for at least one RECO particle
                                                // will be updated.

                // loop over deltapT values between this RECO and its GEN candidates
                // where the GEN has its minimum deltaR with this RECO
                double MINdeltaPT = 999999;
                int    indexGEN_MINdeltaPT = -1;
                for (int j=0; j<nGEN_Cand; ++j)
                {
                    int indexGEN_Cand = indicesGEN[i].at(j);
                    if (deltapT[indexGEN_Cand].at(0) < MINdeltaPT) {
                        MINdeltaPT = deltapT[indexGEN_Cand].at(0);
                        indexGEN_MINdeltaPT = indexGEN_Cand;
                    }
                }

                // GEN particle that is the best match for this RECO in this iteration is now selected.
                // remaining GEN particles are eliminated.
                // for the remaining GEN particles that had their minimum deltaR with this RECO
                // from now on they will try to match the RECO with which they had their 2nd minimum deltaR
                for (int j=0; j<nGEN_Cand; ++j)
                {
                    int indexGEN_Cand = indicesGEN[i].at(j);
                    if (indexGEN_Cand != indexGEN_MINdeltaPT){

                        // remaining GEN particles will not try to match this RECO any more
                        indicesRECO[indexGEN_Cand].erase(indicesRECO[indexGEN_Cand].begin());
                        deltapT[indexGEN_Cand].erase(deltapT[indexGEN_Cand].begin());
                        deltaR[indexGEN_Cand].erase(deltaR[indexGEN_Cand].begin());

                        // if there are other RECO particles that the remaining GEN particles can match
                        // then they will try to match the next RECO particle in their list
                        if (indicesRECO[indexGEN_Cand].size() > 0) {
                            indicesGEN[indicesRECO[indexGEN_Cand].at(0)].push_back(indexGEN_Cand);
                        }
                    }
                }
                // clear the list of candidate GEN particles, only keep the one with min deltapT
                indicesGEN[i].clear();
                indicesGEN[i].push_back(indexGEN_MINdeltaPT);
            }
        }
    } while(!eachRECOhasOneGENCand);

    // use the best matched GEN particle from the last iteration
    for (int i=0; i<nRECO; ++i)
    {
        if(indicesGEN[i].size() > 0) {
            // the GEN particle that best matches this RECO after last iteration is at position 0.
            matched.at(indicesGEN[i].at(0)) = i;
        }
    }

    return matched;
}

/*
 * result is in vector "matched"
 * matched.at(i) = j;            GEN particle at index i matches RECO particle at index j
 * nGENinsideDeltaR[i] = n;      there are "n" GEN particles which have deltaR < "thresholdDeltaR"
 *                               with RECO particle at index i
 *
 * runs the function matchGEN2RECO() with verbose mode
 * to keep track of the steps in the algorithm
 * */
std::vector<int> matchGEN2RECOVerbose(const std::vector<float>* genPt,  const std::vector<float>* genEta, const std::vector<float>* genPhi,
                   const std::vector<float>* recoPt, const std::vector<float>* recoEta,const std::vector<float>* recoPhi,
                   const std::vector<bool>&   genValid, const std::vector<bool>&  recoValid,
                   double thresholdDeltaR,
                   int* nGENinsideDeltaR)
{
    int nGEN  = genPt->size();
    int nRECO = recoPt->size();

    std::vector<double> deltaR[nGEN];   // array of vectors for GEN particle, each array element is a vector,
                                        // each vector contains deltaR values between the GEN particle and RECO particles
                                        // where deltaR is within the desired limits
    std::vector<double> deltapT[nGEN];
    std::vector<int> indicesRECO[nGEN];

    std::vector<int> indicesGEN[nRECO];

    if (nGENinsideDeltaR != NULL) {
        for (int i=0; i< nRECO; ++i) {
            nGENinsideDeltaR[i] = 0;  // default
        }
    }

    ///// verbose /////
    int nGENphoton = std::count(genValid.begin(), genValid.end(), true);
    std::cout<<"nGEN                       = "<<nGEN<<std::endl;
    std::cout<<"nGEN that passed selection = "<<nGENphoton<<std::endl;
    ///// verbose /////

    std::vector<int> matched;
    for (int i=0; i< nGEN; ++i)
    {
        matched.push_back(-1);   // by default, no GEN particle has a matched RECO particle.

        if (!genValid.at(i)) continue;

        for (int j=0; j<nRECO; ++j)
        {
            if (!recoValid.at(j)) continue;

            double deltaRtmp  = getDR(genEta->at(i), genPhi->at(i), recoEta->at(j), recoPhi->at(j));
            double deltapTtmp = TMath::Abs(genPt->at(i)-recoPt->at(j));
            bool passedDeltaR = (deltaRtmp < thresholdDeltaR);

            if(passedDeltaR)
            {
                if (nGENinsideDeltaR != NULL)  nGENinsideDeltaR[j]++;

                deltaR[i].push_back(deltaRtmp);
                deltapT[i].push_back(deltapTtmp);
                indicesRECO[i].push_back(j);
            }
        }

        if (indicesRECO[i].size() > 0){

            ///// verbose /////
            std::cout<<"GEN at index "<<i<<" has "<<indicesRECO[i].size()<<" possible matches"<<std::endl;
            for (unsigned ii=0; ii<indicesRECO[i].size(); ++ii)
            {
                std::cout<<"indexRECO = "<<indicesRECO[i].at(ii)<<std::endl;
                std::cout<<"deltaR    = "<<deltaR[i].at(ii)<<std::endl;
                std::cout<<"deltapT   = "<<deltapT[i].at(ii)<<std::endl;
            }
            ///// verbose /////

            // sort indices of RECO particles wrt. their deltaR with the GEN particle
            // use lambda expression for that
            // http://stackoverflow.com/questions/25921706/creating-a-vector-of-indices-of-a-sorted-vector/25921907#25921907
            std::sort(indicesRECO[i].begin(), indicesRECO[i].end(),
                    [&](int i1, int i2) { return deltaR[i][i1] < deltaR[i][i2]; } );

            std::sort(deltapT[i].begin(), deltapT[i].end(),
                    [&](int i1, int i2) { return deltaR[i][i1] < deltaR[i][i2]; } );

            std::sort(deltaR[i].begin(), deltaR[i].end());

            // add the GEN particle to the GEN particles list of the RECO particle
            // where the GEN has its minimum deltaR with this RECO
            indicesGEN[indicesRECO[i].at(0)].push_back(i);

            ///// verbose /////
            std::cout<<"SORTED : GEN at index "<<i<<" has "<<indicesRECO[i].size()<<" possible matches"<<std::endl;
            for (unsigned ii=0; ii<indicesRECO[i].size(); ++ii)
            {
                std::cout<<"indexRECO = "<<indicesRECO[i].at(ii)<<std::endl;
                std::cout<<"deltaR    = "<<deltaR[i].at(ii)<<std::endl;
                std::cout<<"deltapT   = "<<deltapT[i].at(ii)<<std::endl;
            }
            ///// verbose /////
        }
    }

    bool eachRECOhasOneGENCand;  // the algorithm runs over the list of RECO particles until
    // each RECO particle has at most one GEN candidate

    ///// verbose /////
    int iter = 0;
    ///// verbose /////
    do {
        ///// verbose /////
        iter++;
        std::cout << "iteration = " << iter << std::endl;
        ///// verbose /////
        eachRECOhasOneGENCand = true;
        for (int i=0; i<nRECO; ++i)
        {
            // if multiple GEN particles have their minimum deltaR with the same RECO particle
            // break ties using deltapT
            int nGEN_Cand = indicesGEN[i].size();
            ///// verbose /////
            if (nGEN_Cand>0) {
                std::cout<<"RECO at index "<<i<<" has "<<nGEN_Cand<<" GEN candidates"<<std::endl;
            }
            ///// verbose /////
            if(nGEN_Cand > 1)
            {
                eachRECOhasOneGENCand = false;  // after this loop over RECO particles,
                                                // GEN candidate list for at least one RECO particle
                                                // will be updated.

                // loop over deltapT values between this RECO and its GEN candidates
                // where the GEN has its minimum deltaR with this RECO
                double MINdeltaPT = 999999;
                int    indexGEN_MINdeltaPT = -1;
                for (int j=0; j<nGEN_Cand; ++j)
                {
                    int indexGEN_Cand = indicesGEN[i].at(j);
                    if (deltapT[indexGEN_Cand].at(0) < MINdeltaPT) {
                        MINdeltaPT = deltapT[indexGEN_Cand].at(0);
                        indexGEN_MINdeltaPT = indexGEN_Cand;
                    }
                }

                // GEN particle that is the best match for this RECO in this iteration is now selected.
                // remaining GEN particles are eliminated.
                // for the remaining GEN particles that had their minimum deltaR with this RECO
                // from now on they will try to match the RECO with which they had their 2nd minimum deltaR
                for (int j=0; j<nGEN_Cand; ++j)
                {
                    int indexGEN_Cand = indicesGEN[i].at(j);
                    if (indexGEN_Cand != indexGEN_MINdeltaPT){

                        // remaining GEN particles will not try to match this RECO any more
                        indicesRECO[indexGEN_Cand].erase(indicesRECO[indexGEN_Cand].begin());
                        deltapT[indexGEN_Cand].erase(deltapT[indexGEN_Cand].begin());
                        deltaR[indexGEN_Cand].erase(deltaR[indexGEN_Cand].begin());

                        // if there are other RECO particles that the remaining GEN particles can match
                        // then they will try to match the next RECO particle in their list
                        if (indicesRECO[indexGEN_Cand].size() > 0) {
                            indicesGEN[indicesRECO[indexGEN_Cand].at(0)].push_back(indexGEN_Cand);
                        }
                    }
                }
                // clear the list of candidate GEN particles, only keep the one with min deltapT
                indicesGEN[i].clear();
                indicesGEN[i].push_back(indexGEN_MINdeltaPT);
            }
        }
    } while(!eachRECOhasOneGENCand);

    // use the best matched GEN particle from the last iteration
    for (int i=0; i<nRECO; ++i)
    {
        if(indicesGEN[i].size() > 0) {
            // the GEN particle that best matches this RECO after last iteration is at position 0.
            matched.at(indicesGEN[i].at(0)) = i;

            ///// verbose /////
            std::cout<<"GEN at index "<<indicesGEN[i].at(0)<<" will match RECO at index "<<i<<std::endl;
            ///// verbose /////
        }
    }

    return matched;
}

#endif /* SMALLPHOTONUTIL_H_ */
