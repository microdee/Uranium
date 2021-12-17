/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#pragma once

namespace UraniumCefUtils
{
	/**
	 * Load the required modules for CEF3
	 */
	void CEFCOMMON_API LoadCEF3Modules();

	/**
	 * Unload the required modules for CEF3
	 */
	void CEFCOMMON_API UnloadCEF3Modules();
};