/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#pragma once

#ifndef _H2O_GTK_SATURATIONBOX_HXX
#define _H2O_GTK_SATURATIONBOX_HXX 1

#include "dataentrypair.hxx"
#include "datainputoutput.hxx"

#include <h2o>

#include <sigc++/functors/mem_fun.h>
#include <sigc++/connection.h>
#include <sigc++/signal.h>
#include <gtkmm/separator.h>
#include <gtkmm/table.h>

class SaturationBox : public Gtk::Table
{
	typedef sigc::signal<void, h2o::H2O*, int>
		data_changed_sig;

protected:
	DataEntryPair p, T, r;
	DataOutput prim, bis;
	Gtk::Label prim_label, bis_label;
	Gtk::HSeparator sep;
	Gtk::VSeparator vsep;
	data_changed_sig data_changed;

public:
	SaturationBox();

	void recalc();
	void recalc_from_T();
	void reorder_fields();

	data_changed_sig signal_data_changed();
};

#endif /*_H2O_GTK_SATURATIONBOX_HXX*/
