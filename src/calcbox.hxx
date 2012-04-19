/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#pragma once

#ifndef _H2O_GTK_CALCBOX_HXX
#define _H2O_GTK_CALCBOX_HXX 1

#include "datainputoutput.hxx"

#include <h2o>

#include <sigc++/signal.h>
#include <gtkmm/label.h>
#include <gtkmm/table.h>

class CalcBox : public Gtk::Table
{
	typedef sigc::signal<void, h2o::H2O*, int>
		data_changed_sig;

protected:
	DataInputOutput data_io;

public:
	CalcBox();

	void recalc();
	data_changed_sig signal_data_changed();
};

#endif /*_H2O_GTK_CALCBOX_HXX*/
