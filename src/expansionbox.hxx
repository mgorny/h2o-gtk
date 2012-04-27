/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#pragma once

#ifndef _H2O_GTK_EXPANSIONBOX_HXX
#define _H2O_GTK_EXPANSIONBOX_HXX 1

#include "dataentrypair.hxx"
#include "datainputoutput.hxx"

#include <h2o>

#include <sigc++/signal.h>
#include <gtkmm/label.h>
#include <gtkmm/separator.h>
#include <gtkmm/table.h>

class RealExpansionInputOutput : public LockedDataInputOutput
{
	typedef sigc::signal<void>
		eta_changed_sig;

	void eta_change_handler();

protected:
	eta_changed_sig eta_changed;

	DataEntryPair eta;

public:
	RealExpansionInputOutput(Gtk::Table& t, int first_row,
			double start_eta, int first_col = 0);

	void recalc(h2o::H2O& in, h2o::H2O& out);

	eta_changed_sig signal_eta_changed();
};

class ExpansionBox : public Gtk::Table
{
	typedef sigc::signal<void, h2o::H2O*, int>
		data_changed_sig;

	h2o::H2O cached_data[2];

	void input_changed(h2o::H2O* data, int len);
	void output_changed(h2o::H2O* data, int len);
	void real_changed(h2o::H2O* data, int len);
	void eta_changed();

protected:
	DataInput in_io;
	Gtk::HSeparator sep;
	Gtk::VSeparator vsep;
	LockedDataInputOutput out_io;
	RealExpansionInputOutput real_io;
	data_changed_sig data_changed;

public:
	ExpansionBox();

	void recalc();
	data_changed_sig signal_data_changed();
};

#endif /*_H2O_GTK_EXPANSIONBOX_HXX*/
