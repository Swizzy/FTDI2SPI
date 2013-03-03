#pragma once
#include "stdafx.h"
#include "XSPI.h"
#include "wrapper.h"

bool XNAND_InitDevice() {
	return Wrapper_InitDevice();
}