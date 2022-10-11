//
// A very simple test of PhysicalConstants.h
//

#include "larcoreobj/SimpleTypesAndConstants/PhysicalConstants.h"
#include <iomanip>
#include <iostream>
#include <sstream>

int main()
{

  int nbad = 0;

  std::ostringstream os1, os2;
  os1 << std::setprecision(8) << util::kRecombk;
  os2 << "0.0486";
  if (os1.str() != os2.str()) {
    std::cout << "compare --" << os1.str() << "-- to --" << os2.str() << "--" << std::endl;
    nbad++;
  }
  os1.str(std::string());
  os2.str(std::string());
  os1 << std::setprecision(8) << util::kRecombA;
  os2 << "0.8";
  if (os1.str() != os2.str()) {
    std::cout << "compare --" << os1.str() << "-- to --" << os2.str() << "--" << std::endl;
    nbad++;
  }
  os1.str(std::string());
  os2.str(std::string());
  os1 << std::fixed << std::showpoint << std::setprecision(3) << util::kModBoxA;
  os2 << "0.930";
  if (os1.str() != os2.str()) {
    std::cout << "compare --" << os1.str() << "-- to --" << os2.str() << "--" << std::endl;
    nbad++;
  }
  os1.str(std::string());
  os2.str(std::string());
  os1 << util::kModBoxB;
  os2 << "0.212";
  if (os1.str() != os2.str()) {
    std::cout << "compare --" << os1.str() << "-- to --" << os2.str() << "--" << std::endl;
    nbad++;
  }
  os1.str(std::string());
  os2.str(std::string());
  os1 << util::kGeVToElectrons;
  os2 << "42370000.000";
  if (os1.str() != os2.str()) {
    std::cout << "compare --" << os1.str() << "-- to --" << os2.str() << "--" << std::endl;
    nbad++;
  }
  os1.str(std::string());
  os2.str(std::string());
  os1 << std::setprecision(1) << util::kMeterToCentimeter;
  os2 << "100.0";
  if (os1.str() != os2.str()) {
    std::cout << "compare --" << os1.str() << "-- to --" << os2.str() << "--" << std::endl;
    nbad++;
  }
  os1.str(std::string());
  os2.str(std::string());
  os1 << std::setprecision(4) << util::kCentimeterToMeter;
  os2 << "0.0100";
  if (os1.str() != os2.str()) {
    std::cout << "compare --" << os1.str() << "-- to --" << os2.str() << "--" << std::endl;
    nbad++;
  }
  os1.str(std::string());
  os2.str(std::string());
  os1 << util::kMeterToKilometer;
  os2 << "0.0010";
  if (os1.str() != os2.str()) {
    std::cout << "compare --" << os1.str() << "-- to --" << os2.str() << "--" << std::endl;
    nbad++;
  }
  os1.str(std::string());
  os2.str(std::string());
  os1 << util::kKilometerToMeter;
  os2 << "1000.0000";
  if (os1.str() != os2.str()) {
    std::cout << "compare --" << os1.str() << "-- to --" << os2.str() << "--" << std::endl;
    nbad++;
  }
  os1.str(std::string());
  os2.str(std::string());
  os1 << std::setprecision(6) << util::keVToMeV;
  os2 << "0.000001";
  if (os1.str() != os2.str()) {
    std::cout << "compare --" << os1.str() << "-- to --" << os2.str() << "--" << std::endl;
    nbad++;
  }
  os1.str(std::string());
  os2.str(std::string());
  os1 << std::setprecision(1) << util::kMeVToeV;
  os2 << "1000000.0";
  if (os1.str() != os2.str()) {
    std::cout << "compare --" << os1.str() << "-- to --" << os2.str() << "--" << std::endl;
    nbad++;
  }
  //static constexpr double kBogusD            = -999.;                 ///< obviously bogus double value
  //static constexpr int    kBogusI            = -999;                  ///< obviously bogus integer value
  //static constexpr float  kBogusF            = -999.;                 ///< obviously bogus float value
  //static constexpr double quietCompiler = kBogusD*kBogusI*kBogusF*kRecombA*kRecombk*kGeVToElectrons;
  os1.str(std::string());
  os2.str(std::string());
  os1 << std::setprecision(1) << util::kBogusD;
  os2 << "-999.0";
  if (os1.str() != os2.str()) {
    std::cout << "compare ->" << os1.str() << "-- to --" << os2.str() << "--" << std::endl;
    nbad++;
  }
  os1.str(std::string());
  os1 << std::setprecision(1) << util::kBogusF;
  if (os1.str() != os2.str()) {
    std::cout << "compare ->" << os1.str() << "-- to --" << os2.str() << "--" << std::endl;
    nbad++;
  }
  os1.str(std::string());
  os2.str(std::string());
  os1 << std::setprecision(1) << util::kBogusI;
  os2 << "-999";
  if (os1.str() != os2.str()) {
    std::cout << "compare ->" << os1.str() << "-- to --" << os2.str() << "--" << std::endl;
    nbad++;
  }
  os1.str(std::string());
  os2.str(std::string());
  os1 << std::setprecision(1) << util::quietCompiler;
  os2 << "-1642408503589454.5";
  if (os1.str() != os2.str()) {
    std::cout << "compare ->" << os1.str() << "-- to --" << os2.str() << "--" << std::endl;
    nbad++;
  }

  // just exercise the conversion functions
  util::DegreesToRadians(util::RadiansToDegrees(util::pi<float>()));
  util::DegreesToRadians(util::RadiansToDegrees(util::pi<double>()));
  util::DegreesToRadians(util::RadiansToDegrees(util::pi<long double>()));

  return nbad;
}
