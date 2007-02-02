#include "SimGeneral/NoiseGenerators/interface/CorrelatedNoisifier.h"
#include "FWCore/Utilities/interface/Exception.h"

CorrelatedNoisifier::CorrelatedNoisifier(int nFrames)
: theCovarianceMatrix(nFrames, 1.0),
  theMatrix(nFrames, 1.0),
  theRandomGaussian(*(HepRandom::getTheEngine())),
  theSize(nFrames)
{

  isDiagonal_ = true;
  checkOffDiagonal(isDiagonal_);

  computeDecomposition();

}


CorrelatedNoisifier::CorrelatedNoisifier(const HepSymMatrix & matrix)
: theCovarianceMatrix(matrix.num_row(),matrix),
  theMatrix(matrix.num_row(),1.0),
  theRandomGaussian(*(HepRandom::getTheEngine())),
  theSize(theCovarianceMatrix.rank())
{

  isDiagonal_ = true;
  checkOffDiagonal(isDiagonal_);

  computeDecomposition();

}


void CorrelatedNoisifier::setDiagonal(double value) 
{
  for(int i = 0; i < theSize; ++i) 
  {
    theCovarianceMatrix(i,i) = value;
  }

  isDiagonal_ = true;
  checkOffDiagonal(isDiagonal_);

  computeDecomposition();

} 

void CorrelatedNoisifier::setOffDiagonal(int distance, double value)
{
  for(int column = 0; column < theSize; ++column)
  {
    int row = column - distance;
    if(row < 0) continue;
    theCovarianceMatrix(row,column) = value;
    theCovarianceMatrix(column,row) = value;

  }

  isDiagonal_ = true;
  checkOffDiagonal(isDiagonal_);

  computeDecomposition();

}


void CorrelatedNoisifier::computeDecomposition()
{

  for ( int i = 0 ; i < theSize ; i++ ) {
    for ( int j = 0 ; j < theSize ; j++ ) {
      theMatrix(i,j) = 0.;
    }
  }

  double sqrtSigma00 = theCovarianceMatrix(0,0);
  if ( sqrtSigma00 <= 0. ) {
    throw cms::Exception("CorrelatedNoisifier") << "non positive variance.";
  }
  sqrtSigma00 = std::sqrt(sqrtSigma00);

  for ( int i = 0 ; i < theSize ; i++ )
    {
      double hi0 = theCovarianceMatrix(i,0)/sqrtSigma00;
      theMatrix(i,0) = hi0;
    }

  for ( int i = 1 ; i < theSize ; i++ ) 
    {

      for ( int j = 1 ; j < i ; j++ )
        {
          double hij = theCovarianceMatrix(i,j);
          for ( int k = 0 ; k <= j-1 ; k++ ) hij -= theMatrix(i,k)*theMatrix(j,k);
          hij /= theMatrix(j,j);
          theMatrix(i,j) = hij;
        }
      
      double hii = theCovarianceMatrix(i,i);
      for ( int j = 0 ; j <= i-1 ; j++ ) {
        double hij = theMatrix(i,j);
        hii -= hij*hij;
      }
      hii = sqrt(hii);
      theMatrix(i,i) = hii;

    }

}

void CorrelatedNoisifier::checkOffDiagonal(bool & isDiagonal_){

  isDiagonal_ = true;

  for ( int i = 0 ; i < theSize ; i++ ) {
    for ( int j = 0 ; j < theSize ; j++ ) {

      if ( i != j && theCovarianceMatrix(i,j) != 0. ) { isDiagonal_ = false ; return ; }
      
    }
  }
  

}