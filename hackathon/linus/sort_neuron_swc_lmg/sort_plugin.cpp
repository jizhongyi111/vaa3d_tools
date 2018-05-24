/*
 *  sort_func.cpp
 *  io functions for domenu and do func 
 *
 *  2018-05-24 : by Linus Manubens Gil
 *  adapted from 2012-02-01 : by Yinan Wan
 */
 
#include "v3d_message.h"

#include "sort_plugin.h"
#include "sort_func.h"
 
Q_EXPORT_PLUGIN2(sort_neuron_swc_lmg, SORTPlugin);
 
QStringList SORTPlugin::menulist() const
{
	return QStringList()
        <<tr("sort_swc_lmg")
		<<tr("about");
}

QStringList SORTPlugin::funclist() const
{
	return QStringList()
        <<tr("sort_swc_lmg")
		<<tr("help");
}

void SORTPlugin::domenu(const QString &menu_name, V3DPluginCallback2 &callback, QWidget *parent)
{
    if (menu_name == tr("sort_swc_lmg"))
	{
		sort_menu(callback,parent);
	}
	else if (menu_name == tr("help"))
	{
		printHelp(callback,parent);
	}
	else
	{
		printHelp(callback,parent);
	}
}

bool SORTPlugin::dofunc(const QString & func_name, const V3DPluginArgList & input, V3DPluginArgList & output, V3DPluginCallback2 & callback,  QWidget * parent)
{
    if (func_name == tr("sort_swc_lmg"))
	{
		sort_func(input, output);
	  return true;
	}
	else if (func_name == tr("help"))
	{
		printHelp(input,output);
	  return true;
	}
	else if (func_name == tr("TOOLBOXsort_swc"))
	{
		sort_toolbox(input);
		return true;
	}
	return false;
}

