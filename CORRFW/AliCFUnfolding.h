
#ifndef ALICFUNFOLDING_H
#define ALICFUNFOLDING_H

//--------------------------------------------------------------------//
//                                                                    //
// AliCFUnfolding Class                                               //
// Class to handle general unfolding procedure                        // 
// For the moment only bayesian unfolding is supported                //
// The next steps are to add chi2 minimisation and weighting methods  //
//                                                                    //
// Author : renaud.vernet@cern.ch                                     //
//--------------------------------------------------------------------//

#include "TNamed.h"
#include "THnSparse.h"

class TF1;

class AliCFUnfolding : public TNamed {

 public :
  AliCFUnfolding();
  AliCFUnfolding(const Char_t* name, const Char_t* title, const Int_t nVar, 
		 const THnSparse* response, const THnSparse* efficiency, const THnSparse* measured, const THnSparse* prior=0x0);
  AliCFUnfolding(const AliCFUnfolding& c);
  AliCFUnfolding& operator= (const AliCFUnfolding& c);
  ~AliCFUnfolding();

  void SetMaxNumberOfIterations(Int_t n)  {fMaxNumIterations=n;}
  void SetMaxChi2(Double_t val)           {fMaxChi2=val;}
  void SetMaxChi2PerDOF(Double_t val);
  void UseSmoothing(TF1* fcn=0x0, Option_t* opt="iremn") { // if fcn=0x0 then smooth using neighbouring bins 
    fUseSmoothing=kTRUE;                                   // this function must NOT be used if fNVariables > 3
    fSmoothFunction=fcn;                                   // the option "opt" is used if "fcn" is specified
    fSmoothOption=opt;
  } 
                                                                                                
  void Unfold();

  THnSparse* GetResponse()        const {return fResponse;}
  THnSparse* GetInverseResponse() const {return fInverseResponse;}
  THnSparse* GetPrior()           const {return fPrior;}
  THnSparse* GetOriginalPrior()   const {return fOriginalPrior;}
  THnSparse* GetEfficiency()      const {return fEfficiency;}
  THnSparse* GetUnfolded()        const {return fUnfolded;}
  THnSparse* GetEstMeasured()     const {return fMeasuredEstimate;}
  THnSparse* GetConditional()     const {return fConditional;}
  TF1*       GetSmoothFunction()  const {return fSmoothFunction;}

 private :
  
  // user-related settings
  THnSparse     *fResponse;           // Response matrix : dimensions must be 2N = 2 x (number of variables)
                                      // dimensions 0 ->  N-1 must be filled with reconstructed values
                                      // dimensions N -> 2N-1 must be filled with generated values
  THnSparse     *fPrior;              // This is the assumed generated distribution : dimensions must be N = number of variables
                                      // it will be used at the first step 
                                      // then will be updated automatically at each iteration
  THnSparse     *fEfficiency;         // Efficiency map : dimensions must be N = number of variables
                                      // this map must be filled only with "true" values of the variables (should not include resolution effects)
  THnSparse     *fMeasured;           // Measured spectrum to be unfolded : dimensions must be N = number of variables
  Int_t          fMaxNumIterations;   // Maximum  number of iterations to be performed
  Int_t          fNVariables;         // Number of variables used in analysis spectra (pt, y, ...)
  Double_t       fMaxChi2;            // Maximum Chi2 between unfolded and prior distributions. 
  Bool_t         fUseSmoothing;       // Smooth the unfolded sectrum at each iteration
  TF1           *fSmoothFunction;     // Function used to smooth the unfolded spectrum
  Option_t      *fSmoothOption;       // Option to use during the fit (with fSmoothFunction) ; default is "iremn"

  // internal settings
  THnSparse     *fOriginalPrior;     // This is the original prior distribution : will not be modified
  THnSparse     *fInverseResponse;   // Inverse response matrix
  THnSparse     *fMeasuredEstimate;  // Estimation of the measured (M) spectrum given the a priori (T) distribution
  THnSparse     *fConditional;       // Matrix holding the conditional probabilities P(M|T)
  THnSparse     *fProjResponseInT;   // Projection of the response matrix on TRUE axis
  THnSparse     *fUnfolded;          // Unfolded spectrum
  Int_t         *fCoordinates2N;     // Coordinates in 2N (measured,true) space
  Int_t         *fCoordinatesN_M;    // Coordinates in measured space
  Int_t         *fCoordinatesN_T;    // Coordinates in true space
  

  // functions
  void     Init();                  // initialisation of the internal settings
  void     GetCoordinates();        // gets a cell coordinates in Measured and True space
  void     CreateConditional();     // creates the conditional matrix from the response matrix
  void     CreateEstMeasured();     // creates the measured spectrum estimation from the conditional matrix and the prior distribution
  void     CreateInvResponse();     // creates the inverse response function (Bayes Theorem) from the conditional matrix and the prior distribution
  void     CreateUnfolded();        // creates the unfolded spectrum from the inverse response matrix and the measured distribution
  void     CreateFlatPrior();       // creates a flat a priori distribution in case the one given in the constructor is null
  Double_t GetChi2();               // returns the chi2 between unfolded and prior spectra
  Short_t  Smooth();                // function calling smoothing methods
  Short_t  SmoothUsingNeighbours(); // smoothes the unfolded spectrum using the neighbouring cells
  Short_t  SmoothUsingFunction();   // smoothes the unfolded spectrum using a fit function

  ClassDef(AliCFUnfolding,0);
};

#endif
