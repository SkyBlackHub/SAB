#include "AbstractModule.h"

#include "Application.h"

SAB::AbstractModule::AbstractModule(QObject *parent) : QObject{parent}, sounds(qApp->sounds()), windows(qApp->windows())
{

}

QClipboard *SAB::AbstractModule::clipboard()
{
	return qApp->clipboard();
}
