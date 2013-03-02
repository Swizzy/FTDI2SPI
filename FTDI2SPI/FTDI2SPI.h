#pragma once
#ifdef COMPILE_FTDI2SPI
  #define FTDI2SPI_EXPORT __declspec(dllexport)
#else
  #define FTDI2SPI_EXPORT __declspec(dllimport)
#endif
