 /*
* This file and associated .cpp file are licensed under the GPLv3 License Copyright (c) 2024 Sam Groveman
* 
* DFRobot Peristaltic Pump: https://www.dfrobot.com/product-1698.html
*
* Contributors: Sam Groveman
*/

#pragma once
#include <Arduino.h>
#include <DFPeristalticPump.h>
#include <PeriodicTask.h>
#include <ParameterTrigger.h>

/// @brief Adds auto triggering to DFPeristalticPump
class DFAutoPeristalticPump : public DFPeristalticPump, public PeriodicTask {
	private:
		/// @brief Additional settings required
		struct {
			/// @brief Threshold below which to deliver a dose
			int threshold;

			/// @brief If the pump should activate on a low reading
			bool activeLow;
		} add_config;
		
		/// @brief Used to track desired parameters 
		ParameterTrigger trigger;

		bool enableAuto(bool enable);
		JsonDocument addAdditionalConfig();
	public:
		DFAutoPeristalticPump(int Pin, String ConfigFile = "DFAutoPump.json");
		bool begin();
		void runTask(long elapsed);
		String getConfig();
		bool setConfig(String config, bool save);
};