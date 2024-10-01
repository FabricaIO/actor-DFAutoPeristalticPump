#include "DFAutoPeristalticPump.h"

/// @brief Creates a peristaltic pump object
/// @param Pin The pin to use
/// @param ConfigFile The file name to store settings in
DFAutoPeristalticPump::DFAutoPeristalticPump(int Pin, String ConfigFile) : DFPeristalticPump (Pin, ConfigFile) {}

bool DFAutoPeristalticPump::begin() {
	parameter_config.Parameters.resize(1);
	bool result = false;
	bool configExists = checkConfig(config_path);
	// Create settings directory if necessary
	if (DFPeristalticPump::begin()) {
		// Set description
		Description.type = "pump";
		Description.name = "Auto Peristaltic Pump";
		Description.id = 3;
		if (!configExists) {
			// Set defaults
			add_config.activeLow = false;
			add_config.threshold = 50;
			parameter_config.Enabled = false;
			parameter_config.Parameters[0] = "";
			task_config.taskName = "Auto Pump";
			task_config.taskPeriod = 1000;
			String settings;
			serializeJson(addAdditionalConfig(), settings);
			result = setConfig(settings, true);
		} else {
			// Load settings
			result = setConfig(Storage::readFile(config_path), false);
		}
	}
	return result;
}

/// @brief Enables the auto pump
/// @param enable True to enable, false to disable 
/// @return True on success
bool DFAutoPeristalticPump::enableAuto(bool enable) {
	return enableTask(enable);
}

/// @brief Runs the auto pump task
/// @param elapsed The amount of time, in ms, since this was last called
void DFAutoPeristalticPump::runTask(long elapsed) {
	if (taskPeriodTriggered(elapsed)) {
		double value = getParameterValues()[parameter_config.Parameters[0]];
		if (add_config.activeLow) {
			if (value < add_config.threshold) {
				dose();
				return;
			}
		} else {
			if (value > add_config.threshold) {
				dose();
				return;
			}
		}
	}
}

/// @brief Gets the current config
/// @return A JSON string of the config
String DFAutoPeristalticPump::getConfig() {
	JsonDocument doc = addAdditionalConfig();
	// Create string to hold output
	String output;
	// Serialize to string
	serializeJson(doc, output);
	return output;
}

/// @brief Sets the configuration for this device
/// @param config A JSON string of the configuration settings
/// @param save If the configuration should be saved to a file
/// @return True on success
bool DFAutoPeristalticPump::setConfig(String config, bool save) {
	if (DFPeristalticPump::setConfig(config, false)) {
		// Allocate the JSON document
		JsonDocument doc;
		// Deserialize file contents
		DeserializationError error = deserializeJson(doc, config);
		// Test if parsing succeeds.
		if (error) {
			Serial.print(F("Deserialization failed: "));
			Serial.println(error.f_str());
			return false;
		}
		// Assign loaded values
		parameter_config.Parameters[0] = doc["autoParameter"].as<String>();
		add_config.threshold = doc["threshold"].as<int>();
		parameter_config.Enabled = doc["autoEnabled"].as<bool>();
		add_config.activeLow = doc["activeLow"].as<bool>();
		task_config.taskName = doc["taskName"].as<std::string>();
		task_config.taskPeriod = doc["taskPeriod"].as<long>();
		if (save) {
			if (!saveConfig(config_path, config)) {
				return false;
			}
		}
		return enableAuto(parameter_config.Enabled);
	}
	return false;
}

/// @brief Collects all the base class parameters and additional parameters
/// @return a JSON document with all the parameters
JsonDocument DFAutoPeristalticPump::addAdditionalConfig() {
	// Allocate the JSON document
  	JsonDocument doc;
	// Deserialize file contents
	DeserializationError error = deserializeJson(doc, DFPeristalticPump::getConfig());
	// Test if parsing succeeds.
	if (error) {
		Serial.print(F("Deserialization failed: "));
		Serial.println(error.f_str());
		return doc;
	}
	doc["autoParameter"] = parameter_config.Parameters[0];
	doc["threshold"] = add_config.threshold;
	doc["autoEnabled"] = parameter_config.Enabled;
	doc["activeLow"] = add_config.activeLow;
	doc["taskName"] = task_config.taskName;
	doc["taskPeriod"] = task_config.taskPeriod;
	return doc;
}