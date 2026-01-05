#include "SettingsPresenter.h"
#include "Comm/Usb.h"
#include "SettingsView.h"
#include "utils/NetworkHelper.h"
#include "utils/StorageHelper.h"
#include <fstream>

namespace UI
{
	void ConnectionSettingsPresenter::onInit() {}

	void DeveloperSettingsPresenter::startHardwareTest()
	{
		auto& hardwareTest = getView()->getHardwareTest();
		hardwareTest.show(true);

		/* Skip the serial number entry screen since the end user won't know this */
		hardwareTest.getPresenter()->m_serialNumber = "unknown";
		hardwareTest.getPresenter()->createLogFile();
		hardwareTest.getPresenter()->nextTest();
	}
} // namespace UI
