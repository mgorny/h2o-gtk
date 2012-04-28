/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#pragma once

#ifndef _H2O_GTK_DATAENTRYPAIR_HXX
#define _H2O_GTK_DATAENTRYPAIR_HXX 1

#include <h2o>

#include <gtkmm/label.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/table.h>

class DataEntryPair : public Gtk::SpinButton
{
	typedef double (h2o::H2O::*MediumProperty)() const;

	double def_min, def_max;
	bool attached;

protected:
	Gtk::Label label;
	Gtk::Label unit_label;

public:
	DataEntryPair(const char* desc, const char* unit,
			double min, double max, double step, double pagestep,
			double decplaces, double val = 0);

	void add_to_table(Gtk::Table& t, int row, int col = 0);
	void remove_from_table(Gtk::Table& t);

	void enable();
	void disable();

	void set_readonly_value(double newval);
	void set_readonly_value(const h2o::H2O& medium, MediumProperty prop);
};

#endif /*_H2O_GTK_DATAENTRYPAIR_HXX*/
