/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#pragma once

#ifndef _H2O_GTK_DATAENTRYPAIR_HXX
#define _H2O_GTK_DATAENTRYPAIR_HXX 1

#include <gtkmm/label.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/table.h>

class DataEntryPair : public Gtk::SpinButton
{
	double def_min, def_max;

protected:
	Gtk::Label label;
	Gtk::Label unit_label;

public:
	DataEntryPair(const char* desc, const char* unit,
			double min, double max, double step, double pagestep,
			double decplaces, double val = 0);

	void add_to_table(Gtk::Table& t, int row);
	void remove_from_table(Gtk::Table& t);

	void enable();
	void disable();

	void set_value(double newval);
};

#endif /*_H2O_GTK_DATAENTRYPAIR_HXX*/
