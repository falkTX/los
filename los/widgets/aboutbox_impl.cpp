#include "aboutbox_impl.h"
#include "config.h"
#include "icons.h"

AboutBoxImpl::AboutBoxImpl()
{
	setupUi(this);
	imageLabel->setPixmap(*aboutLOSImage);
	QString version(VERSION);
	versionLabel->setText("Version: " + version);
}
