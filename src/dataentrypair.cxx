/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <cassert>

#include "dataentrypair.hxx"

DataEntryPair::DataEntryPair(const char* desc, const char* unit,
			double min, double max, double step, double pagestep,
			double decplaces, double val)
	: Gtk::SpinButton(0, decplaces),
	def_min(min), def_max(max),
	attached(false),
	label(desc, Gtk::ALIGN_END, Gtk::ALIGN_CENTER, true),
	unit_label(unit, Gtk::ALIGN_START, Gtk::ALIGN_CENTER)
{
	get_adjustment()->configure(val, min, max, step, pagestep, 0);

	label.set_mnemonic_widget(*this);
}

void DataEntryPair::add_to_table(Gtk::Table& t, int row)
{
	assert(!attached);

	t.attach(label, 0, 1, row, row + 1);
	t.attach(*this, 1, 2, row, row + 1);
	t.attach(unit_label, 2, 3, row, row + 1);

	attached = true;
}

void DataEntryPair::remove_from_table(Gtk::Table& t)
{
	if (attached)
	{
		t.remove(label);
		t.remove(*this);
		t.remove(unit_label);

		attached = false;
	}
}

void DataEntryPair::enable()
{
	set_editable(true);
	set_sensitive(true);
	set_range(def_min, def_max);
}

void DataEntryPair::disable()
{
	double val = get_value();

	set_editable(false);
	set_range(val, val);
}

void DataEntryPair::set_readonly_value(double val)
{
	set_range(val, val);
#if 0 // set_range() should be enough
	Gtk::SpinButton::set_value(val);
#endif
}
