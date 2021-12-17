/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/
#pragma once

#ifndef USING_CEF_SHARED
#define USING_CEF_SHARED 1
#endif

#include <include/cef_version.h>

#define UR_CHROM_VERSION_EQ(Am, Ai, Ab) \
	(CHROME_VERSION_MAJOR == Am && \
	CHROME_VERSION_MINOR == Ai && \
	CHROME_VERSION_BUILD == Ab)

#define UR_CHROM_VERSION_MIN(Am, Ai, Ab) \
	(CHROME_VERSION_MAJOR >= Am || \
	(CHROME_VERSION_MAJOR == Am && CHROME_VERSION_MINOR >= Ai) || \
	(CHROME_VERSION_MAJOR == Am && CHROME_VERSION_MINOR == Ai && CHROME_VERSION_BUILD >= Ab))

#define UR_CHROM_VERSION_BELOW(Am, Ai, Ab) \
	(CHROME_VERSION_MAJOR < Am || \
	(CHROME_VERSION_MAJOR == Am && CHROME_VERSION_MINOR < Ai) || \
	(CHROME_VERSION_MAJOR == Am && CHROME_VERSION_MINOR == Ai && CHROME_VERSION_BUILD < Ab))

#define UR_CHROM_VERSION_MAX(Am, Ai, Ab) \
	UR_CHROM_VERSION_BELOW(Am, Ai, Ab) || UR_CHROM_VERSION_EQ(Am, Ai, Ab)

#define UR_CHROM_VERSION_RANGE(Am, Ai, Ab, Bm, Bi, Bb) \
	UR_CHROM_VERSION_MIN(Am, Ai, Ab) && UR_CHROM_VERSION_BELOW(Bm, Bi, Bb)

#define UR_CHROM_COMPAT_EXTRAINFO() UR_CHROM_VERSION_MIN(79, 0, 0)