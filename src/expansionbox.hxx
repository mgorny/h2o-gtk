/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#pragma once

#ifndef _H2O_GTK_EXPANSIONBOX_HXX
#define _H2O_GTK_EXPANSIONBOX_HXX 1

#include "datainputoutput.hxx"

#include <h2o>

#include <sigc++/signal.h>
#include <gtkmm/label.h>
#include <gtkmm/separator.h>
#include <gtkmm/table.h>

class ExpansionBox : public Gtk::Table
{
	typedef sigc::signal<void, h2o::H2O*, int>
		data_changed_sig;

	h2o::H2O cached_input;

	void input_changed(h2o::H2O* data, int len);
	void output_changed(h2o::H2O* data, int len);

protected:
	DataInput in_io;
	Gtk::HSeparator sep;
	LockedDataInputOutput out_io;
	data_changed_sig data_changed;

public:
	ExpansionBox();

	void recalc();
	data_changed_sig signal_data_changed();
};

#endif /*_H2O_GTK_EXPANSIONBOX_HXX*/
